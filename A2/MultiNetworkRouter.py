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

DELAY = 1
INTERFACES = []
ROUTER_ADDRESS = ""

HOST_LIST = {}
FORWARD_TABLE = {}
NEIGHBORS = {}
NEIGHBORS_ALIVE = {}
TOPOLOGY = Graph()


def broadcast_setup(input_address: str):
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind((input_address, 0))
    s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    return s


def eth_thread(eth, address):
    global FORWARD_TABLE
    print(str(eth) + " Thread Started with IP: " + address)
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind((address, ROUTER_PORT))

    # ADD ROUTER INTERFACE TO FORWARD TABLE
    FORWARD_TABLE[address] = {}
    FORWARD_TABLE[address][HOSTS] = []
    FORWARD_TABLE[address][HOPS] = 0
    FORWARD_TABLE[address][SEND_TO] = address

    while True:

        packet_data, add = s.recvfrom(4096)
        print("ADDRESS: " + str(add) + " ETH: " + str(eth) + " PACKET: " + str(packet_data))

        try:
            data = convert_to_dict(packet_data)
            dropped = False

            # FIXME CHECK WITH PROF IF DELAY NEED BETWEEN INTERFACES
            # TODO fix delays
            if data[DEST_IP] not in HOST_LIST:
                update_TTL(data)
                update_DELAY(data, DELAY)

            if data[PROTOCOL] == PROTOCOL_RIP and int(data[TTL]) < 0:
                print("ERROR: PACKET DROPPED - TTL ERROR")
                dropped = True
            print("BEFORE SUBNET IF")
            # TODO FIX THIS
            if data[PROTOCOL] == SUBNET_BROADCAST:
                dropped = True
                dest = data[DEST_IP]
                print(dest)
                for host in HOST_LIST:
                    if host.startswith(dest):
                        for interface in INTERFACES:
                            if host in FORWARD_TABLE[interface][HOSTS]:
                                if interface == address:
                                    print("if")
                                    s.sendto(convert_to_json(data), (HOST_LIST[host][ADDRESS], HOST_LIST[host][PORT]))
                                else:
                                    print("else")
                                    s.sendto(convert_to_json(data), (interface, ROUTER_PORT))
                            break
            # Scenarios
            # SCENARIO 1: INTERNAL COMMUNICATION
            #       -->  SCENARIO 1.1  -  DEST IP IS IN CURRENT ETH
            #       -->  SCENARIO 1.2  -  DEST IP IS OTHER ETH
            if not dropped:
                dest = data[DEST_IP]
                found = False
                if dest in HOST_LIST:
                    for interface in INTERFACES:
                        if dest in FORWARD_TABLE[interface][HOSTS]:
                            if interface == address:
                                found = True
                                s.sendto(convert_to_json(data), (HOST_LIST[dest][ADDRESS], HOST_LIST[dest][PORT]))
                            else:
                                found = True
                                s.sendto(convert_to_json(data), (interface, ROUTER_PORT))
                # SCENARIO 2: DEST IP IN ANOTHER ROUTER
                else:
                    if data[PROTOCOL] == PROTOCOL_OSPF:
                        print("INSIDE OSPFF")
                        next = TOPOLOGY.shortest_path_next(ROUTER_ADDRESS, dest)
                        print("NEXT: " + str(next))
                        if next is not None:
                            found = True
                            s.sendto(convert_to_json(data), (next, ROUTER_PORT))
                    else:
                        for key in FORWARD_TABLE:
                            if dest in FORWARD_TABLE[key][HOSTS]:
                                found = True
                                s.sendto(convert_to_json(data), (FORWARD_TABLE[key][SEND_TO], ROUTER_PORT))
                                break
                if not found:
                    print("ERROR: PACKET DROPPED - DESTINATION NOT FOUND")
        except :
            print("ERROR: POTENTIAL PACKET DROP")
            # print("EXCEPTION: " + str(e))



# For receiving FORWARD TABLE from neighbors to update current router tables
# and for updating NAT's with KEEP_ALIVE
def broadcast_recv_thread():
    print("Broadcast Receiving Thread Started")
    global FORWARD_TABLE
    global HOST_LIST
    global ROUTER_ADDRESS
    global TOPOLOGY
    global NEIGHBORS

    sock_list = []
    for eth in INTERFACES:
        sock_list.append(broadcast_setup(eth))

    s_recv = socket(AF_INET, SOCK_DGRAM)
    s_recv.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    s_recv.bind(('255.255.255.255', BROADCAST_PORT))

    while True:
        data, address = s_recv.recvfrom(4096)

        try:
            if address[0] in INTERFACES:  # TO ignore self broadcasts reads
                valid = False
            else:
                data = convert_to_dict(data)
                valid = True
        except:
            valid = False

        if valid:
            if data[TYPE] == TYPE_INITIALIZE:
                FORWARD_TABLE[data.get(ROUTER_INTERFACE)][HOSTS].append(data[ADDRESS])
                HOST_LIST[data[ADDRESS]] = {}
                HOST_LIST[data[ADDRESS]][ADDRESS] = address[0]
                HOST_LIST[data[ADDRESS]][PORT] = address[1]
                HOST_LIST[data[ADDRESS]][KEEP_ALIVE] = dt.datetime.now()
                NEIGHBORS[data[ADDRESS]] = 0


            if data[TYPE] == TYPE_KEEP_ALIVE:
                HOST_LIST[data[ADDRESS]][KEEP_ALIVE] = dt.datetime.now()

            if data[TYPE] == TYPE_ADVERTISE:
                update(FORWARD_TABLE, data)
                NEIGHBORS[data[FROM]] = DELAY
                NEIGHBORS_ALIVE[data[FROM]] = dt.datetime.now()

            if data[TYPE] == MONITOR_REQUEST:
                monitor = monitor_response(ROUTER_ADDRESS, NEIGHBORS)
                monitor_packet = convert_to_json(monitor)
                for sock in sock_list:
                    sock.sendto(monitor_packet, ("255.255.255.255", BROADCAST_PORT))
            if data[TYPE] == MONITOR_TOPO:
                TOPOLOGY.vertices = data[STR_NEIGHBORS]


# For sending forward table to neighbors
def broadcast_send_thread():
    global FORWARD_TABLE
    global HOST_LIST
    global DELAY
    global NEIGHBORS
    print("Broadcast Sending Thread Started")
    sock_list = []
    for eth in INTERFACES:
        sock_list.append(broadcast_setup(eth))

    while True:
        time.sleep(2)
        adv = advertise(FORWARD_TABLE, ROUTER_ADDRESS, NEIGHBORS)
        adv = convert_to_json(adv)
        for sock in sock_list:
            sock.sendto(adv, ("255.255.255.255", BROADCAST_PORT))

        temp = HOST_LIST.copy()
        for key in temp:
            now = dt.datetime.now()
            temp = now - HOST_LIST[key][KEEP_ALIVE]
            if temp.total_seconds() > HOST_NOT_ALIVE:
                print("REMOVING INACTIVE HOST: " + str(key))
                for eth in INTERFACES:
                    if key in FORWARD_TABLE[eth][HOSTS]:
                        FORWARD_TABLE[eth][HOSTS].remove(key)
                        break
                HOST_LIST.pop(key)
                NEIGHBORS.pop(key) # To Remove Host from neighbors

        temp_neighbors = NEIGHBORS_ALIVE.copy()
        for key in temp_neighbors:
            now = dt.datetime.now()
            temp = now - NEIGHBORS_ALIVE[key]
            if temp.total_seconds() > ROUTER_NOT_ALIVE:
                print("REMOVING INACTIVE ROUTER: " + str(key))
                forward_temp = FORWARD_TABLE.copy()
                for forward_key in forward_temp:
                    if FORWARD_TABLE[forward_key][SEND_TO] == key:
                        FORWARD_TABLE.pop(forward_key)
                NEIGHBORS.pop(key)
                NEIGHBORS_ALIVE.pop(key)
        # neightbors and forward table
        # forward table -> pop if send_to is the saem and if key is the same


# PRINT TABLE
# PRINT NAT
# PRINT NEIGHBORS
# SET DELAY 10   #when you update delay, you need to update neighbors
def get_command_input():
    global FORWARD_TABLE
    global HOST_LIST
    global DELAY
    global NEIGHBORS
    while True:
        message = sys.stdin.readline()
        message = message.strip().upper()
        if message == PRINT_NAT:
            print("NAT: " + str(HOST_LIST))
        elif message == PRINT_FORWARD_TABLE:
            print("FORWARD: " + str(FORWARD_TABLE))
        elif message == PRINT_NEIGHBORS:
            print("NEIGHBORS: " + str(NEIGHBORS))
        elif message == PRINT_DELAY:
            print("DELAY: " + str(DELAY))
        elif message == PRINT_TOPOLOGY:
            print("TOPOLOGY: " + str(TOPOLOGY.vertices))
        else:
            if SET_DELAY in message:
                try:
                    new_delay = int(message.split()[2])
                    DELAY = new_delay
                    for neigh in NEIGHBORS:
                        NEIGHBORS[neigh] = DELAY
                except:
                    print("INVALID COMMAND")
                    print(
                        "VALID COMMAND LIST: 'PRINT FORWARD TABLE', 'PRINT NEIGHBORS', 'PRINT NAT', 'PRINT TOPOLOGY', 'SET DELAY <int>'")

            else:
                print("INVALID COMMAND")
                print("VALID COMMAND LIST: 'PRINT FORWARD TABLE', 'PRINT NEIGHBORS', 'PRINT NAT', 'PRINT TOPOLOGY', 'SET DELAY <int>'")


if __name__ == "__main__":
    if len(sys.argv) == 2:
        try:
            if sys.argv[1].upper() == "R":
                delay = random.randint(1, 10)
            else:
                delay = int(sys.argv[1])
                print("Delay: " + str(delay))
            DELAY = delay
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
            INTERFACES.append(ip)
            all_thread.append(thr)

    if len(INTERFACES) == 0:
        raise Exception("ERROR: NO THREAD WERE MADE")
    if ROUTER_ADDRESS == "":
        ROUTER_ADDRESS = INTERFACES[0]

    time.sleep(0.5)
    print("COMPLETED ETH THREADS\n")

    print("STARTING BROADCAST THREADS")
    Thread(target=broadcast_recv_thread).start()
    Thread(target=broadcast_send_thread).start()
    print("COMPLETED BROADCAST THREADS\n")

    Thread(target=get_command_input).start()
    print("ROUTER HAS STARTED!")
    print("INTERFACES: " + str(INTERFACES))
