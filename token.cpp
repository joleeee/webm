std::string get_token()
{
	std::string token;
	std::ifstream token_file("bot.token", std::ifstream::in);
	getline(token_file, token);
	token_file.close();
	std::cout << "token is " << token << std::endl;
	return token;
}
