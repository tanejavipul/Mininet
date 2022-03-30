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
UPDATE = False

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
    global UPDATE

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
                # print("OUTPUT: ", (data, address), router, neighbors)
                if router in G.vertices:
                    if not G.vertices[router] == neighbors:
                        G.add_vertex(router, neighbors)
                        UPDATE = True
                for key in neighbors:
                    if key not in G.vertices:
                        UPDATE = True
                        G.add_vertex(key, {})

        except:
            print("ERROR WITH PACKET")


def broadcast_send_thread():
    global G
    global UPDATE

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


        topo = monitor_topo(G.vertices)
        topo = convert_to_json(topo)
        if UPDATE:
            for sock in sock_list:
                sock.sendto(topo, ("255.255.255.255", BROADCAST_PORT))
            print(G.vertices)
            UPDATE = False


if __name__ == "__main__":
    G = Graph()
    interfaces = ni.interfaces()
    if 'lo' in interfaces: interfaces.remove('lo')
    for inter in interfaces:
        ip_dict = ni.ifaddresses(inter)[2][0]
        ip = ip_dict.get('addr', "")
        if ip:
            INTERFACES.append(ip)
    threads = []
    threads.append(Thread(target=broadcast_send_thread))
    threads.append(Thread(target=broadcast_recv_thread))
    for thr in threads:
        thr.start()
    # for thr in threads:
    #     thr.join()