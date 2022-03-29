import os
import select
import sys
import time
from socket import *
from Packets import *
from threading import Thread
import netifaces as ni
import datetime as dt

HOST_ADDRESS = ""
ROUTER_ADDRESS = ""
ROUTER = (ROUTER_ADDRESS, ROUTER_PORT)
ROUTER_KEEP_ALIVE = dt.datetime.now()
KILL = 0


def setup():
    global HOST_ADDRESS
    global ROUTER_ADDRESS
    global ROUTER
    print(sys.argv)
    if len(sys.argv) < 2:
        host_address = input("Enter End Point IP: ")
    else:
        host_address = sys.argv[1]

    HOST_ADDRESS = host_address
    ROUTER_ADDRESS = ni.gateways()['default'][ni.AF_INET][0]
    ROUTER = (ROUTER_ADDRESS, ROUTER_PORT)

    # UDP BROADCAST CONNECT
    keep_alive = socket(AF_INET, SOCK_DGRAM)
    keep_alive.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    keep_alive.bind(('255.255.255.255', BROADCAST_PORT))

    # PACKET CONNECT
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

    return sock, keep_alive


def send_broadcast(s):
    global ROUTER_KEEP_ALIVE
    ROUTER_KEEP_ALIVE = dt.datetime.now()
    simple_packet = make_broadcast_packet(TYPE_INITIALIZE, HOST_ADDRESS, 0, TYPE_INITIALIZE, ROUTER_ADDRESS)
    s.sendto(simple_packet, ("255.255.255.255", BROADCAST_PORT))


# This sends a keep alive message to router to tell router this IP is still active
# FOR SENDING KEEPALIVE
def keep_alive_thread(s: socket):
    global KILL
    print("Keep Alive Thread Started")
    message = "KEEP_ALIVE"
    simple_packet = make_broadcast_packet(TYPE_KEEP_ALIVE, HOST_ADDRESS, 0, message, ROUTER_ADDRESS)
    while True:
        time.sleep(3)
        s.sendto(simple_packet, ("255.255.255.255", BROADCAST_PORT))
        w = dt.datetime.now() - ROUTER_KEEP_ALIVE
        if w.total_seconds() > ROUTER_NOT_ALIVE:
            KILL = 1
            print("ROUTER IS NOT ALIVE ANYMORE!!!!!!!!!!")
            exit()


# This keep note that the router is still active using the advertised broadcast the router sends out
# FOR RECEIVING KEEPALIVE
def router_keep_alive(s: socket):
    print("Router Keep Alive Thread Started")
    while True:
        global ROUTER_KEEP_ALIVE
        data, address = s.recvfrom(4096)
        # print("RECV: " + str((address, data)))
        data_dict = convert_to_dict(data)
        if data_dict[TYPE] == TYPE_ADVERTISE:
            ROUTER_KEEP_ALIVE = dt.datetime.now()
        if KILL == 1:
            exit()


def main():
    global ROUTER
    global KILL
    sock, keep_alive = setup()  # UDP sock, TCP sock
    Thread(target=keep_alive_thread, args=(keep_alive,)).start()
    Thread(target=router_keep_alive, args=(keep_alive,)).start()
    print("Host IP: " + str(HOST_ADDRESS))
    print("Router IP: " + str(ROUTER_ADDRESS) + "\n")

    send_broadcast(sock)

    while True:
        sockets_list = [sys.stdin, sock]

        read_sockets, write_socket, error_socket = select.select(sockets_list, [], [])

        for socks in read_sockets:
            if KILL == 1:
                print("ROUTER IS NOT ALIVE")
                print("HOST EXITING")
                exit()
            if socks == sock:
                message, add = socks.recvfrom(4096)
                decoded_message = convert_to_dict(message)
                # FIX if needed
                update_TTL(decoded_message)
                if decoded_message[PROTOCOL] == PROTOCOL_OSPF:
                    info = "   DELAY:" + str(decoded_message[DELAY_STR])
                    print("FROM:(" + decoded_message[SOURCE_IP] + ") " + decoded_message[MESSAGE] + info)
                else:
                    if decoded_message[TTL] < 0:
                        print("ERROR TTL: PACKET DROPPED")
                    else:
                        info = "   TTL:" + str(decoded_message[TTL]) + "   DELAY:" + str(decoded_message[DELAY_STR])
                        print("FROM:(" + decoded_message[SOURCE_IP] + ") " + decoded_message[MESSAGE] + info)
            else:
                try:
                    message = sys.stdin.readline()
                    message = message.split(" ")
                    packet = extract(message)
                    if packet is not None:
                        sock.sendto(packet, ROUTER)
                    sys.stdout.flush()
                except:
                    if KILL == 1:
                        print("ROUTER IS NOT ALIVE")
                        print("HOST EXITING")
                        exit()


# FIXME ERIC
def extract(message):
    if len(message) < 3:
        print("Please input in format [IP] [OSPF OR <int: TTL>] [MESSAGE]")
        return None
    else:
        try:
            if message[1].upper() == OSPF:
                ip = message[0]
                message_arg = message[2:]
                full_message = ' '.join(message_arg)
                return make_packet(ip, HOST_ADDRESS, 200, PROTOCOL_OSPF, full_message)
            # Default RIP
            else:
                ip = message[0]
                ttl = message[1]
                message_as_lst = message[2:]
                full_message = ' '.join(message_as_lst)
                return make_packet(ip, HOST_ADDRESS, int(ttl), PROTOCOL_RIP, full_message)
        except:
            print("Please input in format [IP] [OSPF OR TTL] [MESSAGE]")
            return None


if __name__ == "__main__":
    main()
