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
namespace dpp = discordpp;

using DppBot = dpp::PluginRateLimit<dpp::WebsocketSimpleWeb<dpp::RestBeast<dpp::Bot>>>;
using json = nlohmann::json;

database db("discord_db.db");

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
