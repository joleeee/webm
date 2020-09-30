#include<iostream>
#include <sqlite_modern_cpp.h>
#include <discordpp/bot.hh>
#include <discordpp/rest-beast.hh>
#include <discordpp/websocket-simpleweb.hh>
using namespace sqlite;
using namespace std;

namespace asio = boost::asio;
using json = nlohmann::json;
namespace dpp = discordpp;

using DppBot = dpp::WebsocketSimpleWeb<dpp::RestBeast<dpp::Bot>>;

database db("discord_db.db");

int postcount(long id)
{
	long pcount;
	db << "select count(*) from submissions where author = ?;" << id >> pcount;
	return pcount;
}

int main() {
	// setup
	try {
		db <<
			"create table if not exists submissions ("
			"	_id integer primary key autoincrement not null,"
			"	author int not null,"
			"	link text not null,"
			"	filename text not null"
			");";

		cout << "DB Setup successful!\n";
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	auto bot = std::make_shared<DppBot>();
	bot->debugUnhandled = true;

	json self;
	bot->handlers.insert(
			{"READY", [&self](json data) { self = data["user"]; }});

	bot->handlers.insert(
			{
				"MESSAGE_CREATE",
				[&bot](json msg)
				{
					cout << "msg\n";
					cout << msg << endl;
				}
			});
	auto aioc = std::make_shared<asio::io_context>();
	cout << "Initialization\n";
	string token = "";
	bot->initBot(6, token, aioc);
	cout << "Running\n";
	bot->run();
	return 0;
}
