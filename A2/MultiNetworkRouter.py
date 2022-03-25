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
    s_send.bind((ETH[0], BROADCAST_PORT)) #FIXME need to have multple broadcasting threads, one for each interface
    s_send.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    return s_recv, s_send


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
        valid = False
        data, address = s.recvfrom(4096)
        print("BROADCAST INCO: " + str((address, data)))

        try:
            if address[0] in ETH: # TO ignore self broadcasts reads
                print("void")
                valid = False
            else:
                data = convert_to_dict(data)
                valid = True
        except:
            valid = False
        print(valid)
        if valid:
            if data[TYPE] == TYPE_INITIALIZE:
                forward_table[data.get(ROUTER_INTERFACE)][HOSTS].append(data[ADDRESS])
                NAT[data[ADDRESS]] = {}
                NAT[data[ADDRESS]][ADDRESS] = address[0]
                NAT[data[ADDRESS]][PORT] = address[1]
                NAT[data[ADDRESS]][KEEP_ALIVE] = dt.datetime.now()
                print("NAT: " + str(NAT))
                print("FORWARD: " + str(forward_table))
            if data[TYPE] == TYPE_KEEP_ALIVE:
                print("NAT: " + str(NAT))
                NAT[data[ADDRESS]][KEEP_ALIVE] = dt.datetime.now()
            if data[TYPE] == TYPE_ADVERTISE:
                print("UPDATING")
                # TODO update neighbors timing and fix update,if neighbors updated then send update out
                update(forward_table, data)
                #todo update keep alive with neighbors
                #todo populate neigthbors
                print("NEWWW table: "+ str(forward_table))


# For sending forward table to neighbors
# FIXME THIS IS FOR MULTI
def broadcast_send_thread(s: socket):
    print("Broadcast Sending Thread Started")
    while True:
        time.sleep(2)
        adv = advertise(forward_table, ROUTER_ADDRESS)
        adv = convert_to_json(adv)
        s.sendto(adv, ("255.255.255.255", BROADCAST_PORT))

    # TODO CHECK FOR DEAD HOST HERE
        for key in NAT:
            now = dt.datetime.now()
            temp = now - NAT[key][KEEP_ALIVE]
            if temp.total_seconds() > HOST_NOT_ALIVE:
                print("REMOVING INACTIVE HOST: " + str(key))
                for eth in ETH:
                    if key in forward_table[eth][HOSTS]:
                        forward_table[eth][HOSTS].remove(key)
                        break
                NAT.pop(key)


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

    print("STARTING ETH THREADS")
    interfaces = ni.interfaces()
    if 'lo' in interfaces: interfaces.remove('lo')
    all_thread = []
    for inter in interfaces:
        ip_dict = ni.ifaddresses(inter)[2][0]
        ip = ip_dict.get('addr', "")

        if "eth0" in inter or "eth1" in inter:
            # Just populate it with any IP ADDRESS
            ROUTER_ADDRESS = ip

        if ip:
            thr = Thread(target=eth_thread, args=(inter, ip,))
            thr.start()
            ETH.append(ip)
            all_thread.append(thr)

    if len(ETH) == 0:
        raise Exception("ERROR: NO THREAD WERE MADE")
    if ROUTER_ADDRESS == "":
        ROUTER_ADDRESS = ETH[0]

    time.sleep(0.5)
    print("COMPLETED ETH THREADS\n")

    s_recv, s_send = broadcast_setup()
    print("STARTING BROADCAST THREADS")
    Thread(target=broadcast_recv_thread, args=(s_recv,)).start()
    Thread(target=broadcast_send_thread, args=(s_send,)).start()
    print("COMPLETED BROADCAST THREADS\n")

    print("ROUTER HAS STARTED!")
    print(ETH)
