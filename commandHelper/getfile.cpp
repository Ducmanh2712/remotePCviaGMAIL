#include <regex>
#include <filesystem>
#include "../commandHelper/getfile.hpp"

std::string extractPath(const std::string& command) {
    std::regex pattern(R"(^getfile\s+-p\s+(.+))");  // Regex để nhận diện lệnh và đường dẫn
    std::smatch matches;
    
    if (std::regex_match(command, matches, pattern) && matches.size() > 1) {
        return matches[1];  // Trả về đường dẫn được trích xuất
    }
    return "";  // Trả về chuỗi rỗng nếu không đúng định dạng
}

string getFileAtPath(const std::string& path) {
    namespace fs = std::filesystem;
    if (fs::exists(path)) {
        try {
            return "Here your file:";
        } catch (const fs::filesystem_error& e) {
            return "Failed to find: " + path;
        }
    } else {
        return "File not found " + path;
    }
}