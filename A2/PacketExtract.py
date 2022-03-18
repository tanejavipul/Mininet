import json


# Make Packet
def make_packet(dest_ip, dest_port, TTL, source_ip, source_port, message):
    output = "|" + str(dest_ip)
    output += "|" + str(dest_port)
    output += "|" + str(TTL)
    output += "|" + str(source_ip)
    output += "|" + str(source_port)
    output += "|" + str(message)
    return output + "|"


def packet_split(packet):
    return packet.split('|')



def update_ttl(packet):
    lst = packet_split(packet)
    ttl = int(lst[3])
    ttl -= 1
    lst[3] = str(ttl)
    return "|".join(lst)





def get_dest_ip(packet):
    return packet_split(packet)[1]


def get_dest_port(packet):
    return packet_split(packet)[2]

# Subtract TTL
def get_TTL(packet):
    return packet_split(packet)[3]


def get_source_ip(packet):
    return packet_split(packet)[4]


def get_source_port(packet):
    return packet_split(packet)[5]


def get_message(packet):
    return packet_split(packet)[6]


