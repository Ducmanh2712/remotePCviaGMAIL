#pragma once
#include <iostream>
#include <gdiplus.h>
using namespace std;

void SaveBitmapToFile(HBITMAP hBitmap, HDC hDC, const std::wstring& filename);

void CaptureScreen(const std::wstring& filename);