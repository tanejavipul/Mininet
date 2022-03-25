# retrieve the forwarding table for this router in a format that can be used for routing algorithms
import json
from Packets import *


def get_forwarding_table():
    pass


# set the forwarding table for this router given the data in some appropriate format
def set_forwarding_table():
    pass


# this will be useful in debugging, and also will allow us to easily see what your code is doing as the system updates
def print_forwarding_table(table):
    print(table)


# broadcast a message to neighbouring routers with the destinations that you can reach and the distance within which
# you can reach them
def advertise(forward_table, interface):
    return {TYPE: TYPE_ADVERTISE, FROM: interface, TABLE: forward_table}


# take in an advertisement from a neighbouring router and update your routing table accordingly
def update(cur: dict, update: dict):
    recv_table = update[TABLE]
    for key in recv_table:
        if key in cur:
            print(key)
            # if hop count lower than update or send_to field same then copy hosts and update hops and send_to field
            if int(cur[key][HOPS]) > int(recv_table[key][HOPS])+1 or cur[key][SEND_TO] == update[FROM]: # or int(recv_table[key][HOPS]) == 0: # same router but differnt eth
                cur[key][HOPS] = int(recv_table[key][HOPS]) + 1
                cur[key][SEND_TO] = update[FROM]
                cur[key][HOSTS] = recv_table[key][HOSTS]
        else:
            cur[key] = {}
            cur[key][HOPS] = int(recv_table[key][HOPS]) + 1
            cur[key][SEND_TO] = update[FROM]
            cur[key][HOSTS] = recv_table[key][HOSTS]
    lst = []
    for key in cur:
        if cur[key][SEND_TO] == update[FROM] and key not in recv_table:
            lst.append(key)

    for key in lst:
        cur.pop(key)


# current = {'1.1.1.1': {'HOSTS': ['323.232.3.2'], 'HOPS': 0, 'SEND_TO': '1.1.1.1'}}
# neigh = {'7.7.7.7': {'HOSTS': ['9.9.9.9.9'], 'HOPS': 10, 'SEND_TO': '7.7.7.7'}}
# pack = {'TYPE': 'ADVERTISE', 'FROM': '8.8.8.8', 'TABLE': {'7.7.7.7': {'HOSTS': ['9.9.9.9.9'], 'HOPS': 10, 'SEND_TO': '7.7.7.7'}}}
# update(current,pack)
#
# pack = {'TYPE': 'ADVERTISE', 'FROM': '8.8.8.8', 'TABLE': {'0.0.0.0': {'HOSTS': ['new'], 'HOPS': 45, 'SEND_TO': '8.8.8.8'}}}