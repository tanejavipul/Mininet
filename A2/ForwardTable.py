# retrieve the forwarding table for this router in a format that can be used for routing algorithms
import json


def get_forwarding_table():
    pass


# set the forwarding table for this router given the data in some appropriate format
def set_forwarding_table():
    pass


# this will be useful in debugging, and also will allow us to easily see what your code is doing as the system updates
def print_forwarding_table():
    print(get_forwarding_table())


# broadcast a message to neighbouring routers with the destinations that you can reach and the distance within which
# you can reach them
def advertise():
    return "hello i advertised"


# take in an advertisement from a neighbouring router and update your routing table accordingly
def update(current: dict, update: json):
    pass