#include <iostream>
#include <sstream>
#include <fstream>

#include <sqlite_modern_cpp.h>
#include <discordpp/bot.hh>
#include <discordpp/plugin-ratelimit.hh>
#include <discordpp/rest-beast.hh>
#include <discordpp/websocket-simpleweb.hh>

#include "token.cpp"
#include "jsonwrapper.cpp"
#include "sqlwrapper.cpp"
#include "apiwrapper.cpp"
using namespace sqlite;
using namespace std;

namespace asio = boost::asio;
using json = nlohmann::json;
namespace dpp = discordpp;

using DppBot = dpp::PluginRateLimit<dpp::WebsocketSimpleWeb<dpp::RestBeast<dpp::Bot>>>;

database db("discord_db.db");

string token;
static string submissions_id = "758776672186138624";
static string error_id = "758777881966018590";

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

void react(std::shared_ptr<DppBot> &bot, string channel_id, string message_id, string emoji)
{
	bot->call(
		make_shared<string>("PUT"),
		make_shared<string>("/channels/"+channel_id+"/messages/"+message_id+"/reactions/"+emoji+"/@me"),
		nullptr, nullptr, nullptr);
}

void request_messages(std::shared_ptr<DppBot> &bot, string channel_id, int count)
{
	assert(count >= 1 && count <= 100)
	bot->call(
}


void handle_message(std::shared_ptr<DppBot> &bot, json raw_msg)
{
	Message msg(raw_msg);
	if(msg.author_id == "758781959178289172")
		return;
	cout << "MESSAGE_CREATE\n";

	bool has_attachments = msg.attachments > 0;
	bool in_submissions = msg.channel_id == submissions_id;
	bool in_error = msg.channel_id == error_id;

	if(in_submissions && (!has_attachments || msg.content.size() < 5))
	{
		delete_message(bot, msg.channel_id, msg.message_id);
		send_message(bot, error_id,
				"<@"+msg.author_id+">, " + "All submissions must have both an attachment and a message of length >= 5. Your submission was deleted and not counted.");
	}
	else if(in_submissions)
	{
		// add this submission to our database
		Attachment attachment = msg.get_attachment();
		db << "insert into submissions (author, timestmp, description, link, filename, width, height, size) values (?,?,?,?,?,?,?,?);"
			<< msg.author_id
			<< msg.timestamp
			<< msg.content
			<< attachment.url
			<< attachment.filename
			<< attachment.width
			<< attachment.height
			<< attachment.size;
		cout << "Successfully inserted new entry, " << msg.author_id << " now has " << postcount(db, msg.author_id) << endl;

		react(bot, submissions_id, msg.message_id, "%F0%9F%86%97");
	}
	else if(in_error)
	{
		istringstream iss(msg.content);
		vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};
		string datastring="err";
		if(tokens.size() == 1)
		{
			if(tokens[0] == "postcount")
			{
				long cnt = postcount(db, msg.author_id);
				datastring = "You have made " + to_string(cnt) + " posts.";
			}
			else if(tokens[0] == "lastpost" && postcount(db, msg.author_id) > 0)
			{
				string url = latestposturl(db, msg.author_id);
				string desc = latestpostdescription(db, msg.author_id);
				datastring = desc + "\n" + url;
			}
		}
		send_message(bot, error_id,
				"<@"+msg.author_id+">, " + datastring);
	}
	cout << raw_msg << endl;
}


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
				[&bot](json raw_msg)
				{
					handle_message(bot, raw_msg);
				}
			});
	auto aioc = std::make_shared<asio::io_context>();

	bot->initBot(6, token, aioc);
	cout << "Initialization complete\n";

	cout << "Running...\n";
	bot->run();
	return 0;
}
