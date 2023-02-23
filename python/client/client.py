from gui import *
import threading

ip_of_server = ("192.168.1.211", 8200)


def parser(s: socket.socket, tkinter_handle: TkInterScreen):
    while True:
        data, ip = s.recvfrom(1024)
        data = data.decode()
        if SocketData.is_in_Tkinter:
            while tkinter_handle.menu is None:
                pass
        if data.startswith("additional_option"):
            data = data[17:]
            tkinter_handle.update_options(data)
        elif data.startswith("options"):
            data = data[7:]
            if data:
                for option in data.split('\n'):
                    tkinter_handle.update_options(option)
        elif data.startswith("hello i am ur victim"):
            SocketData.ip_of_victim = ip
        elif data == 'bye':
            s.close()
            return


def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client.sendto("attacker".encode(), ip_of_server)
    SocketData.s = client
    SocketData.ip_of_server = ip_of_server
    t = TkInterScreen()
    p = PygameScreen()
    threading.Thread(target=parser, args=(client, t), daemon=True).start()
    while not SocketData.want_to_leave:
        SocketData.is_in_Tkinter = True
        client.sendto("request_options".encode(), ip_of_server)
        t.main_loop()
        SocketData.is_in_Tkinter = False
        if SocketData.want_to_leave:
            break
        p.run()


if __name__ == '__main__':
    main()
