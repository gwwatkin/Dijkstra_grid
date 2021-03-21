#ifndef DIJKSTRA_GRID_GRIDWITHROUGHBOUNDARIES_H
#define DIJKSTRA_GRID_GRIDWITHROUGHBOUNDARIES_H

#include <vector>
#include <ostream>
#include <optional>
#include <functional>

// 0,0   0,1   0,2   0,3
//  |     |     |     |
// 1,0 - 1,1 - 1,2 - 1,3
//  |     |     |     |
// 2,0 - 2,1 - 2,2 - 2,3
//  |     |     |     |
// 3,0   3,1   3,2   3,3


struct Vertex
{
    int row;
    int col;
};

std::ostream& operator<<(std::ostream& os, const Vertex& v);

class GridWithRoughBoundaries {
public:

    using VertexLabel = int;
    using EdgeLabel = std::pair<VertexLabel,VertexLabel>;
    using EdgeWeight = double;

    explicit GridWithRoughBoundaries(int side_nodes_count);


    EdgeWeight edge_weight(const Vertex& s, const Vertex& t) const;
    void edge_weight(const Vertex& s, const Vertex& t, double new_value);

    /// Get a unique label for each vertex
    VertexLabel label(const Vertex& s) const;
    Vertex vertex_from_label(VertexLabel l) const;

    void traverse_nodes(const std::function<void(Vertex)>& f) const;
    void traverse_edges(const std::function<void(Vertex,Vertex)>& f) const;

    int nodes_per_side() const {return side_length_;}
    int num_edges() const;



private:
    const int side_length_;
    std::vector<double> weights_;
};



class GridWithRoughBoundariesBoostGraph
{
public:

    explicit GridWithRoughBoundariesBoostGraph(const GridWithRoughBoundaries& g);


    using VertexLabelIterator = std::vector<GridWithRoughBoundaries::VertexLabel>::iterator;
    using EdgeLabelIterator = std::vector<GridWithRoughBoundaries::EdgeLabel>::iterator;

    VertexLabelIterator get_vertex_iterator() const;
    EdgeLabelIterator get_edge_iterator() const;

private:
    const GridWithRoughBoundaries& graph_ref;
    std::vector<GridWithRoughBoundaries::EdgeLabel> edge_list;

};



namespace detail
{
    bool are_grid_neighbours(Vertex s, Vertex t);
    std::optional<int> get_storage_index_for_weight(int side_node_count, Vertex s, Vertex t);
    bool are_vertical_boundary_nodes(int side_node_count, Vertex s, Vertex t);
    int compute_num_edges(int side_nodes_count);

}


#endif //DIJKSTRA_GRID_GRIDWITHROUGHBOUNDARIES_H
