import os
import random
import select
import sys
import time
from socket import *
from Packets import *
from threading import Thread
from ForwardTable import *
import netifaces as ni
import datetime as dt
from dijkstras import *


G = Graph()

def broadcast_setup(ip):
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind((ip, BROADCAST_PORT))
    s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    return s


# For receiving FORWARD TABLE from neighbors to update current router tables
# and for updating NAT's with KEEP_ALIVE
def broadcast_recv_thread():
    print("Broadcast Receiving Thread Started")
    global forward_table
    global NAT

    s_recv = socket(AF_INET, SOCK_DGRAM)
    s_recv.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    s_recv.bind(('255.255.255.255', BROADCAST_PORT))

    while True:
        data, address = s_recv.recvfrom(4096)
        print("BROADCAST INCO: " + str((address, data)))

        try:
            data = convert_to_dict(data)
            if data[TYPE] == TYPE_ADVERTISE:
                ip = data[FROM]
                neighbors = data[STR_NEIGHBORS]
                G.add_vertex(ip, neighbors)
        except:
            print("ERROR WITH PACKET")


if __name__ == "__main__":
    G = Graph()

    thread = Thread(target=broadcast_recv_thread)
    thread.start()
    thread.join()

    while True:
        s = socket(AF_INET, SOCK_DGRAM)
        s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
        s.bind(('', BROADCAST_PORT)) # FIXME

        try:
            data, address = s.recvfrom(4096)

            data = convert_to_dict(data)
            from_router = data[FROM]
            to_router = data[TO]

        except:
            pass






