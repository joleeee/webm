#include <sqlite_modern_cpp.h>

int postcount(sqlite::database &db, std::string &id)
{
	long pcount;
	db << "select count(*) from submissions where author = ?;" << id >> pcount;
	return pcount;
}

std::string latestposturl(sqlite::database &db, std::string &id)
{
	long latestpostid;
	db << "select max(_id) from submissions where author = ?;" << id >> latestpostid;
	std::string url = "";
	db << "select link from submissions where _id = ?;" << latestpostid >> url;
	return url;
}

std::string latestpostdescription(sqlite::database &db, std::string &id)
{
	long latestpostid;
	db << "select max(_id) from submissions where author = ?;" << id >> latestpostid;
	std::string url = "";
	db << "select description from submissions where _id = ?;" << latestpostid >> url;
	return url;
}

