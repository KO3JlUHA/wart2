import socket
import time

import keyboard
import threading
import pyautogui
import win32api
from pynput import mouse


class F:
    flag = False
    s = None
    ip = ''


are_down = []
key_names = ["space", "alt", "left windows", "ctrl", "shift", "caps lock", "tab", "esc", "f1", "f2", "f3", "f4", "f5",
             "f6", "f7", "f8", "f9", "f10", "f11", "f12", "print screen", "scroll lock", "pause", "insert", "home",
             "page up", "delete", "end", "page down", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "-", "+", "_",
             "=", "enter", "up", "down", "left", "right", "[", "]", "{", "}", "|", "\\", ",", ".", "<", ">", "/", "?",
             ";", ":", "'", '"', '`', '~', 'backspace', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c',
             'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
             'y', 'z']

key_values = [0x20, 0x12, 0x5b, 0xa2, 0xa0, 0x14, 0x09, 0x1b, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
              0x79, 0x7a, 0x7b, 0x2c, 0x91, 0x13, 0x2d, 0x24, 0x21, 0x2e, 0x23, 0x22, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
              0xbd, 0xbb, 0xbd, 0xbb, 0x0d, 0x26, 0x28, 0x25, 0x27, 0xdb, 0xdd, 0xdb, 0xbb, 0xdc, 0xdc, 0xbc, 0xbe,
              0xbc, 0xbe, 0xbf, 0xbf, 0xba, 0xba, 0xde, 0xde, 0xc0, 0xc0, 0x08, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
              0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d,
              0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a]


def log_keyboard():
    while F.flag:
        relevant_string = str(keyboard.read_event())[14:-1]
        index_of_last_space = len(relevant_string) - relevant_string[::-1].index(' ') - 1
        key = relevant_string[:index_of_last_space].lower()
        action = relevant_string[index_of_last_space + 1:]
        if key in key_names:
            key = key_values[key_names.index(key)]
            if action == 'down' and key not in are_down:
                are_down.append(key)
                F.s.sendto(f'kd{key}'.encode(), F.ip)
            else:
                F.s.sendto(f'ku{key}'.encode(), F.ip)
                are_down.remove(key)
                if 0x10 <= key <= 0x12:
                    F.s.sendto(f'kd{key}'.encode(), F.ip)
                    F.s.sendto(f'ku{key}'.encode(), F.ip)


def on_scroll(x, y, dx, dy):
    if dy > 0:
        F.s.sendto("msu".encode(), F.ip)
    else:
        F.s.sendto("msd".encode(), F.ip)
    time.sleep(0.01)


def x_func():
    with mouse.Listener(on_scroll=on_scroll) as listener:
        listener.join()


def log_mouse():
    threading.Thread(target=x_func).start()
    w, h = pyautogui.size()
    x, y = pyautogui.position()
    state_left = win32api.GetKeyState(0x01)  # Left button down = 0 or 1. Button up = -127 or -128
    state_right = win32api.GetKeyState(0x02)  # Right button down = 0 or 1. Button up = -127 or -128
    while F.flag:
        a, b = pyautogui.position()
        a = a * 100 // w
        b = b * 100 // h
        if (a, b) != (x, y):
            x, y = a, b
            t = "mM" + f'{x}'.zfill(3) + f'{y}'.zfill(3)
            F.s.sendto(t.encode(), F.ip)
        a = win32api.GetKeyState(0x01)
        b = win32api.GetKeyState(0x02)
        if a != state_left:  # Button state changed
            state_left = a
            if a < 0:
                F.s.sendto('mld'.encode(), F.ip)
            else:
                F.s.sendto('mlu'.encode(), F.ip)
        if b != state_right:  # Button state changed
            state_right = b
            if b < 0:
                F.s.sendto('mrd'.encode(), F.ip)
            else:
                F.s.sendto('mru'.encode(), F.ip)
        time.sleep(0.01)
