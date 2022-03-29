import json

BROADCAST_PORT = 8008
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
DELAY_STR = "DELAY"

FROM = "FROM"
TO = "TO"
TABLE = "TABLE"

# KEEP ALIVE TIMING
ROUTER_NOT_ALIVE = 7.5
HOST_NOT_ALIVE = 6.5

# ROUTER COMMANDS
PRINT_NAT = "PRINT NAT"
PRINT_FORWARD_TABLE = "PRINT FORWARD TABLE"
PRINT_NEIGHBORS = "PRINT NEIGHBORS"
PRINT_DELAY = "PRINT DELAY"
PRINT_TOPOLOGY = "PRINT TOPOLOGY"
SET_DELAY = "SET DELAY"
STR_NEIGHBORS = "NEIGHBORS"


MONITOR_REQUEST = "MONITOR_REQUEST"
MONITOR_RESPONSE = "MONITOR_RESPONSE"
MONITOR_TOPO = "MONITOR_TOPO"

# GENERAL FUNCTIONS
def convert_to_json(packet: dict):
    return json.dumps(packet).encode('utf-8')


def convert_to_dict(packet: bytes):
    return json.loads(packet.decode('utf-8'))


def copy_dict(d: dict):
    return d.copy()


# Make Packet
def make_packet(dest_ip, source_ip, ttl, protocol, message, delay=0):
    output = {DEST_IP: str(dest_ip), SOURCE_IP: str(source_ip), TTL: str(ttl), PROTOCOL: str(protocol),
              MESSAGE: message, DELAY_STR: delay}
    return json.dumps(output).encode('utf-8')


# BROADCAST FUNCTIONS
def make_broadcast_packet(type, address, ttl, message, router_interface):
    output = {TYPE: str(type), ADDRESS: str(address), MESSAGE: message, TTL: ttl,
              ROUTER_INTERFACE: str(router_interface)}
    return json.dumps(output).encode('utf-8')


def make_OSPF_packet(from_router, to_router):
    output = {TYPE: str(type), FROM: str(from_router), TO: str(to_router)}
    return json.dumps(output).encode('utf-8')


# Although this returns a new dictionary this return is not need as parameter dictionary is updated
def update_TTL(packet):
    packet[TTL] = int(packet[TTL]) - 1
    return packet


# Although this returns a new dictionary this return is not need as parameter dictionary is updated
def update_DELAY(packet, delay):
    packet[DELAY_STR] = int(packet[DELAY_STR]) + int(delay)
    return packet
