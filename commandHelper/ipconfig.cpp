#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <sstream>  
#include "../commandHelper/ipconfig.hpp"
// #include "../base64_encode/base64_encode.hpp"
// #include "../callBack/callBack.hpp"


// Hàm chuyển đổi từ wchar_t* sang std::string
std::string WideCharToString(const wchar_t* wideStr) {
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0) {
        return "";
    }
    std::string result(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &result[0], bufferSize, NULL, NULL);
    return result;
}

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

std::string PrintAdapterInfo() {
    // Khởi tạo Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        return "WSAStartup failed with error: " + std::to_string(iResult) + "\n";
    }
    // Tao bien output de luu dau ra cua thong tin
    std::ostringstream output;
    output << "\nWindows IP Configuration\n" << endl;

    // Khởi tạo cấu trúc để lưu thông tin Adapter
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_UNSPEC; // Lấy cả IPv4 và IPv6

    // Kích thước bộ đệm ban đầu
    ULONG outBufLen = 15000;
    IP_ADAPTER_ADDRESSES* adapterAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);

    // Lấy thông tin adapter
    DWORD dwRetVal = GetAdaptersAddresses(family, flags, NULL, adapterAddresses, &outBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        free(adapterAddresses);
        adapterAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
        dwRetVal = GetAdaptersAddresses(family, flags, NULL, adapterAddresses, &outBufLen);
    }

    // Kiểm tra xem lấy thông tin thành công hay không
    if (dwRetVal == NO_ERROR) {
        IP_ADAPTER_ADDRESSES* currentAdapter = adapterAddresses;
        while (currentAdapter) {
            std::string adapterName = WideCharToString(currentAdapter->FriendlyName);
            output << "\nWireless LAN adapter " << adapterName << std::endl;

            // In ra thông tin địa chỉ IP
            IP_ADAPTER_UNICAST_ADDRESS* unicast = currentAdapter->FirstUnicastAddress;
            bool hasTempIPv6 = false;
            bool hasLinkLocalIPv6 = false;
            if (unicast != NULL) {
                for (IP_ADAPTER_UNICAST_ADDRESS* addr = unicast; addr != NULL; addr = addr->Next) {
                    char ipAddress[INET6_ADDRSTRLEN] = { 0 };
                    getnameinfo(addr->Address.lpSockaddr, addr->Address.iSockaddrLength, ipAddress, sizeof(ipAddress), NULL, 0, NI_NUMERICHOST);

                    // Kiểm tra loại địa chỉ IP (IPv4 hay IPv6)
                    if (addr->Address.lpSockaddr->sa_family == AF_INET6) {
                        if (addr->DadState == IpDadStatePreferred && !hasTempIPv6) {
                            output << "   IPv6 Address. . . . . . . . . . . : " << ipAddress << std::endl;
                            hasTempIPv6 = true; // Đánh dấu rằng đã in Temporary IPv6 Address
                        } else if (addr->DadState != IpDadStatePreferred && !hasLinkLocalIPv6) {
                            output << "   Link-local IPv6 Address . . . . . : " << ipAddress << std::endl;
                            hasLinkLocalIPv6 = true; // Đánh dấu rằng đã in Link-local IPv6 Address
                        } else {
                            output << "   Temporary IPv6 Address. . . . . . : " << ipAddress << std::endl;
                        }
                    } else if (addr->Address.lpSockaddr->sa_family == AF_INET) {
                        output <<     "   IPv4 Address. . . . . . . . . . . : " << ipAddress << std::endl;
                    }
                }
            }

            // In ra trạng thái kết nối
            if (currentAdapter->OperStatus == IfOperStatusUp) {
                output << "   Status: Connected" << std::endl;
            } else {
                output << "   Status: Disconnected" << std::endl;
            }

            // In ra địa chỉ gateway
            IP_ADAPTER_GATEWAY_ADDRESS* gateway = currentAdapter->FirstGatewayAddress;
            if (gateway != NULL) {
                char gatewayAddress[INET6_ADDRSTRLEN] = { 0 };
                getnameinfo(gateway->Address.lpSockaddr, gateway->Address.iSockaddrLength, gatewayAddress, sizeof(gatewayAddress), NULL, 0, NI_NUMERICHOST);
                output << "   Default Gateway: " << gatewayAddress << std::endl;
            } else {
                output << "   Default Gateway: None" << std::endl;
            }

            // Chuyển đến adapter tiếp theo
            currentAdapter = currentAdapter->Next;
        }
    } else {
        output << "GetAdaptersAddresses failed with error: " << dwRetVal << std::endl;
    }

    // Giải phóng bộ nhớ
    if (adapterAddresses) {
        free(adapterAddresses);
    }

    // Dọn dẹp Winsock
    WSACleanup();

    return output.str();
}