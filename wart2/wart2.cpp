using namespace std;
#include <windows.h>
#include <string>
#include <iostream>
#include "wtypes.h"

INPUT keyboard_input;
INPUT mouse_input;

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

int key_name_to_code(string key_name) {
    if ((key_name <= "9" && key_name >= "0") || (key_name <= "z" && key_name >= "a") || (key_name <= "Z" && key_name >= "A")) {
        return int(toupper(key_name[0]));
    }
    for (int i = 0; i < 4; i++) {
        if (key_name == names[i]) {
            return codes[i];
        }
    }
    return 0;
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

int main()
{
    //--------------------- some basic setup ----------------------------
    GetDesktopResolution();
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

    

}