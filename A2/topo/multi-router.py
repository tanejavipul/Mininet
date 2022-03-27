# RITVIK AND VIPUL COLAB

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


NUM_NODES = 5
connect = [(0, 1), (1, 2), (1, 3), (0, 4), (4, 3)]

made_connections = []


class NetworkTopo(Topo):
    def build(self, **_opts):

        r = []
        s = []
        # Add NUM_ROUTERS to r
        for x in range(NUM_NODES):
            r.append(self.addHost(f'r{x + 1}', cls=LinuxRouter, ip=f'10.{x}.0.1/24'))  # 10.0.0.1 10.1.0.1 10.2.0.1
            print(r[x])

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

    # Add routing for reaching networks that aren't directly connected
    # info(net['r1'].cmd("ip route add 10.1.0.0/24 via 10.10.0.2 dev r1-LINK-r2"))
    # info(net['r2'].cmd("ip route add 10.0.0.0/24 via 10.10.0.1 dev r2-LINK-r1"))

    # #add r1 to r3
    # info(net['r1'].cmd("ip route add 10.2.0.0/24 via 10.20.0.2 dev r1-LINK-r3"))
    # info(net['r3'].cmd("ip route add 10.0.0.0/24 via 10.20.0.1 dev r3-LINK-r1"))

    net.start()
    CLI(net)
    net.stop()


if __name__ == '__main__':
    setLogLevel('info')
    run()