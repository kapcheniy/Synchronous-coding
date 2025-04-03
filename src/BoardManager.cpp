#include "BoardManager.h"

BoardManager::BoardManager()
{
	addBoard("board1", "123");
	addBoard("board2", "secret");
}

int BoardManager::boardExists(const std::string& boardName)
{
	return clientAccessCount[boardName];
}

int BoardManager::addBoard(const std::string& boardName, const std::string& password)
{
	if (boardPassword.count(boardName)) return 1;
	boardPassword[boardName] = password;
	return 0;
}

bool BoardManager::validatePassword(const std::string& boardName, const std::string& password){
	return password == boardPassword[boardName];
}

void BoardManager::incrementAccessCount(const std::string& boardName) {
	clientAccessCount[boardName]++;
}

int BoardManager::getAccessCount(const std::string& boardName) {
	return clientAccessCount[boardName];
}

void BoardManager::decrementAccessCount(const std::string& boardName) {
	clientAccessCount[boardName]--;
}
