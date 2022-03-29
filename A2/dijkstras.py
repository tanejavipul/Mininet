# THIS CODE WAS TAKEN FROM GITHUB USER : mburst (https://github.com/mburst)
# LINK: https://github.com/mburst/dijkstras-algorithm

import heapq
import sys
from pqdict import PQDict

class Graph:
    
    def __init__(self):
        self.vertices = {}
        self.dist = None
        self.pred = None
        
    def add_vertex(self, name, edges):
        self.vertices[name] = edges
    
    def shortest_path(self, start, finish):
        distances = {} # Distance from start to node
        previous = {}  # Previous node in optimal path from source
        nodes = [] # Priority queue of all nodes in Graph

        for vertex in self.vertices:
            if vertex == start: # Set root node as distance of 0
                distances[vertex] = 0
                heapq.heappush(nodes, [0, vertex])
            else:
                distances[vertex] = sys.maxsize
                heapq.heappush(nodes, [sys.maxsize, vertex])
            previous[vertex] = None
        
        while nodes:
            smallest = heapq.heappop(nodes)[1] # Vertex in nodes with smallest distance in distances
            if smallest == finish: # If the closest node is our target we're done so print the path
                path = []
                while previous[smallest]: # Traverse through nodes til we reach the root which is 0
                    path.append(smallest)
                    smallest = previous[smallest]
                return path
            if distances[smallest] == sys.maxsize: # All remaining vertices are inaccessible from source
                break
            
            for neighbor in self.vertices[smallest]: # Look at all the nodes that this vertex is attached to
                alt = distances[smallest] + self.vertices[smallest][neighbor] # Alternative path distance
                if alt < distances[neighbor]: # If there is a new shortest path update our priority queue (relax)
                    distances[neighbor] = alt
                    previous[neighbor] = smallest
                    for n in nodes:
                        if n[1] == neighbor:
                            n[0] = alt
                            break
                    heapq.heapify(nodes)
        return distances

    # EDITED
    # this function returns the next node to go to for the shortest path
    def shortest_path_next(self, start, finish):
        output = self.shortest_path(start, finish)
        if type(output) == dict:
            return None
        return self.shortest_path(start, finish)[-1]

    def remove_vertex(self, key):
        self.vertices.pop(key)

    def __str__(self):
        return str(self.vertices)


    def all_shortest_paths(self, start):
        self.dist, self.pred = dijkstra(self.vertices, start)


    def fast_shortest_path(self,start, end):
        try:
            v = end
            path = [v]
            while v != start:
                v = self.pred[v]
                path.append(v)
            path.reverse()
        except:
            return None

        return path[1:]







# Taken from https://github.com/joyrexus/dijkstra
def dijkstra(G, start, end=None):
    '''
    dijkstra's algorithm determines the length from `start` to every other
    vertex in the graph.
    The graph argument `G` should be a dict indexed by nodes.  The value
    of each item `G[v]` should also a dict indexed by successor nodes.
    In other words, for any node `v`, `G[v]` is itself a dict, indexed
    by the successors of `v`.  For any directed edge `v -> w`, `G[v][w]`
    is the length of the edge from `v` to `w`.
        graph = {'a': {'b': 1},
                 'b': {'c': 2, 'b': 5},
                 'c': {'d': 1},
                 'd': {}}
    Returns two dicts, `dist` and `pred`:
        dist, pred = dijkstra(graph, start='a')

    `dist` is a dict mapping each node to its shortest distance from the
    specified starting node:
        assert dist == {'a': 0, 'c': 3, 'b': 1, 'd': 4}
    `pred` is a dict mapping each node to its predecessor node on the
    shortest path from the specified starting node:
        assert pred == {'b': 'a', 'c': 'b', 'd': 'c'}

    '''
    inf = float('inf')
    D = {start: 0}  # mapping of nodes to their dist from start
    Q = PQDict(D)  # priority queue for tracking min shortest path
    P = {}  # mapping of nodes to their direct predecessors
    U = set(G.keys())  # unexplored nodes

    while U:  # nodes yet to explore
        (v, d) = Q.popitem()  # node w/ min dist d on frontier
        D[v] = d  # est dijkstra greedy score
        U.remove(v)  # remove from unexplored
        if v == end: break

        # now consider the edges from v with an unexplored head -
        # we may need to update the dist of unexplored successors
        for w in G[v]:  # successors to v
            if w in U:  # then w is a frontier node
                d = D[v] + G[v][w]  # dgs: dist of start -> v -> w
                if d < Q.get(w, inf):
                    Q[w] = d  # set/update dgs
                    P[w] = v  # set/update predecessor

    return D, P


if __name__ == '__main__':
    g = Graph()
    g.add_vertex('A', {'B': 7, 'C': 8})
    g.add_vertex('B', {'A': 7, 'F': 2})
    g.add_vertex('C', {'A': 8, 'F': 6, 'G': 4})
    g.add_vertex('D', {'F': 8})
    g.add_vertex('E', {'H': 1})
    g.add_vertex('F', {'B': 2, 'C': 6, 'D': 8, 'G': 9, 'H': 3})
    g.add_vertex('G', {'C': 4, 'F': 9})
    g.add_vertex('H', {'E': 1, 'F': 3})
    # g.vertices = {'A': {'B': 7, 'C': 8},  'C': {'A': 8, 'F': 6, 'G': 4}, 'D': {'F': 8}, 'E': {'H': 1}, 'F': {'B': 2, 'C': 6, 'D': 8, 'G': 9, 'H': 3}, 'G': {'C': 4, 'F': 9}, 'H': {'E': 1, 'F': 3}}
    # g.vertices = {'10.0.0.1': {'10.1.0.1': 1, '2.2.2.2': 0}, '10.1.0.1': {'10.0.0.1': 1, '9.9.9.9': 0}, "2.2.2.2": {}, "9.9.9.9": {}}
    print(g)
    print(g.shortest_path('A', 'H'))
    g.all_shortest_paths("A")
    print(g.fast_shortest_path("A","H"))
