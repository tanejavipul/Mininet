import os
import select
import sys
import time
from socket import *
from PacketExtract import *
from threading import Thread

ROUTER_PORT = 8080
ROUTER_ADDRESS = "172.16.0.1"


def setup():
    if len(sys.argv) < 3:
        host_address = input("Enter End Point IP: ")
    else:
        host_address = sys.argv[1]
    #UDP BROADCAST CONNECT
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.bind((host_address, 0))
    sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

    #TCP CONNECT
    sock_tcp = socket(AF_INET, SOCK_STREAM)
    sock_tcp.bind((host_address, 0))
    return sock, sock_tcp


def send_broadcast(s):
    src_address = s.getsockname()[0]
    src_port = s.getsockname()[1]
    message = "Hello I am a host"
    simple_packet = make_broadcast_packet(src_address, src_port, message)
    s.sendto(simple_packet, ("255.255.255.255", ROUTER_PORT))


# def thread_send(s):
#     dest_ip = input("Enter destination address: ")
#     ttl = input("Specify the TTL for the message: ")
#     message = input("Enter a message: ")
#
#     src_address = s.getsockname()[0]
#     src_port = s.getsockname()[1]
#     # dest_port should probably be determined by router
#
#     packet = make_packet(dest_ip, -1, ttl, src_address, src_port, message)
#
#     # send message to router
#     s.connect((ROUTER_ADDY, ROUTER_PORT))
#     s.sendall(packet)
#
#
# def thread_receive(sock_tcp):
#     while True:
#         conn, addr = sock_tcp.accept()  # Establish connection with client
#         while True:
#             data = conn.recv(1024)
#             print(f'Message received {data}')
#             if not data:
#                 break
#     conn.close()
#     pass
#     Thread(target=thread_send(), args=(sock_tcp,)).start() # Create thread for sending messages
#
#     Thread(target=thread_receive(), args=(sock_tcp,)).start()



def main():
    sock, sock_tcp = setup() #UDP sock, TCP sock
    send_broadcast(sock)

    sock_tcp.connect((ROUTER_ADDRESS, ROUTER_PORT))

    while True:

        # maintains a list of possible input streams
        sockets_list = [sys.stdin, sock_tcp]

        """ There are two possible input situations. Either the
        user wants to give manual input to send to other people,
        or the server is sending a message to be printed on the
        screen. Select returns from sockets_list, the stream that
        is reader for input. So for example, if the server wants
        to send a message, then the if condition will hold true
        below.If the user wants to send a message, the else
        condition will evaluate as true"""
        read_sockets, write_socket, error_socket = select.select(sockets_list, [], [])

        for socks in read_sockets:
            if socks == sock_tcp:
                message = socks.recv(2048)
                print(message)
            else:
                message = sys.stdin.readline()
                sock_tcp.send(message.encode())
                sys.stdout.write("<You>")
                sys.stdout.write(message)
                sys.stdout.flush()



if __name__ == "__main__":
    main()
