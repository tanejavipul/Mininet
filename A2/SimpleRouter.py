import os
import select
import sys
import time
from socket import *
from PacketExtract import *
from threading import Thread
from ForwardTable import *


forward_table = {}



def broadcast_setup():
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind(('255.255.255.255', BROADCAST_PORT))
    return s



def eth_thread(eth, address):
    pass


# For receiving forward table from neighbors to update current router tables
def broadcast_recv_thread(s: socket):
    while True:
        data, add = s.recvfrom(4096)
        print(add, data)


# For sending forward table to neighbors
def broadcast_send_thread(s: socket):
    while True:
        adv = advertise()
        s.sendto(adv, ("255.255.255.255", BROADCAST_PORT))
    pass


def main():
    broad = broadcast_setup()


    pass




if __name__ == "__main__":
    main()








    # s = socket(AF_INET, SOCK_DGRAM)
    # s.bind((ROUTER_ADDRESS, ROUTER_PORT))
    # while True:
    #     data, add = s.recvfrom(1024)
    #     print(add, data)
    #     # conn.send("recieved ".encode())


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