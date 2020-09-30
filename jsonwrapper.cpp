using json = nlohmann::json;

struct Attachment
{
	std::string url, filename;
	long size, width, height;
	Attachment(json attach)
	{
		url	= attach["url"].get<std::string>();
		filename= attach["filename"].get<std::string>();
		size	= attach["size"].get<long>();
		width	= attach["width"].get<long>();
		height	= attach["height"].get<long>();
	}
};

struct Message
{
	std::string author_id, channel_id, message_id;
	std::string content, timestamp;
	int attachments;
	json raw_attachment;
	Message(json msg)
	{
		author_id = msg["author"]["id"].get<std::string>();
		channel_id = msg["channel_id"].get<std::string>();
		message_id = msg["id"].get<std::string>();

		content = msg["content"].get<std::string>();
		timestamp = msg["timestamp"].get<std::string>();

		attachments = msg["attachments"].size();

		if(attachments > 0)
		{
			raw_attachment = msg["attachments"][0];
		}
	}
	Attachment get_attachment()
	{
		if(attachments <= 0)
			throw "No attachments!";
		return Attachment(raw_attachment);
	}
};
