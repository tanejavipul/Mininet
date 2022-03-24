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

    # PACKET CONNECT
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

    return sock, keep_alive


def send_broadcast(s):
    global ROUTER_KEEP_ALIVE
    ROUTER_KEEP_ALIVE = dt.datetime.now()

    message = "Hello I am a host"
    simple_packet = make_broadcast_packet(TYPE_INITIALIZE, HOST_ADDRESS, 0, message, ROUTER_ADDRESS)
    s.sendto(simple_packet, ("255.255.255.255", BROADCAST_PORT))


# This sends a keep alive message to router to tell router this IP is still active
# FOR SENDING KEEPALIVE
def keep_alive_thread(s: socket):
    print("Keep Alive Thread Started")
    message = "KEEP_ALIVE"
    simple_packet = make_broadcast_packet(TYPE_KEEP_ALIVE, HOST_ADDRESS, 0, message, ROUTER_ADDRESS)
    while True:
        time.sleep(3)
        s.sendto(simple_packet, ("255.255.255.255", BROADCAST_PORT))
        w = dt.datetime.now() - ROUTER_KEEP_ALIVE
        if w.total_seconds() > ROUTER_NOT_ALIVE:
            print("ROUTER IS NOT ALIVE ANYMORE!!!!!!!!!!")
            exit()


# This keep note that the router is still active using the advertised broadcast the router sends out
# FOR RECEIVING KEEPALIVE
def router_keep_alive(s: socket):
    print("Router Keep Alive Thread Started")
    while True:
        global ROUTER_KEEP_ALIVE
        data, address = s.recvfrom(4096)
        print("RECV: " + str((address, data)))
        data_dict = convert_to_dict(data)
        if data_dict[TYPE] == TYPE_ADVERTISE:
            ROUTER_KEEP_ALIVE = dt.datetime.now()


def main():
    global ROUTER
    global KILL
    sock, keep_alive = setup()  # UDP sock, TCP sock
    # Thread(target=keep_alive_thread, args=(keep_alive,)).start() #TODO UNCOMMENT WHEN KEEP-ALIVE WORKING
    print("Host IP: " + str(HOST_ADDRESS))
    print("Router IP: " + str(ROUTER_ADDRESS))

    send_broadcast(sock)

    while True:
        sockets_list = [sys.stdin, sock]

        read_sockets, write_socket, error_socket = select.select(sockets_list, [], [])
        if KILL == 1:
            print("ROUTER IS NOT ALIVE")
            print("HOST EXITING")
            exit()

        for socks in read_sockets:
            if socks == sock:
                message, add = socks.recvfrom(4096)
                decoded_message = convert_to_dict(message)
                print("FROM:(" + decoded_message[SOURCE_IP] + ") " + decoded_message[MESSAGE])
                # print(message)
            else:
                message = sys.stdin.readline()

                message = message.split(" ")
                packet = extract(message)
                if packet is not None:
                    sock.sendto(packet, ROUTER)
                sys.stdout.flush()


def extract(message):
    if len(message) < 3:
        print("Please input in format [IP] [TTL] [MESSAGE]")
        return None
    else:
        if message[1].upper() == OSPF:
            pass
        else:
            ip = message[0]
            # print('ip argument : ' + ip)
            ttl = message[1]
            # print('ttl argument: ' + ttl)
            message_as_lst = message[2:]
            # print('message_as_lst: ' + str(message_as_lst))
            full_message = ' '.join(message_as_lst)
            # print('full_message: |' + full_message + "|")
            return make_packet(ip, HOST_ADDRESS, int(ttl), PROTOCOL_RIP, full_message)


if __name__ == "__main__":
    main()
