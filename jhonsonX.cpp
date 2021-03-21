//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <stdio.h>

#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/property_map/property_map.hpp>

#include <typeinfo>

using namespace boost;

void LoadFile(int& node_num, int& edge_num, int*& edges_flat, int*& weights, char* filename)
{
    int e = 0;
    char LINE[1000];
    FILE* fp = fopen(filename, "r");
    if (!fp) { printf("Can't open %s\n", filename); exit(1); }

    int format = -1; // 0: DIMACS format. node id's start from 1
                     // 1: simpler format (without "p" and "e"). node id's start from 0

    edge_num = -1;
    while (fgets(LINE, sizeof(LINE)-1, fp))
    {
        if (LINE[0] == 'c') continue;
        if (format < 0)
        {
            if (LINE[0] == 'p')
            {
                format = 0;
                if (sscanf(LINE, "p edge %d %d\n", &node_num, &edge_num) != 2) { printf("%s: wrong format #1\n", filename); exit(1); }
            }
            else
            {
                format = 1;
                if (sscanf(LINE, "%d %d\n", &node_num, &edge_num) != 2) { printf("%s: wrong format #1\n", filename); exit(1); }
            }

            //////////////////////////////////////////////////////////////////////////////////
            if (node_num <= 0 || edge_num < 0) { printf("# of nodes and edges_flat should be positive\n"); exit(1); }
            edges_flat = new int[2*edge_num];
            weights = new int[edge_num];
            //////////////////////////////////////////////////////////////////////////////////
        }
        else
        {
            int i, j;
            char* ptr = LINE;
            if (format == 0) { if (LINE[0] != 'e') continue; ptr = &LINE[1]; }
            else             ptr = &LINE[0];

            int len;
            if (sscanf(ptr, "%d %d %d\n", &i, &j, &len) != 3) continue;
            if (format == 0) { i --; j --; }
            edges_flat[2*e] = i;
            edges_flat[2*e+1] = j;
            weights[e] = len;
            e ++;
        }
    }

    if (e != edge_num) { printf("%s: wrong format #3\n", filename); exit(1); }
    fclose(fp);
}

void LoadSourceInfo(int& source_num, int& boundary_num, int*& sources, int*& boundaries, char* filename)
{
    int e = 0;
    char LINE[1000];
    int* all_list;
    FILE* fp = fopen(filename, "r");
    if (!fp) { printf("Can't open %s\n", filename); exit(1); }

    int format = -1; // 0: DIMACS format. node id's start from 1
                     // 1: simpler format (without "p" and "e"). node id's start from 0

    source_num = -1;
    while (fgets(LINE, sizeof(LINE)-1, fp))
    {
        if (LINE[0] == 'c') continue;
        if (format < 0)
        {
            // The first row of the source file record the source_num and boundary_num
            if (LINE[0] == 'p')
            {
                format = 0;
                if (sscanf(LINE, "p edge %d %d\n", &source_num, &boundary_num) != 2) { printf("%s: wrong format #1\n", filename); exit(1); }
            }
            else
            {
                format = 1;
                if (sscanf(LINE, "%d %d\n", &source_num, &boundary_num) != 2) { printf("%s: wrong format #1\n", filename); exit(1); }
            }

            //////////////////////////////////////////////////////////////////////////////////
            if (source_num <= 0 || boundary_num < 0) { printf("# of nodes and edges_flat should be positive\n"); exit(1); }
            sources = new int[source_num];
            boundaries = new int[boundary_num];
            all_list = new int[source_num + boundary_num];
            //////////////////////////////////////////////////////////////////////////////////
        }
        else
        {
            int i,j;
            char* ptr = LINE;
            if (format == 0) { if (LINE[0] != 'e') continue; ptr = &LINE[1]; }
            else             ptr = &LINE[0];

            if (sscanf(ptr, "%d\n", &i) != 1) continue;
            if (format == 0) { i --; j --; }
            all_list[e] = i;
            e ++;
        }
    }

    if (e != (source_num + boundary_num)) { printf("%s: wrong format #3\n", filename); exit(1); }
    fclose(fp);

    // Although source nodes and boundary nodes are both in the Source file, they are loaded to separate variables: sources and boundaries
    // For the dijkstra, we are interested in:
    // 1. the distance between all source nodes
    // 2. the distance between every source node and its closest boundary node

    for (int j=0;j<source_num;j++) sources[j] = all_list[j];
    for (int j=0;j<boundary_num;j++) boundaries[j] = all_list[source_num+j];
    delete [] all_list;

}

void SaveDijkstra(int source_num, int pair_num, int* pairs_flat, int* dijkstra_weights, int* sb_pairs_flat, int* sb_pairs_weight, char* filename)
{
    FILE* fp = fopen(filename, "w");
    if (!fp) { printf("Can't open %s\n", filename); exit(1); }
    fprintf(fp, "%d %d\n", source_num*2, pair_num+source_num);
    int i;
    for (i=0; i<pair_num; i++)
    {
        fprintf(fp, "%d %d %d\n", pairs_flat[2*i], pairs_flat[2*i+1], dijkstra_weights[i]);
    }
    for (i=0; i<source_num; i++)
    {
        fprintf(fp, "%d %d %d\n", sb_pairs_flat[2*i], sb_pairs_flat[2*i+1], sb_pairs_weight[i]);
    }
    fclose(fp);
}

void ShowUsage()
{
    printf("Something went wrong\n");
    exit(1);
}


int main(int argc, char* argv[])
{

    typedef adjacency_list< listS, vecS, undirectedS, no_property,
        property< edge_weight_t, int > >
        graph_t;
    typedef graph_traits< graph_t >::vertex_descriptor vertex_descriptor;
    typedef std::pair< int, int > Edge;

    char* filename = NULL;
    char* save_filename = NULL;
    char* source_info_filename = NULL;


    int i, e, node_num, edge_num, source_num, boundary_num;
    int* edges_flat;
    int* weights;
    int* sources;
    int* boundaries;

    for (i=1; i<argc; i++)
    {
        if (argv[i][0] != '-') { printf("Unknown option: %s\n", argv[i]); ShowUsage(); }
        switch (argv[i][1])
        {
            case 'e':
                if (filename || argv[i][2] || ++i == argc) ShowUsage();
                filename = argv[i];
                break;
            case 's':
                if (source_info_filename || argv[i][2] || ++i == argc) ShowUsage();
                source_info_filename = argv[i];
                break;
            case 'w':
                if (save_filename || argv[i][2] || ++i == argc) ShowUsage();
                save_filename = argv[i];
                break;
            default:
                printf("Unknown option: %s\n", argv[i]);
                ShowUsage();
                break;
        }
        
    }

    if (!filename) ShowUsage();

    if (filename) LoadFile(node_num, edge_num, edges_flat, weights, filename);

    if (source_info_filename) LoadSourceInfo(source_num, boundary_num, sources, boundaries, source_info_filename);


    Edge edge_array[edge_num];

    for (e=0; e<edge_num; e++) edge_array[e] = Edge(edges_flat[2*e], edges_flat[2*e+1]);
    graph_t g(edge_array, edge_array + edge_num, weights, node_num);

    int ii, jj, pair_num = 0, pp = 0;

    for (int kk=1; kk<source_num; kk++) pair_num = pair_num + kk;

    // int pairs_flat[2*pair_num]; 
    int* pairs_flat = new int[2*pair_num];

    // int dijkstra_weights[pair_num]; 
    int* dijkstra_weights = new int[pair_num];
    
    // int sb_pairs_flat[2*source_num];
    int* sb_pairs_flat = new int[2*source_num];

    // int sb_pairs_weight[source_num];
    int* sb_pairs_weight = new int[source_num];
    int closest_b, closest_b_weight;

    // First compute all to all paths with Jhonson's, then extract only distances from sources to sources
    std::vector<std::vector<int>> output_distance_matrix (num_vertices(g),std::vector<int>(num_vertices(g),0));
    boost::johnson_all_pairs_shortest_paths(g, output_distance_matrix);

    for (ii=0; ii<source_num; ii++)
    {

        vertex_descriptor s = vertex(sources[ii], g);
        const std::vector<int>& d = output_distance_matrix[s];

        // After the dijkstra for source s, all distances to s is saved in d.
        // The following loop find the distance from s to other source nodes, save them to dijkstra_weights.
        for (jj=1; jj<(source_num-ii); jj++)
        {
            pairs_flat[2*pp] = sources[ii];
            pairs_flat[2*pp+1] = sources[ii+jj];

            dijkstra_weights[pp] = d[sources[ii+jj]];

            pp++;
            // std::cout << "pp is: " << pp << std::endl;

        }

        // The following loop is to find the closest boundadry node for each source.
        closest_b = boundaries[0];
        closest_b_weight = d[closest_b];
        for (int bb=1;bb<boundary_num; bb++){
            if (d[boundaries[bb]]<closest_b_weight) {
                closest_b = boundaries[bb];
                closest_b_weight = d[closest_b];
            }

        }

        sb_pairs_flat[2*ii] = sources[ii];
        sb_pairs_flat[2*ii+1] = closest_b;
        sb_pairs_weight[ii] = closest_b_weight;

    }

    
    if (save_filename) SaveDijkstra(source_num, pair_num, pairs_flat, dijkstra_weights, sb_pairs_flat, sb_pairs_weight, save_filename);



    if (filename)
    {
        delete [] edges_flat;
        delete [] weights;
    }

    if (source_info_filename)
    {
        delete [] sources;
        delete [] boundaries;
    }

    delete [] pairs_flat;
    delete [] dijkstra_weights;
    delete [] sb_pairs_flat;
    delete [] sb_pairs_weight;

    return EXIT_SUCCESS;
}
