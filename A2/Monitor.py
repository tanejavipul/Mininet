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
INTERFACES = []
ROUTER_KEEP_ALIVE = {}


def broadcast_setup(ip):
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind((ip, BROADCAST_PORT))
    s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    return s


# For receiving FORWARD TABLE from neighbors to update current router tables
# and for updating NAT's with KEEP_ALIVE
def broadcast_recv_thread():
    print("Broadcast Receiving Thread Started")
    global G

    s_recv = socket(AF_INET, SOCK_DGRAM)
    s_recv.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    s_recv.bind(('255.255.255.255', BROADCAST_PORT))

    while True:
        data, address = s_recv.recvfrom(4096)

        try:
            data = convert_to_dict(data)
            if data[TYPE] == MONITOR_RESPONSE:
                router = data[ROUTER_INTERFACE]
                neighbors = data[STR_NEIGHBORS]
                print("OUTPUT: ", (data, address), router, neighbors)
                G.add_vertex(router, neighbors)
                for key in neighbors:
                    if key not in G.vertices:
                        G.add_vertex(key, {})

        except:
            print("ERROR WITH PACKET")


def broadcast_send_thread():
    global G

    print("Broadcast Sending Thread Started")
    sock_list = []
    for eth in INTERFACES:
        sock_list.append(broadcast_setup(eth))

    req = monitor_request()
    req = convert_to_json(req)
    while True:
        for sock in sock_list:
            sock.sendto(req, ("255.255.255.255", BROADCAST_PORT))
        time.sleep(4)
        print(G.vertices)

        topo = monitor_topo(G.vertices)
        topo = convert_to_json(topo)
        for sock in sock_list:
            sock.sendto(topo, ("255.255.255.255", BROADCAST_PORT))
        G = Graph()


if __name__ == "__main__":
    G = Graph()
    interfaces = ni.interfaces()
    if 'lo' in interfaces: interfaces.remove('lo')
    for inter in interfaces:
        ip_dict = ni.ifaddresses(inter)[2][0]
        ip = ip_dict.get('addr', "")
        if ip:
            INTERFACES.append(ip)

    send = Thread(target=broadcast_send_thread).start()
    recv = Thread(target=broadcast_recv_thread).start()

    # thread.start()
    # thread.join()
    #
    # while True:
    #     s = socket(AF_INET, SOCK_DGRAM)
    #     s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    #     s.bind(('', BROADCAST_PORT)) # FIXME
    #
    #     try:
    #         data, address = s.recvfrom(4096)
    #
    #         data = convert_to_dict(data)
    #         from_router = data[FROM]
    #         to_router = data[TO]
    #
    #     except:
    #         pass
