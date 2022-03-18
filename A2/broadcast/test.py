# Testing if router eth2 can connect to Host on eth3
# ANSWER: IT CAN!


MYPORT = 8080

import sys, time
from socket import *

s = socket(AF_INET, SOCK_DGRAM)


# s.bind(('10.0.1.10', 0))
s.bind(('172.16.0.1', 0)) # r0-eth2  router - eth2
# s.bind(('192.168.84.129', MYPORT)) 172.16.0.100
print(s)
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
print("Start sending....")

while 1:
    data = "hello my name jefff57"
    s.sendto(data.encode(), ('10.0.0.100', MYPORT)) #host in eth3
