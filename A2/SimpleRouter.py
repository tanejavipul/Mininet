import os
import select
import sys
import time
from socket import *
from Packets import *
from threading import Thread
from ForwardTable import *
import netifaces as ni


forward_table = {}
ROUTER_ADDRESS = ""



def broadcast_setup():
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind(('255.255.255.255', BROADCAST_PORT))
    return s


# FIXME FOR MULTI
def eth_thread(eth, address):
    print(str(eth) + " Thread Started with IP: " + address)
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind((address, PACKET_PORT))
    while(True):
        output = s.recvfrom(4096)
        print(eth + str(output))


# For receiving forward table from neighbors to update current router tables
# FIXME CHANGE FOR MULTI
def broadcast_recv_thread(s: socket):
    print("Broadcast Receiving Thread Started")
    global forward_table
    while True:
        data, add = s.recvfrom(4096)
        print("BROADCAST: " + str((add, data)))
        dict = convert_to_dict(data)
        if dict["type"] == TYPE_INITIALIZE:
            if ROUTER_ADDRESS in forward_table:
                forward_table[ROUTER_ADDRESS].append(dict.get("source_ip"))
            else:
                forward_table[ROUTER_ADDRESS] = [dict.get("source_ip")]


# For sending forward table to neighbors
# FIXME THIS IS FOR MULTI
def broadcast_send_thread(s: socket):
    print("Broadcast Sending Thread Started")
    time.sleep(3)
    # while True:
    #     adv = advertise()
    #     s.sendto(adv, ("<broadcast>", BROADCAST_PORT))
    pass


def main():
    pass





if __name__ == "__main__":
    broad = broadcast_setup()
    print("STARTING BROADCAST THREADS")
    Thread(target=broadcast_recv_thread, args=(broad, )).start()
    Thread(target=broadcast_send_thread, args=(broad, )).start()
    print("COMPLETED BROADCAST THREADS\n")


    print("STARTING ETH THREADS")
    interfaces = ni.interfaces()
    if 'lo' in interfaces: interfaces.remove('lo')
    all_thread = []
    eth0 = None
    for inter in interfaces:
        ip_dict = ni.ifaddresses(inter)[2][0]
        ip = ip_dict.get('addr', "")
        if "eth0" in inter:
            eth0 = inter
        if ip:
            # Just populate it with any IP ADDRESS
            ROUTER_ADDRESS = ip
            thr = Thread(target=eth_thread, args=(inter, ip,)).start()
            all_thread.append(thr)
    print("COMPLETED ETH THREADS\n")


    if eth0: #set address to eth0 if exist
        ROUTER_ADDRESS = '172.16.0.1' # change according to NIC's for library

    if len(all_thread) == 0:
        raise Exception("NO THREAD WERE MADE")

    print("ROUTER HAS STARTED!")








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