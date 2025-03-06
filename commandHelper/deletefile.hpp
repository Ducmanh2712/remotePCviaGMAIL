#pragma once
#include <iostream>
using namespace std;

string extractPathForDel(const std::string& command);

string deleteFileAtPath(const std::string& path);