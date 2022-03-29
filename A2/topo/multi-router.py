# RITVIK AND VIPUL COLAB

##################### ROUTER TOPOLGY #####################
#           ┌───┐       ┌───┐
#   ┌───────┤r2 ├───────┤r3 ├──────┐
#   │       └─┬─┘       └─┬─┘      │
#   │         │           │        │
#   │         └───┐   ┌───┘        │
# ┌─┴─┐           ├───┤          ┌─┴─┐
# │r1 ├───────────┤r6 ├──────────┤r4 │
# └─┬─┘           └─┬─┘          └─┬─┘
#   │               │              │
#   │               │              │
#   │             ┌─┴─┐            │
#   └─────────────┤r5 ├────────────┘
#                 └───┘
# *NOTE: R6 can be used a MONITOR NODE or a MULTI-ROUTER.*
##################### ROUTER TOPOLGY #####################

# !/usr/bin/python
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI


class LinuxRouter(Node):
    def config(self, **params):
        super(LinuxRouter, self).config(**params)
        self.cmd('sysctl net.ipv4.ip_forward=1')

    def terminate(self):
        self.cmd('sysctl net.ipv4.ip_forward=0')
        super(LinuxRouter, self).terminate()


NUM_NODES = 6
connect = [(0, 1), (1, 2), (2, 3), (0, 4), (4, 3)]
connect.extend([(0, 5), (1, 5), (2, 5), (3, 5), (4, 5)])
made_connections = []


class NetworkTopo(Topo):
    def build(self, **_opts):

        r = []
        s = []
        # Add NUM_ROUTERS to r
        for x in range(NUM_NODES):
            r.append(self.addHost(f'r{x + 1}', cls=LinuxRouter, ip=f'10.{x}.0.1/24'))  # 10.0.0.1 10.1.0.1 10.2.0.1
            print(r[x])

        monitor_node = self.addHost('monitor', cls=LinuxRouter, ip=f'30.0.0.1/24')

        # Add NUM_NODES to s
        for x in range(NUM_NODES):
            s.append(self.addSwitch(f's{x + 1}'))

        # Add host-switch links in the same subnet
        for i in range(NUM_NODES):
            self.addLink(s[i],
                         r[i],
                         intfName2=f'r{i}-eth1',
                         params2={'ip': f'10.{i}.0.1/24'}
                         )

        counter = {}

        # Add router-router link in a new subnet for the router-router connection
        for router1, router2 in connect:
            assign_ip_addr = max(counter.get(router1, 1), counter.get(router2, 1))
            link1 = f'r{router1 + 1}-LINK-r{router2 + 1}'
            link2 = f'r{router2 + 1}-LINK-r{router1 + 1}'
            params1 = f'10.{assign_ip_addr}0.0.1'
            params2 = f'10.{assign_ip_addr}0.0.2'

            self.addLink(r[router1],
                         r[router2],
                         intfName1=link1,
                         intfName2=link2,
                         params1={'ip': f'{params1}/24'},
                         params2={'ip': f'{params2}/24'}
                         )

            made_connections.append((f'r{router1 + 1}', f'10.{router2}.0.0/24', params2, link1))
            made_connections.append((f'r{router2 + 1}', f'10.{router1}.0.0/24', params1, link2))

            counter[router1] = assign_ip_addr + 1
            counter[router2] = assign_ip_addr + 1

        hosts = []
        for i in range(NUM_NODES):
            hosts.append(self.addHost(
                name=f'h{i + 1}',
                ip=f'10.{i}.0.251/24',
                defaultRoute=f'via 10.{i}.0.1'
            ))
            self.addLink(hosts[i], s[i])


def run():
    topo = NetworkTopo()
    net = Mininet(topo=topo)

    # Add routing for reaching networks that aren't directly connected
    for routerName, ip, gateway, eth in made_connections:
        info(net[routerName].cmd(f'ip route add {ip} via {gateway} dev {eth}'))

    net.start()
    CLI(net)
    net.stop()


if __name__ == '__main__':
    setLogLevel('info')
    run()