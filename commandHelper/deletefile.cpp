#include <filesystem>  // C++17
#include <regex>
#include "../commandHelper/deletefile.hpp"

string extractPathForDel(const std::string& command) {
    std::regex pattern(R"(^deletefile\s+-p\s+(.+))");  // Regex để nhận diện lệnh và đường dẫn
    std::smatch matches;
    
    if (std::regex_match(command, matches, pattern) && matches.size() > 1) {
        return matches[1];  // Trả về đường dẫn được trích xuất
    }
    return "";  // Trả về chuỗi rỗng nếu không đúng định dạng
}

string deleteFileAtPath(const std::string& path) {
    namespace fs = std::filesystem;
    if (fs::exists(path)) {
        try {
            fs::remove(path);
            return "File deleted successfully: " + path;
        } catch (const fs::filesystem_error& e) {
            return "Failed to delete file: " + path;
        }
    } else {
        return "File not found " + path;
    }
}
