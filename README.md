# Mininet

**MultiNetworkRouter.py** -> this router automatically binds to it interfaces and send messages to neighboring routers to advertise what the router can connect or send its topology to OSPF node and OSPF will calculate and send each router there fastest routes to other routers.


**SimpleEndSystem.py** -> this is an end system which first broadcast a message to router to let router know it exist. Now an end system can send messages to other end systems.


**ForwardTable.py** -> this is set of functions used to compile OSPF request/responses and RIP advertisements and RIP updates to forward tables.


**Monitor.py** -> Monitor is the OSPF node which send requests to router for topology updates. It sends updated fastest topology response for each router if topology has changed. It uses dijkstras.py and pqdict.py to calculate fastest path for each router. 


**Packets.py** -> this is a set of functions which either compile OSPF/RIP packets or updates them like updating the TTL or updating the delays. Packets are converted to json to

Routers can detect disconnects of neighboring routers or host.




## Steps To Run

First either download mininet on a VM or directly on your machine

> http://mininet.org/


Then go to Mininet directory

> sudo python3 multi-router.py


Open the nodes in xterm

> xterm r1 r2 r3 h1 h2 h3


Now either run MultiNetworkRouter.py in the router xterm window

> sudo python3 MultiNetworkRouter.py

or 

run the SimpleEndSystem.py in the host xterm window

> sudo python3 SimpleEndSystem.py {ANY IP ADDRESS}




## SimpleEndSystem Commands List

> {IP YOU WANT TO SEND TO} {OSPF/RIP} {MESSAGE}

To send subnet broadcast

> <BROADCAST> {MESSAGE}
  
  
## MultiNetworkRouter Command List (**Report** also contain the list of commands)

> PRINT FORWARD TABLE
> PRINT NEIGHBORS
> PRINT HOSTS
> PRINT TOPOLOGY
  
To set router delay 

> SET DELAY <int>
  






