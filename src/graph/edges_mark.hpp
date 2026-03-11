/**
 * \file src/graph/edges_mark.hpp
 * \brief API declarations for edges mark graph data structures and operations.
 */

#ifndef EDGES_MARK_HPP
#define EDGES_MARK_HPP

#include <map>
#include <string>
#include <vector>

class Edge;
class Node;
class Graph;

struct EdgeMarking
{
    std::string marking;  // "a", "0", or multiple like "a,b,0,1"
    bool has_unique_solution;
    bool is_a;  // only if has_unique_solution
};

class GraphMarker
{
   public:
    struct InternalEdge
    {
        int u, v;
        int id;
        InternalEdge(int u, int v, int id) : u(u), v(v), id(id) {}
    };

    struct InternalFace
    {
        std::vector<int> vertices;
        int degree() const { return vertices.size(); }
        bool isOdd() const { return degree() % 2 == 1; }
    };

    std::map<const Edge*, EdgeMarking> mark_graph(const Graph& graph,
                                                  const std::vector<std::vector<size_t>>& faces);

   private:
    std::vector<std::string> encodeSymbols(const std::vector<int>& values);
};

#endif  // EDGES_MARK_HPP
