#pragma once
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include "../include/json.hpp"
#include "../include/base64.hpp"
#include <iostream>
#include <string>

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

std::string encodeImageToBase64(const std::string& filePath);