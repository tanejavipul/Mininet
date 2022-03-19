import json

BROADCAST_PORT = 4200
PACKET_PORT = 8008
TYPE_INITIALIZE = "HOST_INITIALIZATION"
TYPE_ADVERTISE = "ADVERTISE"


# GENERAL FUNCTIONS
def convert(packet: dict):
    return json.dumps(packet).encode('utf-8')


def copy_dict(d: dict):
    return d.copy()






# Make Packet
def make_packet(dest_ip, dest_port, ttl, source_ip, source_port, message):
    output = {"dest_ip": str(dest_ip), "dest_port": str(dest_port), "ttl": str(ttl), "source_ip": source_ip,
              "source_port": source_port, "message": message}

    return json.dumps(output).encode('utf-8')


def update_ttl(packet):
    new_packet = json.loads(packet.decode('utf-8'))
    print(get_ttl(packet))
    ttl = int(get_ttl(packet))
    ttl -= 1
    new_packet['ttl'] = ttl
    return json.dumps(new_packet).encode('utf-8')


def get_dest_ip(packet):
    return json.loads(packet.decode('utf-8'))['dest_ip']


def get_dest_port(packet):
    return json.loads(packet.decode('utf-8'))['dest_port']


def get_ttl(packet):
    return json.loads(packet.decode('utf-8'))['ttl']


def get_source_ip(packet):
    return json.loads(packet.decode('utf-8'))['source_ip']


def get_source_port(packet):
    return json.loads(packet.decode('utf-8'))['source_port']


def get_message(packet):
    return json.loads(packet.decode('utf-8'))['message']




# BROADCAST FUNCTIONS
def make_broadcast_packet(type, source_ip, source_port, ttl, message):
    output = {"type": str(type), "source_ip": str(source_ip), "source_port": str(source_port),
              "message": str(message), "ttl": ttl}
    return json.dumps(output).encode('utf-8')
