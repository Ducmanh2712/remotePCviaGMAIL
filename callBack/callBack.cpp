#include "../callBack/callBack.hpp"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* out) {
    size_t totalSize = size * nmemb;
    out->append((char*)contents, totalSize);
    return totalSize;
}
size_t IgnoreCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb; // Trả về đúng kích thước mà không làm gì cả.
}