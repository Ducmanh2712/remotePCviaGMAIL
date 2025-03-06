#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <iostream>
#include "../commandHelper/screencapture.hpp"
#pragma comment(lib, "gdiplus.lib")


void SaveBitmapToFile(HBITMAP hBitmap, HDC hDC, const std::wstring& filename)
{
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader = {0};
    BITMAPINFOHEADER bi = {0};

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = -bmp.bmHeight;  // Negative height to store top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;

    DWORD dwBmpSize = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;

    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char* lpbitmap = (char*)GlobalLock(hDIB);

    GetDIBits(hDC, hBitmap, 0, (UINT)bmp.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwSizeofDIB;
    bmfHeader.bfType = 0x4D42;

    DWORD dwBytesWritten;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    CloseHandle(hFile);
}

// chup man hinh
void CaptureScreen(const std::wstring& filename)
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN); // Tọa độ X của màn hình ảo
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN); // Tọa độ Y của màn hình ảo

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, 1920, 1080);
    SelectObject(hDC, hBitmap);

    // Sao chép toàn bộ nội dung từ màn hình ảo
    BitBlt(hDC, 0, 0, 1920, 1080 , hScreen, x, y, SRCCOPY);
    SaveBitmapToFile(hBitmap, hDC, filename);

    DeleteObject(hBitmap);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    std::wcout << L"Screenshot saved as \"" << filename << L"\"" << std::endl;
}


// chup man hinh co theo kich thuoc tuy chon
void CaptureScreen(const std::wstring& filename, int x, int y, int width, int height)
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
    SelectObject(hDC, hBitmap);

    // Sử dụng BitBlt với tọa độ và kích thước tùy chọn
    BitBlt(hDC, 0, 0, width, height, hScreen, x, y, SRCCOPY);
    SaveBitmapToFile(hBitmap, hDC, filename);

    DeleteObject(hBitmap);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    std::wcout << L"Custom region screenshot saved as \"" << filename << L"\"" << std::endl;
}  
