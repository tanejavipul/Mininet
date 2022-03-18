import os
import select
import sys
import time
from socket import *
from PacketExtract import *
from threading import Thread

ROUTER_PORT = 8080
ROUTER_ADDRESS = "172.16.0.1"


def eth_thread():
    pass


def broadcast_thread(t):
    pass


def main():


    pass


if __name__ == "__main__":
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind((ROUTER_ADDRESS, ROUTER_PORT))
    while True:
        data, add = s.recvfrom(1024)
        print(add, data)
        # conn.send("recieved ".encode())


    # main()


# s = socket(AF_INET, SOCK_DGRAM)
# # s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
#
# #192.168.84.129
# # s.bind(('10.0.1.1', MYPORT))
# s.bind(('255.255.255.255', MYPORT))
#
# # s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
# print(s)
# while 1:
#     print("####### Server is listening #######")
#     data = s.recvfrom(1500)
#     print(data)