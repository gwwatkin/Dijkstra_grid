import os
import datetime
import matplotlib.pyplot as plt
from typing import *


slow_values = list(map(lambda x: x*100,range(1,11)))
fast_values = [2,3,4,5,10,15,20,30]

values = fast_values


if __name__ == "__main__":

    dijsktra_results : List[float] = []
    jhonson_results : List[float] = []


    for v in values:
        # Make a grid graph
        os.system("./cmake-build-debug/makeGrid %i %i testGraph.txt testSource.txt"%(v,v/2))

        # Benchmark against dijkstraX
        start = datetime.datetime.now()
        os.system("./cmake-build-debug/dijkstraX -e testGraph.txt -s testSource.txt -w testOut_dijkstra.txt")
        finish = datetime.datetime.now()
        dijkstra_tdiff = finish - start

        # Run JhonsonX
        start = datetime.datetime.now()
        os.system("./cmake-build-debug/jhonsonX -e testGraph.txt -s testSource.txt -w testOut_jhonson.txt")
        finish = datetime.datetime.now()
        jhonson_tdiff = finish - start

        print(v, dijkstra_tdiff, jhonson_tdiff)
        dijsktra_results.append(dijkstra_tdiff.total_seconds())
        jhonson_results.append(jhonson_tdiff.total_seconds())

    l1,l2 = plt.plot(values, dijsktra_results, values, jhonson_results)
    plt.title("Shortest path finding, 50% source nodes")
    plt.ylabel("Time in seconds")
    plt.xlabel("Code distance")
    plt.legend((l1,l2),("Dijkstra's running time", "Jhonson's Running time"))
    plt.show()