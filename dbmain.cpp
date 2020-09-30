#include <iostream>
#include <vector>
#include <sqlite3.h>
#include <climits>

sqlite3* DB;
bool verbose = 0;

std::vector<std::string> split(std::string in, char delim=' ')
{
	std::vector<std::string> out;
	std::string part = "";
	for(char c: in)
	{
		if(c != delim)
			part.push_back(c);
		else
		{
			out.push_back(part);
			part = "";
		}
	}
	if(part != "")
		out.push_back(part);
	return out;
}

void close(bool exit)
{
	std::cout << "Good night. Pleasant flight." << std::endl;
	sqlite3_close(DB);
	if(exit)
		std::exit(0);
}

void open()
{
	int exit = sqlite3_open("example.db", &DB);
	if(exit)
	{
		std::cerr << "Error opening DB " << sqlite3_errmsg(DB) << std::endl;
		close(true);
	}
	else
	{
		std::cout << "Opened DB successfully" << std::endl;
	}
}

//static int callback(void* data, int nrc, char** 

int get_posts_res;
int get_posts_cb(void* a, int b, char** c, char** d)
{
	for(int i = 0; i < b; i++)
	{
		if(std::string(d[i]) == "pstcnt")
		{
			get_posts_res = std::stoi(c[i]);
			return 0;
		}
	}
	std::cerr << "DIDNT FIND pstcnt" << std::endl;
	return 0;
}

//int get_post_count()
//{
	//std::string data = "CALLBACK FUNCTION";
	//std::string req = "SELECT MAX(rowid) FROM PostCount";
	//if(verbose)
		//std::cout << req << std::endl;
	//sqlite3_exec(DB, req.c_str(), get_post_count_cb(), (void*)data.c_str()
//}

int get_posts(long id)
{
	std::string data = "CALLBACK FUNCTION";
	std::string req = "SELECT * FROM PostCount WHERE usrid = " + std::to_string(id);
	if(verbose)
		std::cout << req << std::endl;
	sqlite3_exec(DB, req.c_str(), get_posts_cb, (void*)data.c_str(), NULL);
	return get_posts_res;
}

int set_posts_cb(void* a, int b, char** c, char** d)
{
	std::cout << "set_posts_cb" << std::endl;
	for(int i = 0; i < b; i++)
	{
		std::cout << d[i] << " = " << c[i] << std::endl;
	}
	return 0;
}

int set_posts(long id, int value)
{
	if(get_posts(id) == 0)
	{
		insert_posts(id, value);
		return;
	}
	std::string data = "CALLBACK FUNCTION";
	std::string req = "UPDATE PostCount SET pstcnt = " + std::to_string(value) + " WHERE usrid = " + std::to_string(id);
	if(verbose)
		std::cout << req << std::endl;
	int err = sqlite3_exec(DB, req.c_str(), set_posts_cb, (void*)data.c_str(), NULL);
	if(err)
		std::cerr << "err:" << err << std::endl;
	return 0;
}

int main(int argc, char** argv)
{
	open();
	//std::cout << (*argv) << std::endl;
	std::string input;
	std::vector<std::string> cmd;
	while(std::cin >> input)
	{
		if(input.size() >= 1 && input[0] != '_')
		{
			cmd.push_back(input);
		}
		else
		{
			if(cmd.size() == 0)
				continue;
			//std::cout << "->" << cmd[0] << std::endl;
			if(cmd[0] == "q" || cmd[0] == "exit" || cmd[0] == "quit")
				break;
			if(cmd[0] == "verbose")
			{
				std::cout << "VERBOSE ON" << std::endl;
				verbose=true;
				std::cout << "OK" << std::endl;
			}
			else if(cmd[0] == "get" && cmd.size() == 2)
			{
				int pc = get_posts(std::stoll(cmd[1]));
				std::cout << pc << std::endl;
				std::cout << "OK" << std::endl;
			}
			else if(cmd[0] == "set" && cmd.size() == 3)
			{
				long id = std::stoll(cmd[1]);
				int value = std::stoi(cmd[2]);
				set_posts(id, value);
				std::cout << "OK" << std::endl;
			}
			cmd.clear();
		}
	}
	close(false);
}
