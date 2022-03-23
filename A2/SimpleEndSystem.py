import os
import select
import sys
import time
from socket import *
from Packets import *
from threading import Thread
import netifaces as ni

HOST_ADDRESS = ""
ROUTER_ADDRESS = ""
ROUTER = (ROUTER_ADDRESS, ROUTER_PORT)


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
    message = "Hello I am a host"
    simple_packet = make_broadcast_packet(TYPE_INITIALIZE, HOST_ADDRESS, 0, message, ROUTER_ADDRESS)
    s.sendto(simple_packet, ("255.255.255.255", BROADCAST_PORT))

def keep_alive_thread(s: socket):
    print("Keep Alive Thread Started")
    while True:
        time.sleep(3)
        send_keep_alive(s)

def send_keep_alive(s):
    message = "KEEP_ALIVE"
    simple_packet = make_broadcast_packet(TYPE_KEEP_ALIVE, HOST_ADDRESS, 0, message, ROUTER_ADDRESS)
    s.sendto(simple_packet, ("255.255.255.255", BROADCAST_PORT))

def main():
    global ROUTER
    sock, keep_alive = setup()  # UDP sock, TCP sock
    Thread(target=keep_alive_thread, args=(keep_alive,)).start()
    print("Host IP: " + str(HOST_ADDRESS))
    print("Router IP: " + str(ROUTER_ADDRESS))

    send_broadcast(sock)

    while True:
        sockets_list = [sys.stdin, sock]

        read_sockets, write_socket, error_socket = select.select(sockets_list, [], [])

        for socks in read_sockets:
            if socks == sock:
                message = socks.recvfrom(4096)
                print(message)
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
        if message[2] == "OSPF":
            pass
        ip = message[0]
        print('ip argument : ' + ip)
        ttl = message[1]
        print('ttl argument: ' + ttl)
        message_as_lst = message[2:]
        print('message_as_lst: ' + str(message_as_lst))
        full_message = ' '.join(message_as_lst)
        print('full_message: |' + full_message + "|")
        return make_packet(ip, ROUTER_PORT, int(ttl), HOST_ADDRESS, ROUTER_PORT, full_message)


if __name__ == "__main__":
    main()
