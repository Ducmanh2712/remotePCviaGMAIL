#pragma once
#include <iostream>
using namespace std;
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* out);
size_t IgnoreCallback(void* contents, size_t size, size_t nmemb, void* userp);