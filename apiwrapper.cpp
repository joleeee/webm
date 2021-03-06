using DppBot = dpp::PluginRateLimit<dpp::WebsocketSimpleWeb<dpp::RestBeast<dpp::Bot>>>;
using json = nlohmann::json;

void send_message(	std::shared_ptr<DppBot> &bot, sqlite::database &db, std::string channel_id, std::string message)
{

	json tosend;
	tosend["content"] = message;
	bot->call(
		make_shared<std::string>("POST"),
		make_shared<std::string>("/channels/"+channel_id+"/messages"),
		make_shared<json>(tosend),
		nullptr, nullptr);
}

void delete_message(	std::shared_ptr<DppBot> &bot, sqlite::database &db, std::string channel_id, std::string message_id)
{
	bot->call(
		make_shared<std::string>("DELETE"),
		make_shared<std::string>("/channels/"+channel_id+"/messages/"+message_id),
		nullptr, nullptr, nullptr);
}

void react(		std::shared_ptr<DppBot> &bot, sqlite::database &db, std::string channel_id, std::string message_id, std::string emoji)
{
	bot->call(
		make_shared<std::string>("PUT"),
		make_shared<std::string>("/channels/"+channel_id+"/messages/"+message_id+"/reactions/"+emoji+"/@me"),
		nullptr, nullptr, nullptr);
}

void request_messages(	std::shared_ptr<DppBot> &bot, sqlite::database &db, std::string channel_id, int count)
{
	assert(count >= 1 && count <= 100)
	//bot->call(
}


void handle_message(	std::shared_ptr<DppBot> &bot, sqlite::database &db, json raw_msg)
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
		istd::stringstream iss(msg.content);
		vector<std::string> tokens{istream_iterator<std::string>{iss}, istream_iterator<std::string>{}};
		std::string datastd::string="err";
		if(tokens.size() == 1)
		{
			if(tokens[0] == "postcount")
			{
				long cnt = postcount(db, msg.author_id);
				datastd::string = "You have made " + to_std::string(cnt) + " posts.";
			}
			else if(tokens[0] == "lastpost" && postcount(db, msg.author_id) > 0)
			{
				std::string url = latestposturl(db, msg.author_id);
				std::string desc = latestpostdescription(db, msg.author_id);
				datastd::string = desc + "\n" + url;
			}
		}
		send_message(bot, error_id,
				"<@"+msg.author_id+">, " + datastd::string);
	}
	cout << raw_msg << endl;
}
