#pragma once

#include <iostream>
using namespace std;

string createEmail(const string& to, const string& body);

bool sendEmail(const string& to, const string& body, const string& accessToken);

string senderEmail(string messageResponse);

string createEmailWithAttachment(const string& to, const string& body, const string& attachmentBase64, const string& attachmentName);

bool sendEmailWithAttachment(const string& to, const string& body, const string& accessToken, const string& attachmentPath, const string& attachmentName);