#pragma once
#include "SharedDocument.h"

class MessageHandler
{
private:
	websocket::stream<tcp::socket>& ws;
	SharedDocument& doc;
	BoardManager boardManager;
public: 

	MessageHandler(websocket::stream<tcp::socket>& ws, SharedDocument& doc)
		: ws(ws), doc(doc) {}
	int handle(const std::string& messange);
private:
	std::string create_response(bool success, const std::string& user, const std::string& board);

	void handle_message(const json& data);

	void handle_changes(const json& data);

	void handle_connection(const json& data);

	void handle_compile(const json& data);
};

