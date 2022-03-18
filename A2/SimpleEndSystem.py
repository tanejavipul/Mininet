import os
import sys
import time
from socket import *
from PacketExtract import *
from threading import Thread

ROUTER_PORT = 8080
ROUTER_ADDY = "12123912"


def setup():
    host_address = sys.argv[1]
    if len(sys.argv) < 3:
        host_address = input("Enter End Point IP: ")
    sock = socket(AF_INET, SOCK_DGRAM)
    sock_tcp = socket(AF_INET, SOCK_STREAM)
    sock_tcp.bind((host_address, 0))
    sock.bind((host_address, 0))
    return sock, sock_tcp


def send_broadcast(s):
    s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
    src_address = s.getsockname()[0]
    src_port = s.getsockname()[1]
    message = "Hello I am a host"
    # TODO FIX PACKET
    simple_packet = make_packet("255.255.255.255", ROUTER_PORT, 0, src_address, src_port, message)
    s.sendto(simple_packet, ("255.255.255.255", ROUTER_PORT)) # not sure if need to .encode packet


def thread_send(s):
    dest_ip = input("Enter destination address: ")
    ttl = input("Specify the TTL for the message: ")
    message = input("Enter a message: ")

    src_address = s.getsockname()[0]
    src_port = s.getsockname()[1]
    # dest_port should probably be determined by router

    packet = make_packet(dest_ip, -1, ttl, src_address, src_port, message)

    # send message to router
    s.connect((ROUTER_ADDY, ROUTER_PORT))
    s.sendall(packet)


def thread_receive(sock_tcp):
    while True:
        conn, addr = sock_tcp.accept()  # Establish connection with client
        while True:
            data = conn.recv(1024)
            print(f'Message received {data}')
            if not data:
                break
    conn.close()
    pass


def main():
    sock, sock_tcp = setup() #UDP sock, TCP sock
    send_broadcast(sock)
    Thread(target=thread_send(), args=(sock_tcp,)).start() # Create thread for sending messages

    Thread(target=thread_receive(), args=(sock_tcp,)).start()


if __name__ == "__main__":
    main()
