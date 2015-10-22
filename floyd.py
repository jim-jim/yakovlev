import time
import sys
import xml.dom.minidom as dom
from math import ldexp
from pyfloyd import shortest_dists
if __name__ == "__main__":
    t_start = time.process_time()
    if len(sys.argv) < 3: print("You should specify input and output file paths"); exit()
    elem_type = dom.Element.ELEMENT_NODE
    doc = dom.parse(sys.argv[1])
    for node in doc.childNodes:
        if node.nodeName == "schematics": break
    if node.nodeName != "schematics": print("Can't find schematics"); exit()
    nodes = node.childNodes
    nets_d = {}
    for i in range(nodes.length):
        if nodes[i].nodeType != elem_type: continue
        if nodes[i].nodeName != "net": continue
        nets_d[(int)(nodes[i].getAttribute("id"))] = 0
    length = 0
    for x in sorted(nets_d):
        nets_d[x] = length
        length += 1
    W = [[[] for j in range(length)] for i in range(length)]
    for i in range(nodes.length):
        if nodes[i].nodeType != elem_type: continue
        name = nodes[i].nodeName
        if name == "diode":
            net_from, net_to = nets_d[(int)(nodes[i].getAttribute("net_from"))], nets_d[(int)(nodes[i].getAttribute("net_to"))]
            res, rev_res = (float)(nodes[i].getAttribute("resistance")), (float)(nodes[i].getAttribute("reverse_resistance"))
            W[net_from][net_to].append(res)
            W[net_to][net_from].append(rev_res)
        else:
            if name == "capactor" or name == "resistor":
                net_from, net_to = nets_d[(int)(nodes[i].getAttribute("net_from"))], nets_d[(int)(nodes[i].getAttribute("net_to"))]
                res = (float)(nodes[i].getAttribute("resistance"))
                W[net_from][net_to].append(res)
                W[net_to][net_from].append(res)
    delta = ldexp(1,-53)
    mxm = ldexp(1-delta,1024)
    eps = ldexp(1,-1074)
    inf = mxm * 2
    nan = inf - inf
    t_end = time.process_time()
    for i in range(length):
        for j in range(length):
            res = 0
            if i != j:
                a = W[i][j]
                if len(a) == 0: res = inf
                else:
                    if len(a) == 1: res = a[0]
                    else:
                        for item in a:
                            if item <= 0: res = inf; break
                            res += 1 / item
                        res = 1 / res
            W[i][j] = res
    #W = [[0,3,8,inf,-4],[inf,0,inf,1,7],[inf,4,0,inf,inf],[2,inf,-5,0,inf],[inf,inf,inf,6,0]]
    D = shortest_dists(W)
    with open(sys.argv[2], 'w') as outfile:
        for i in range(length):
            outfile.write(",".join("%.7f" % j for j in D[i]))
            outfile.write(",\n")
    t_time = t_end - t_start
    print((int)(t_time * 1000))