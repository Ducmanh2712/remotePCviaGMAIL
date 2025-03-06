#pragma once
#include <iostream>
#include "../Source/__inIt__.hpp"

using namespace std;

class Server{
    string accessToken = "STARTING";
    string oldMessageId;
    string sender;
    Config config = loadConfig("../config.json");
public:
    Server();

    string getAccessToken();

    string getEmail(string& oldMessageId, string& accessToken);

    void processCommand(const string& command, const string& sender, const string& accessToken);

    void run();
};