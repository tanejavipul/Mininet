import json

BROADCAST_PORT = 420
ROUTER_PORT = 58008
TYPE_INITIALIZE = "HOST_INITIALIZATION"
TYPE_ADVERTISE = "ADVERTISE"
TYPE_KEEP_ALIVE = "KEEP_ALIVE"
HOPS = "HOPS"
HOSTS = "HOSTS"
SEND_TO = "SEND_TO"
TYPE = "TYPE"
ADDRESS = "ADDRESS"
MESSAGE = "MESSAGE"
ROUTER_INTERFACE = "ROUTER_INTERFACE"

PORT = "PORT"
KEEP_ALIVE = "KEEP_ALIVE"

PROTOCOL = "PROTOCOL"
PROTOCOL_RIP = "PROTOCOL_RIP"
PROTOCOL_OSPF = "PROTOCOL_OSPF"
OSPF = "OSPF"
RIP = "RIP"

DEST_IP = "DEST_IP"
SOURCE_IP = "SOURCE_IP"
TTL = "TTL"

# KEEP ALIVE TIMING
ROUTER_NOT_ALIVE = 7.5
HOST_NOT_ALIVE = 6.5


# GENERAL FUNCTIONS
def convert_to_json(packet: dict):
    return json.dumps(packet).encode('utf-8')


def convert_to_dict(packet: bytes):
    return json.loads(packet.decode('utf-8'))


def copy_dict(d: dict):
    return d.copy()


# Make Packet
def make_packet(dest_ip, source_ip, ttl, protocol, message):
    output = {DEST_IP: str(dest_ip), SOURCE_IP: str(source_ip), TTL: str(ttl), PROTOCOL: str(protocol),
              MESSAGE: message}
    return json.dumps(output).encode('utf-8')


def update_TTL(packet):
    new_packet = json.loads(packet.decode('utf-8'))
    ttl = int(get_TTL(packet)) - 1
    new_packet['ttl'] = ttl
    return json.dumps(new_packet).encode('utf-8')


def get_DEST_IP(packet):
    return json.loads(packet.decode('utf-8'))[DEST_IP]


def get_TTL(packet):
    return json.loads(packet.decode('utf-8'))[TTL]


def get_SOURCE_IP(packet):
    return json.loads(packet.decode('utf-8'))[SOURCE_IP]


def get_MESSAGE(packet):
    return json.loads(packet.decode('utf-8'))[MESSAGE]


def get_PROTOCOL(packet):
    return json.loads(packet.decode('utf-8'))[PROTOCOL]


# BROADCAST FUNCTIONS
def make_broadcast_packet(type, address, ttl, message, router_interface):
    output = {TYPE: str(type), ADDRESS: str(address), MESSAGE: str(message), TTL: ttl,
              ROUTER_INTERFACE: str(router_interface)}
    return json.dumps(output).encode('utf-8')


def get(data, key):
    return data[key]
