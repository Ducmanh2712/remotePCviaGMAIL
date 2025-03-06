#include <iostream>
#include <string>
#include <curl/curl.h>
#include "../include/json.hpp"
#include "../callBack/callBack.hpp"

using json = nlohmann::json;


// Variables for OAuth 2.0
const std::string CLIENT_ID = "YOUR_CLIENT_ID";
const std::string CLIENT_SECRET = "YOUR_CLIENT_SECRET";
const std::string REDIRECT_URI = "YOUR_REDIRECT_URI";
const std::string TOKEN_ENDPOINT = "YOUR_TOKEN_ENDPOINT";
const std::string AUTHORIZATION_ENDPOINT = "YOUR_AUTHORIZATION_ENDPOINT";
const std::string SCOPE = "YOUR_SCOPE";

// Helper function to write response data from libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t total_size = size * nmemb;
    userp->append(static_cast<char*>(contents), total_size);
    return total_size;
}



// Step 1: Generate the authorization URL
std::string GenerateAuthorizationURL() {
    return AUTHORIZATION_ENDPOINT + "?client_id=" + CLIENT_ID +
           "&redirect_uri=" + REDIRECT_URI +
           "&response_type=code&scope=" + SCOPE;
}

// Step 2: Exchange authorization code for an access token
std::string GetAccessToken(const std::string& auth_code) {
    CURL* curl;
    CURLcode res;
    std::string response;
    curl = curl_easy_init();

    if (curl) {
        // Prepare POST data
        std::string post_fields = "code=" + auth_code +
                                  "&client_id=" + CLIENT_ID +
                                  "&client_secret=" + CLIENT_SECRET +
                                  "&redirect_uri=" + REDIRECT_URI +
                                  "&grant_type=authorization_code";

        curl_easy_setopt(curl, CURLOPT_URL, TOKEN_ENDPOINT.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());

        // Set callback for response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return "";
        }

        curl_easy_cleanup(curl);
    }

    return response;
}

// Step 3: Parse the JSON response to extract the access token
void ParseAndPrintToken(const std::string& json_response) {
    try {
        // Parse JSON using nlohmann/json
        json parsed = json::parse(json_response);

        // Extract values from the JSON object
        std::string access_token = parsed["access_token"];
        int expires_in = parsed["expires_in"];
        std::string refresh_token = parsed["refresh_token"];
        std::string scope = parsed["scope"];
        std::string token_type = parsed["token_type"];

        // Print the values
        std::cout << "Access Token: " << access_token << std::endl;
        std::cout << "Expires In: " << expires_in << " seconds" << std::endl;
        std::cout << "Refresh Token: " << refresh_token << std::endl;
        std::cout << "Scope: " << scope << std::endl;
        std::cout << "Token Type: " << token_type << std::endl;
    } catch (json::exception& e) {
        std::cerr << "JSON Parsing error: " << e.what() << std::endl;
    }
}

int main() {
    // Step 1: Generate and display the authorization URL for the user
    std::string auth_url = GenerateAuthorizationURL();
    std::cout << "Open this URL in your browser and authorize the application: " << std::endl;
    std::cout << auth_url << std::endl;

    // Step 2: Get the authorization code from the user
    std::string auth_code = "YOUR_AUTH_CODE";

    // Step 3: Exchange the authorization code for an access token
    std::string json_response = GetAccessToken(auth_code);

    // Step 4: Parse and display the access token and other details
    if (!json_response.empty()) {
        ParseAndPrintToken(json_response);
    } else {
        std::cerr << "Failed to retrieve access token." << std::endl;
    }

    return 0;
}