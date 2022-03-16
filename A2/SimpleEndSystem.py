# import os
# import sys
# import time
# from socket import *
#
# PORT = 8091
#
#
#
# if __name__ == '__main__':
#     s = socket(AF_INET, SOCK_DGRAM)
#     s.bind(('', PORT))
#     s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
#     print(s)
#     data = "EJEHERR"
#     s.sendto(data.encode(), ('<broadcast>', 9090))
#     #
#     #
#     # # ADDRESS = input("Enter End System Address: ")
#     # sock = socket(AF_INET, SOCK_STREAM)
#     # sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
#     #
#     # sock.bind(("", PORT))
#     # sock.connect(("", PORT))
#     # print(sock.getsockname())
#     # # sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
#     # print(sock.sendto("hi".encode(), ('0.0.0.0', 9090)))
#     #
#     # # connection = socket(AF_INET, SOCK_STREAM)
#     # #
#     # # connection.connect((gethostname(), 8090))
#     # #
#     # # while True:
#     # #     message = connection.recv(1024)

MYPORT = 50000

import sys, time
from socket import *

s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', 0))
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

while 1:
    data = "vvnter End System Address"
    s.sendto(data.encode(), ('', MYPORT))
    time.sleep(2)