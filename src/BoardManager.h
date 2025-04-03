#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <iostream>

class BoardManager
{
private:
	std::unordered_map<std::string, unsigned int> clientAccessCount;
	std::unordered_map<std::string, std::string> boardPassword;
public:
	BoardManager();

    int addBoard(const std::string& boardName, const std::string& boardPassword);

    bool validatePassword(const std::string& boardName, const std::string& password);
    
    void incrementAccessCount(const std::string& boardName);

    int getAccessCount(const std::string& boardName);

    void decrementAccessCount(const std::string& boardName);

    int boardExists(const std::string& board);
};

