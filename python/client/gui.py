import socket
import threading
from tkinter import *
from functions import *


class D:
    s = None
    ip_of_server = ()


def send_choice():
    if choice.get() != "whom to troll: ":
        D.s.sendto(f"choice{int(choice.get()[0]) - 1}".encode(), D.ip_of_server)
    else:
        print("choose first")
    F.flag = True


def init():
    root.mainloop()


root = Tk()
root.title("Ewwww GUI")
root.configure(bg="#1b1b38")
root.geometry("400x400")
root.iconbitmap('icon.ico')
choice = StringVar()
choice.set("whom to troll: ")
options = ['whom to troll: ']
menu = OptionMenu(root, choice, *options)
menu.config(bg="#544580", activebackground="#544580", borderwidth=2)
menu["highlightthickness"] = 0
menu.pack()
start_trolling = Button(root, text="start", command=send_choice, height=3, width=12, bg="#544580",
                        activebackground="#7461ad")
start_trolling.place(x=150, y=100)


def destroy():
    root.destroy()


def update_options(to_add: str):
    if options.__contains__('whom to troll: '):
        options.remove('whom to troll: ')
    options.append(to_add)
    menu.destroy()
    a = OptionMenu(root, choice, *options)
    a.config(bg="#544580", activebackground="#544580", borderwidth=2)
    a["highlightthickness"] = 0
    a.pack()
    return a

# ------------------------------------
