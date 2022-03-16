# from socket import *
# import sys
#
# PORT = 9090
#
#
#
#
#
# if __name__ == '__main__':
#
#     # ADDRESS = input("Enter End System Address: ")
#     sock = socket(AF_INET, SOCK_STREAM)
#     sock.bind(("", PORT))
#     print(sock)
#     sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
#     sock.listen(5)
#     while 1:
#         (clientsocket, address) = sock.accept()
#         data = sock.recv(1024).decode()
#         print("OUTPUT")
#         print(data)
#         print(address)



MYPORT = 50000

import sys
from socket import *

s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', MYPORT))

while 1:
    data, wherefrom = s.recvfrom(1500, 0)
    print(data)
    print(wherefrom)