#pragma once
#include <iostream>

using namespace std;

struct Config {
    string refreshToken;
    string clientId;
    string clientSecret;
    string userGmail;
    string accessToken;  // Thêm trường accessToken.
};

Config loadConfig(const string& filePath);

string makeGetRequest(const string& url, const string& token);

string makePostRequest(const string& url, const string& data);

void saveAccessTokenToConfig(const string& accessToken, const string& configPath);

string refreshAccessToken(const string& refreshToken, const string& clientId, const string& clientSecret, const string& configPath);

string __init_();