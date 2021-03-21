//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/config.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/array.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>


boost::grid_graph<2> make_boost_grid_graph()
{
    using namespace boost;

    typedef grid_graph<2> Graph;
    typedef graph_traits<Graph> Traits;

    // Create a 3x3, unwrapped grid_graph (Figure 3)
    boost::array<size_t, 2> lengths = {{3, 3}};
    Graph graph(lengths);

    // Do a round-trip test of the vertex index functions
    for (Traits::vertices_size_type v_index = 0;
         v_index<num_vertices(graph); ++v_index)
    {

        // The two indices should always be equal
        std::cout << "Index of vertex " << v_index << " is " <<
                  get(boost::vertex_index, graph, vertex(v_index, graph)) << std::endl;

    }

    // Do a round-trip test of the edge index functions
    for (Traits::edges_size_type e_index = 0;
         e_index<num_edges(graph); ++e_index)
    {

        // The two indices should always be equal
        std::cout << "Index of edge " << e_index << " is " <<
                  get(boost::edge_index, graph, edge_at(e_index, graph)) << std::endl;

    }
}


int main()
{
    using namespace boost;
    typedef adjacency_list<vecS, vecS, directedS, no_property,
                           property< edge_weight_t, int, property< edge_weight2_t, int > > > Graph;
    const int V = 6;
    typedef std::pair < int, int >Edge;
    Edge edge_array[] =
            { Edge(0, 1), Edge(0, 2), Edge(0, 3), Edge(0, 4), Edge(0, 5),
              Edge(1, 2), Edge(1, 5), Edge(1, 3), Edge(2, 4), Edge(2, 5),
              Edge(3, 2), Edge(4, 3), Edge(4, 1), Edge(5, 4)
            };
    const std::size_t E = sizeof(edge_array) / sizeof(Edge);
    Graph g(edge_array, edge_array + E, V);

    property_map < Graph, edge_weight_t >::type w = get(edge_weight, g);
    int weights[] = { 0, 0, 0, 0, 0, 3, -4, 8, 1, 7, 4, -5, 2, 6 };
    int *wp = weights;

    graph_traits < Graph >::edge_iterator e, e_end;
    for (boost::tie(e, e_end) = edges(g); e != e_end; ++e)
        w[*e] = *wp++;

    int D[V][V];
    johnson_all_pairs_shortest_paths(g, D);

    std::cout << "       ";
    for (int k = 0; k < V; ++k)
        std::cout << std::setw(5) << k;
    std::cout << std::endl;
    for (int i = 0; i < V; ++i) {
        std::cout << std::setw(3) << i << " -> ";
        for (int j = 0; j < V; ++j) {
            if (D[i][j] == (std::numeric_limits<int>::max)())
                std::cout << std::setw(5) << "inf";
            else
                std::cout << std::setw(5) << D[i][j];
        }
        std::cout << std::endl;
    }

    std::ofstream fout("figs/johnson-eg.dot");
    fout << "digraph A {\n"
         << "  rankdir=LR\n"
         << "size=\"5,3\"\n"
         << "ratio=\"fill\"\n"
         << "edge[style=\"bold\"]\n" << "node[shape=\"circle\"]\n";

    graph_traits < Graph >::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        fout << source(*ei, g) << " -> " << target(*ei, g)
             << "[label=" << get(edge_weight, g)[*ei] << "]\n";

    fout << "}\n";
    return 0;
}
