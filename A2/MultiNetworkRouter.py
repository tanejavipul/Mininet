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

DELAY = 1
ETH = []
ROUTER_ADDRESS = ""

NAT = {}
forward_table = {}
NEIGHBORS = {}


def broadcast_setup():
    s_recv = socket(AF_INET, SOCK_DGRAM)
    s_recv.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    s_recv.bind(('255.255.255.255', BROADCAST_PORT))

    s_send = socket(AF_INET, SOCK_DGRAM)
    s_send.bind((ETH[0],BROADCAST_PORT))
    s_send.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    return s_recv, s_send


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
        print("ADDRESS: " + str(add) + " ETH: " + str(eth) + " PACKET: " + str(packet_data))

        data = convert_to_dict(packet_data)

        # FIXME CHECK WITH PROF IF DELAY NEED BETWEEN INTERFACES
        if add[0] not in ETH:
            update_TTL(data)
            update_DELAY(data, DELAY)


        # Scenarios
        # SCENARIO 1: INTERNAL COMMUNICATION
        #       -->  SCENARIO 1.1  -  DEST IP IS IN CURRENT ETH
        #       -->  SCENARIO 1.2  -  DEST IP IS OTHER ETH
        dest = data[DEST_IP]
        found = False
        if dest in NAT:
            for interface in ETH:
                if dest in forward_table[interface][HOSTS]:
                    if interface == address:
                        found = True
                        print("matching eth")
                        s.sendto(convert_to_json(data), (NAT[dest][ADDRESS], NAT[dest][PORT]))
                    else:
                        found = True
                        print("not matching")
                        print(interface)
                        s.sendto(convert_to_json(data), (interface, ROUTER_PORT))
        # SCENARIO 2: DEST IP IN ANOTHER ROUTER
        else:
            for key in forward_table:
                if dest in forward_table[key][HOSTS]:
                    found = True
                    s.sendto(convert_to_json(data), (forward_table[key][SEND_TO], ROUTER_PORT))
                    break
        if not found:
            print("ERROR: PACKET DROPPED - DESTINATION NOT FOUND")


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
            forward_table[data_dict.get(ROUTER_INTERFACE)][HOSTS].append(data_dict[ADDRESS])
            NAT[data_dict[ADDRESS]] = {}
            NAT[data_dict[ADDRESS]][ADDRESS] = address[0]
            NAT[data_dict[ADDRESS]][PORT] = address[1]
            NAT[data_dict[ADDRESS]][KEEP_ALIVE] = dt.datetime.now()
            print("NAT: " + str(NAT))
            print("FORWARD: " + str(forward_table))
        if data_dict[TYPE] == TYPE_KEEP_ALIVE:
            NAT[ADDRESS][KEEP_ALIVE] = dt.datetime.now()
            # TODO
        if data_dict[TYPE] == TYPE_ADVERTISE:
            update(forward_table, data_dict)


# For sending forward table to neighbors
# FIXME THIS IS FOR MULTI
def broadcast_send_thread(s: socket):
    print("Broadcast Sending Thread Started")
    while True:
        time.sleep(2.5)
        adv = advertise()
        print("SENDING TESTING")
        print(s)

        s.sendto(adv.encode(), ("255.255.255.255", BROADCAST_PORT))

    # TODO CHECK FOR DEAD HOST HERE
    # for key in NAT:
    #     now = dt.datetime.now()
    #     temp = now - key[KEEP_ALIVE]
    #     if temp.total_seconds() > HOST_NOT_ALIVE:
    #         print("REMOVING INACTIVE HOST: " + str(key))
    #         for eth in ETH:
    #             if key in forward_table[eth][HOSTS]:
    #                 forward_table[eth][HOSTS].remove(key)
    #                 break
    #         NAT.pop(key)


def main():
    pass


if __name__ == "__main__":
    if len(sys.argv) == 2:
        try:
            if sys.argv[1].upper() == "R":
                delay = random.randint(1, 10)
            else:
                delay = int(sys.argv[1])
                print("Delay: " + str(delay))
        except:
            print("Please provide a number for delay")
            print("Delay: 1")
    # recv, send = broadcast_setup()
    print("STARTING BROADCAST THREADS")
    # Thread(target=broadcast_recv_thread, args=(recv,)).start()
    # Thread(target=broadcast_send_thread, args=(send,)).start()
    print("COMPLETED BROADCAST THREADS\n")

    print("STARTING ETH THREADS")
    interfaces = ni.interfaces()
    if 'lo' in interfaces: interfaces.remove('lo')
    all_thread = []
    for inter in interfaces:
        ip_dict = ni.ifaddresses(inter)[2][0]
        ip = ip_dict.get('addr', "")
        if ip:
            # Just populate it with any IP ADDRESS
            ROUTER_ADDRESS = ip
            thr = Thread(target=eth_thread, args=(inter, ip,))
            thr.start()
            ETH.append(ip)
            all_thread.append(thr)

    time.sleep(0.5)
    print("COMPLETED ETH THREADS\n")

    if len(ETH) == 0:
        raise Exception("NO THREAD WERE MADE")

    print("ROUTER HAS STARTED!")
    recv, send = broadcast_setup()
    Thread(target=broadcast_send_thread, args=(send,)).start()

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
