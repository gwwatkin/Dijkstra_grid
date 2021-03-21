#include <optional>
#include <string>
#include <sstream>
#include <exception>
#include "GridWithRoughBoundaries.h"

using namespace std::string_literals;


std::ostream& operator<<(std::ostream& os, const Vertex& v)
{
    os<<"Vertex{"s<<std::to_string(v.row)<<","s<<std::to_string(v.col)<<"}"s;
    return os;
}

int detail::compute_num_edges(int side_nodes_count)
{
    return (side_nodes_count-1)*side_nodes_count
         + (side_nodes_count-1)*(side_nodes_count-2);
}


/**
 * Get the position of an edge in storage according to the position on the grid.
 * 0,0   0,1   0,2   0,3
 *  |     |     |     |
 * 1,0 - 1,1 - 1,2 - 1,3
 *  |     |     |     |
 * 2,0 - 2,1 - 2,2 - 2,3
 *  |     |     |     |
 * 3,0   3,1   3,2   3,3
 */


bool detail::are_grid_neighbours(Vertex s, Vertex t)
{
    return (s.row-t.row)*(s.col-t.col) == 0
           and std::abs(s.row-t.row) + std::abs(s.col-t.col) == 1;
}


bool detail::are_vertical_boundary_nodes(int side_node_count, Vertex s, Vertex t)
{
    return (s.row==0 and t.row==0) or (s.row==side_node_count-1 and t.row==side_node_count-1);
}



std::optional<int> detail::get_storage_index_for_weight(int side_node_count, Vertex s, Vertex t)
{
    using namespace detail;

     Vertex first{std::min(s.row,t.row),std::min(s.col,t.col)};
     Vertex second{std::max(s.row,t.row),std::max(s.col,t.col)};

     if(!are_grid_neighbours(s,t) || are_vertical_boundary_nodes(side_node_count,s,t))
         return std::nullopt;

     if(first.col == second.col) // Vertical case:
     {
         // 0,0   0,1   0,2   0,3
         //  |     |     |     |
         // 1,0   1,1   1,2   1,3
         //  |     |     |     |
         // 2,0   2,1   2,2   2,3
         //  |     |     |     |
         // 3,0   3,1   3,2   3,3
         return (side_node_count-1)*first.col + first.row;
     }
     else if (first.row == second.row) // Horizontal case
     {
         // 0,0   0,1   0,2   0,3
         //
         // 1,0 - 1,1 - 1,2 - 1,3
         //
         // 2,0 - 2,1 - 2,2 - 2,3
         //
         // 3,0   3,1   3,2   3,3
         int horizontal_node_offset = (side_node_count-1)*side_node_count;
         return horizontal_node_offset + (first.row-1)*(side_node_count-1) + first.col;
     }

    return std::nullopt;
}



void GridWithRoughBoundaries::traverse_edges(const std::function<void(Vertex,Vertex)>& f) const
{
    for (int col = 0; col < side_length_; ++col)
        for (int row = 0; row < side_length_-1; ++row)
            f(Vertex{row, col}, Vertex{row+1, col});

    for (int row = 1; row < side_length_-1; ++row)
        for (int col = 0; col < side_length_ - 1; ++col)
            f(Vertex{row, col}, Vertex{row, col+1});


}

void GridWithRoughBoundaries::traverse_nodes(const std::function<void(Vertex)>& f) const
{
    for (int col = 0; col < side_length_; ++col)
        for (int row = 0; row < side_length_; ++row)
            f(Vertex{row, col});
}



GridWithRoughBoundaries::GridWithRoughBoundaries(int side_nodes_count)
: side_length_(side_nodes_count), weights_(detail::compute_num_edges(side_nodes_count),0.0)
{}


double GridWithRoughBoundaries::edge_weight(const Vertex& s, const Vertex& t) const {
    auto maybe_idx = detail::get_storage_index_for_weight(side_length_,s,t);
    if(maybe_idx) return weights_.at(*maybe_idx);
    return std::numeric_limits<double>::infinity();
}

void GridWithRoughBoundaries::edge_weight(const Vertex& s, const Vertex& t, double new_value) {
    auto maybe_idx = detail::get_storage_index_for_weight(side_length_,s,t);
    if(maybe_idx)
    {
        weights_.at(*maybe_idx) = new_value;
        return;
    }
    std::ostringstream ss("");
    ss<<"Nodes not neighbours of grid: "<<s<<" and "<<t;
    throw std::runtime_error(ss.str());
}


GridWithRoughBoundaries::VertexLabel GridWithRoughBoundaries::label(const Vertex& s) const {
    return s.row*side_length_ + s.col;
}

Vertex GridWithRoughBoundaries::vertex_from_label(GridWithRoughBoundaries::VertexLabel l) const {
    return Vertex{static_cast<int>(l/this->nodes_per_side()), static_cast<int>(l%this->nodes_per_side())};
}


int GridWithRoughBoundaries::num_edges() const
{
    return detail::compute_num_edges(nodes_per_side());
}


GridWithRoughBoundariesBoostGraph::GridWithRoughBoundariesBoostGraph(const GridWithRoughBoundaries& g)
:graph_ref(g) { }

GridWithRoughBoundariesBoostGraph::VertexLabelIterator GridWithRoughBoundariesBoostGraph::get_vertex_iterator() const {
    return 0;
}