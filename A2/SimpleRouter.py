MYPORT = 50000

import sys
from socket import *

s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', MYPORT))

while 1:
    data, wherefrom = s.recvfrom(1500, 0)
    print(data)
    print(wherefrom)