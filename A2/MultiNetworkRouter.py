import os
import select
import sys
import time
from socket import *
from Packets import *
from threading import Thread
from ForwardTable import *
import netifaces as ni
import datetime as dt

DELAY = 1
ETH = []
ROUTER_ADDRESS = ""

NAT = {}
forward_table = {}
NEIGHBORS = {}

def broadcast_setup():
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind(('255.255.255.255', BROADCAST_PORT))
    return s


# FIXME FOR MULTI
# TODO HERE FOR SIMPLE
# TODO NEXT TIME UPDATE TTL and send as json ENCODE
# PORT is always 8008
# deconstruct packet and send ?
def eth_thread(eth, address):
    global forward_table
    print(str(eth) + " Thread Started with IP: " + address)
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind((address, ROUTER_PORT))

    # ADD ROUTER INTERFACE TO FORWARD TABLE
    forward_table[address] = {}
    forward_table[address][HOSTS] = []
    forward_table[address][HOPS] = 0
    forward_table[address][SEND_TO] = address

    while True:
        packet_data, add = s.recvfrom(4096)
        print("from add: " + str(add))
        print(eth + str(packet_data))
        print(packet_data)

        # TODO FIX with new packet formula
        # # Decode packet?
        # data = convert_to_dict(packet_data)
        #
        # # RECIEVED packet info
        # dest_ip = data.get('dest_ip')
        # dest_port = int(data.get('dest_port'))
        # ttl = int(data.get('ttl'))
        # source_ip = data.get('source_ip')
        # source_port = int(data.get('source_port'))
        # message = data.get('message')
        #
        # # validate dest_ip is in router forwarding table
        # is_dest_valid = False
        # for lst in forward_table.values():  # loop through each list of forward_table
        #     if dest_ip in lst:
        #         is_dest_valid = True
        #
        # if is_dest_valid:  # send packet to dest
        #     destination_packet = make_packet(dest_ip, dest_port, ttl, source_ip, source_port, message)
        #     s.sendto(destination_packet, (dest_ip, dest_port))
        # else:  # send packet back to sender
        #     error_message = 'FAILED_TO_SEND: ' + message
        #     resend_packet = make_packet(source_ip, source_port, ttl, ROUTER_ADDRESS, 8008, error_message)
        #     s.sendto(resend_packet, (source_ip, source_port))


# For receiving FORWARD TABLE from neighbors to update current router tables
# and for updating NAT's with KEEP_ALIVE
def broadcast_recv_thread(s: socket):
    print("Broadcast Receiving Thread Started")
    global forward_table
    global NAT
    while True:
        data, address = s.recvfrom(4096)
        print("BROADCAST: " + str((address, data)))
        data_dict = convert_to_dict(data)
        if data_dict[TYPE] == TYPE_INITIALIZE:
            forward_table[data_dict.get(ROUTER_INTERFACE)][HOSTS].append(data_dict.get(ADDRESS))
            NAT[data_dict.get(ADDRESS)] = {}
            NAT[data_dict.get(ADDRESS)][ADDRESS] = address[0]
            NAT[data_dict.get(ADDRESS)][PORT] = address[1]
            NAT[data_dict.get(ADDRESS)][KEEP_ALIVE] = dt.datetime.now()
            print("NAT: " + str(NAT))
            print("FORWARD: " + str(forward_table))
        if data_dict[TYPE] == TYPE_KEEP_ALIVE:
            NAT[ADDRESS][KEEP_ALIVE] = dt.datetime.now()







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
    Thread(target=broadcast_recv_thread, args=(broad,)).start()
    Thread(target=broadcast_send_thread, args=(broad,)).start()
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
            ETH.append(ip)
            all_thread.append(thr)
    print("COMPLETED ETH THREADS\n")

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
