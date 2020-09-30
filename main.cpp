#include <iostream>
#include <sstream>
#include <fstream>

#include <sqlite_modern_cpp.h>
#include <discordpp/bot.hh>
#include <discordpp/plugin-ratelimit.hh>
#include <discordpp/rest-beast.hh>
#include <discordpp/websocket-simpleweb.hh>
using namespace sqlite;
using namespace std;

namespace asio = boost::asio;
using json = nlohmann::json;
namespace dpp = discordpp;

using DppBot = dpp::PluginRateLimit<dpp::WebsocketSimpleWeb<dpp::RestBeast<dpp::Bot>>>;

database db("discord_db.db");

string get_token()
{
	string token;
	std::ifstream token_file("bot.token", std::ifstream::in);
	getline(token_file, token);
	token_file.close();
	cout << "token is " << token << endl;
	return token;

}

int postcount(string id)
{
	long pcount;
	db << "select count(*) from submissions where author = ?;" << id >> pcount;
	return pcount;
}

string latestposturl(string id)
{
	long latestpostid;
	db << "select max(_id) from submissions where author = ?;" << id >> latestpostid;
	string url = "";
	db << "select link from submissions where _id = ?;" << latestpostid >> url;
	return url;
}

string latestpostdescription(string id)
{
	long latestpostid;
	db << "select max(_id) from submissions where author = ?;" << id >> latestpostid;
	string url = "";
	db << "select description from submissions where _id = ?;" << latestpostid >> url;
	return url;
}

void send_message(std::shared_ptr<DppBot> &bot, string channel_id, string message)
{

	json tosend;
	tosend["content"] = message;
	bot->call(
		make_shared<string>("POST"),
		make_shared<string>("/channels/"+channel_id+"/messages"),
		make_shared<json>(tosend),
		nullptr, nullptr);
}

void delete_message(std::shared_ptr<DppBot> &bot, string channel_id, string message_id)
{
	bot->call(
		make_shared<string>("DELETE"),
		make_shared<string>("/channels/"+channel_id+"/messages/"+message_id),
		nullptr, nullptr, nullptr);
}

void react(std::shard_ptr<DppBot> &bot, string channel_id, string message_id, string emoji)
{
	bot->call(
		make_shared<string>("PUT"),
		make_shared<string>("/channels/"+channel_id+"/messages/"+message_id+"/reactions/"+emoji+"/@me"),
		nullptr, nullptr, nullptr);
}

void insert_submission(

string token;
static string submissions_id = "758776672186138624";
static string error_id = "758777881966018590";
// todo webm of the day
int main() {
	// setup
	try {
		db <<
			"create table if not exists submissions ("
			"	_id integer primary key autoincrement not null,"
			"	author text not null,"
			"	link text not null,"
			"	filename text not null,"
			"	timestmp text not null,"
			"	width integer not null,"
			"	height integer not null,"
			"	size integer not null,"
			"	description text not null"
			");";

		cout << "DB Setup successful!\n";
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	token = get_token();

	auto bot = std::make_shared<DppBot>();
	bot->debugUnhandled = true;


	json self;
	bot->handlers.insert(
		{
			"READY",
			[&self](json data)
			{
				cout << "READY\n";
				self = data["user"];
			}
		});

	bot->handlers.insert(
			{
				"MESSAGE_CREATE",
				[&bot](json msg)
				{
					string author_id = msg["author"]["id"].get<string>();
					if(author_id == "758781959178289172")
						return;
					cout << "MESSAGE_CREATE\n";
					string channel_id = msg["channel_id"].get<string>();
					string message_id = msg["id"].get<string>();
					string content = msg["content"].get<string>();
					bool has_attachments = msg["attachments"].size() > 0;
					bool in_submissions = channel_id == submissions_id;
					bool in_error = channel_id == error_id;
					string timestamp = msg["timestamp"].get<string>();

					// todo break if sender is ourselves
					if(in_submissions && (!has_attachments || content.size() < 5))
					{
						delete_message(bot, channel_id, message_id);
						send_message(bot, error_id,
								"<@"+author_id+">, " + "All submissions must have both an attachment and a message of length >= 5. Your submission was deleted and not counted.");
					}
					else if(in_submissions)
					{
						// add this submission to our database
						json attachment = msg["attachments"][0];
						string url = attachment["url"].get<string>();
						long size = attachment["size"].get<long>();
						long h = attachment["height"].get<long>();
						long w = attachment["width"].get<long>();
						string filename = attachment["filename"].get<string>();
						db << "insert into submissions (author, link, filename, timestmp, width, height, size, description) values (?,?,?,?,?,?,?,?);"
							<< author_id
							<< url
							<< filename
							<< timestamp
							<< w
							<< h
							<< size
							<< content;
						cout << "Successfully inserted new entry, " << author_id << " now has " << postcount(author_id) << endl;

						react(bot, submissions_id, message_id, "%F0%9F%86%97");
					}
					else if(in_error)
					{
						istringstream iss(content);
						vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};
						string datastring="err";
						if(tokens.size() == 1)
						{
							if(tokens[0] == "postcount")
							{
								long cnt = postcount(author_id);
								datastring = "You have made " + to_string(cnt) + " posts.";
							}
							else if(tokens[0] == "lastpost" && postcount(author_id) > 0)
							{
								string url = latestposturl(author_id);
								string desc = latestpostdescription(author_id);
								datastring = desc + "\n" + url;
							}
						}
						send_message(bot, error_id,
								"<@"+author_id+">, " + datastring);
					}
					cout << msg << endl;
				}
			});
	auto aioc = std::make_shared<asio::io_context>();
	cout << "Initialization\n";
	bot->initBot(6, token, aioc);

	cout << "Running\n";
	bot->run();
	return 0;
}
