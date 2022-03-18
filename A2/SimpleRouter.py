MYPORT = 8080


def eth_thread():
    pass


def broadcast_thread(t):
    pass


def main():
    pass


if __name__ == "__main__":
    main()


# s = socket(AF_INET, SOCK_DGRAM)
# # s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
#
# #192.168.84.129
# # s.bind(('10.0.1.1', MYPORT))
# s.bind(('255.255.255.255', MYPORT))
#
# # s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
# print(s)
# while 1:
#     print("####### Server is listening #######")
#     data = s.recvfrom(1500)
#     print(data)