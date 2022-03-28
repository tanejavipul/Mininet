import os
import select
import sys
import time
from socket import *
from threading import Thread


# import keyboard
#
# while True:  # making a loop
#     try:  # used try so that if user pressed other than the given key error will not be shown
#         print("hi")
#         if keyboard._list:
#             print('You Pressed A Key!')
#             break  # finishing the loop
#     except:
#         break


# TODO NEW
# from threading import Thread
# from time import sleep
#
# a = 0
#
# def task():
#     global a
#     while(1):
#         a+=1
#         sleep(1)
#
# def pr():
#     while(True):
#         print(a)
#
#
# t1 = Thread(target=task)
# t2 = Thread(target=pr)
#
# t1.start()
# t2.start()
#
# # wait for the threads to complete
# t1.join()
# t2.join()

#TODO NEW
if __name__ == "__main__":
    s = socket(AF_INET, SOCK_STREAM)
    s.bind(("172.16.0.1", 8008))
    s.listen(5)
    while True:
        conn, addr = s.accept()
        print(conn.recv(4096))




# TODO this is for sending to monitor
MYPORT = 8080

import sys, time
from socket import *

s = socket(AF_INET, SOCK_DGRAM)


# s.bind(('10.0.1.10', 0))
# s.bind(('192.168.84.129', MYPORT)) 172.16.0.100
print(s)
print("Start sending....")

while 1:
    data = "hello my name jefff57"
    s.sendto(data.encode(), ('10.0.0.1', MYPORT))
