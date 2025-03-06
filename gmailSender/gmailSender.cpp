#include <curl/curl.h>
#include <fstream>
#include "../gmailSender/gmailSender.hpp"
#include "../commandHelper/screencapture.hpp"
#include "../base64_encode/base64_encode.hpp"
#include "../callBack/callBack.hpp"

// Hàm tạo email dưới dạng Base64
string createEmail(const string& to, const string& body) {
    ostringstream rawEmail;
    rawEmail << "To: " << to << "\r\n"
             << "Subject: " << "[AUTO-REPLY]" << "\r\n"
             << "Content-Type: text/plain; charset=\"UTF-8\"\r\n"
             << "\r\n"
             << body;

    string rawEmailStr = rawEmail.str();
    string base64Email = base64_encode(
        reinterpret_cast<const unsigned char*>(rawEmailStr.data()), rawEmailStr.size());
    return base64Email;
}

string createEmailWithAttachment(const string& to, const string& body, const string& attachmentBase64, const string& attachmentName) {
    string boundary = "----=_Part_0_00123456_78901234";
    
    ostringstream rawEmail;
    rawEmail << "To: " << to << "\r\n"
             << "Subject: " << "[AUTO-REPLY]" << "\r\n"
             << "Content-Type: multipart/mixed; boundary=\"" << boundary << "\"\r\n"
             << "\r\n"
             << "--" << boundary << "\r\n"
             << "Content-Type: text/plain; charset=\"UTF-8\"\r\n"
             << "\r\n"
             << body << "\r\n"
             << "\r\n"
             << "--" << boundary << "\r\n"
             << "Content-Type: image/jpeg; name=\"" << attachmentName << "\"\r\n"
             << "Content-Disposition: attachment; filename=\"" << attachmentName << "\"\r\n"
             << "Content-Transfer-Encoding: base64\r\n"
             << "\r\n"
             << attachmentBase64 << "\r\n"
             << "--" << boundary << "--\r\n";
    
    string rawEmailStr = rawEmail.str();
    string base64Email = base64_encode(reinterpret_cast<const unsigned char*>(rawEmailStr.data()), rawEmailStr.size());
    return base64Email;
}

bool sendEmailWithAttachment(const string& to, const string& body, const string& accessToken, const string& attachmentPath, const string& attachmentName) {
    // Mã hóa hình ảnh thành Base64
    string attachmentBase64 = encodeImageToBase64(attachmentPath);
    if (attachmentBase64.empty()) {
        std::cerr << "Failed to encode file." << std::endl; 
        return false;
    }

    // Tạo email có phần đính kèm hình ảnh
    string emailData = createEmailWithAttachment(to, body, attachmentBase64, attachmentName);
    
    string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/send";
    string postData = R"({"raw": ")" + emailData + R"("})";

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IgnoreCallback);
    CURLcode res = curl_easy_perform(curl);
    bool success = (res == CURLE_OK);
    if (!success) {
        std::cerr << "Failed to send email: " << curl_easy_strerror(res) << endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}


// Hàm gửi email qua Gmail API
bool sendEmail(const string& to, const string& body, const string& accessToken) {
    string emailData = createEmail(to, body);
    string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/send";
    string postData = R"({"raw": ")" + emailData + R"("})";

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IgnoreCallback);
    CURLcode res = curl_easy_perform(curl);
    bool success = (res == CURLE_OK);
    if (!success) {
        std::cerr << "Failed to send email: " << curl_easy_strerror(res) << endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

string senderEmail(string messageResponse){
    nlohmann::json jsonResponse;
    try {
        jsonResponse = nlohmann::json::parse(messageResponse);
    } catch (const nlohmann::json::exception& e) {
        cerr << "JSON parse error: " << e.what() << endl;
        return "";
    }
    
    for (const auto& headers : jsonResponse["payload"]["headers"]) {
        if (headers["name"] == "Subject" && headers["value"].get<string>().find("[AUTO-REPLY]") != string::npos) {
            return "AUTO_REPLY";  // Đánh dấu đây là email phản hồi tự động
        }
    }

    for (const auto& headers : jsonResponse["payload"]["headers"]){
        if (headers["name"] == "From"){
            return headers["value"];
        }
    }
    return "Unknown";
}
