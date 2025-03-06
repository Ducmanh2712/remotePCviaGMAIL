#include <curl/curl.h>
#include <fstream>
#include "../include/json.hpp"
#include "../callBack/callBack.hpp"
#include "__inIt__.hpp"
#include <filesystem>

Config loadConfig(const string& filePath) {
    ifstream configFile(filePath);
    nlohmann::json configJson;
    Config config;

    if (configFile.is_open()) {
        configFile >> configJson;
        config.refreshToken = configJson["refresh_token"];
        config.clientId = configJson["client_id"];
        config.clientSecret = configJson["client_secret"];
        config.userGmail = configJson["user_gmail"];
        if (configJson.contains("access_token")) {
            config.accessToken = configJson["access_token"];
        }
        configFile.close();
    } else {
        cerr << "ERROR: Cannot open config file: " << filePath << endl;
    }

    return config;
}

string makeGetRequest(const string& url, const string& token) {
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
    string response;

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());

        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl.get());
        if (res != CURLE_OK) {
            cerr << "Curl request failed: " << curl_easy_strerror(res) << endl;
        }
        curl_slist_free_all(headers);
    }

    return response;
}

string makePostRequest(const string& url, const string& data) {
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
    string response;

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl.get());
        if (res != CURLE_OK) {
            cerr << "Curl request failed: " << curl_easy_strerror(res) << endl;
        }
        curl_slist_free_all(headers);
    }

    return response;
}


void saveAccessTokenToConfig(const string& accessToken, const string& configPath) {
    // Đọc file config.json hiện tại.
    ifstream configFile(configPath);
    nlohmann::json configJson;
    if (configFile.is_open()) {
        configFile >> configJson;
        configFile.close();
    } else {
        cerr << "ERROR: Cannot open config file: " << configPath << endl;
        return;
    }

    // Thêm hoặc cập nhật thành phần accessToken.
    configJson["access_token"] = accessToken;

    // Ghi file config.json với accessToken mới.
    ofstream outFile(configPath);
    if (outFile.is_open()) {
        outFile << configJson.dump(4);  // Định dạng đẹp với thụt lề 4 spaces.
        outFile.close();
    } else {
        cerr << "ERROR: Cannot write to config file: " << configPath << endl;
    }
}

string refreshAccessToken(const string& refreshToken, const string& clientId, const string& clientSecret, const string& configPath) {
    string url = "https://oauth2.googleapis.com/token";
    string postData = "client_id=" + clientId +
                      "&client_secret=" + clientSecret +
                      "&refresh_token=" + refreshToken +
                      "&grant_type=refresh_token";

    string response = makePostRequest(url, postData);

    nlohmann::json jsonResponse = nlohmann::json::parse(response);
    if (jsonResponse.contains("access_token")) {
        string accessToken = jsonResponse["access_token"];
        saveAccessTokenToConfig(accessToken, configPath);  // Lưu accessToken vào config.json.
        return accessToken;
    } else {
        cerr << "ERROR: Cannot refresh access token: " << response << endl;
        return "";
    }
}


string __init_() {
    string configPath = "C:/Users/admin/Documents/HK3/Project_1/config.json";  // Đường dẫn tới file config.json.

    // Đọc file config.json.
    Config config = loadConfig(configPath);
    string accessToken = config.accessToken;  // Lấy accessToken từ file config.json.

    string listUrl = "https://gmail.googleapis.com/gmail/v1/users/"+config.userGmail+"/messages?q=in:inbox";
    string listResponse = makeGetRequest(listUrl, accessToken);

    nlohmann::json jsonResponse;
    try {
        jsonResponse = nlohmann::json::parse(listResponse);
    } catch (const nlohmann::json::exception& e) {
        cerr << "JSON parse error: " << e.what() << endl;
        return "";
    }

    // Kiểm tra lỗi 401 từ Gmail API.
    if (jsonResponse.contains("error") && jsonResponse["error"].contains("code") && jsonResponse["error"]["code"] == 401) {
        accessToken = refreshAccessToken(config.refreshToken, config.clientId, config.clientSecret, configPath);  // Làm mới token và lưu vào file config.json.
        listResponse = makeGetRequest(listUrl, accessToken);  // Gửi lại yêu cầu với token mới.
        jsonResponse = nlohmann::json::parse(listResponse);
    }

    if (jsonResponse.contains("messages") && !jsonResponse["messages"].empty()) {
        return jsonResponse["messages"][0]["id"];
    }
    return "";
}



