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

struct EdgeDistributionTable
{
    std::string label;
    int wa = 0;
    int w0 = 0;
    int p0 = 0;
    int pa = 0;

    bool operator==(const EdgeDistributionTable& other) const
    {
        return label == other.label && wa == other.wa && w0 == other.w0 && p0 == other.p0 &&
               pa == other.pa;
    }
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
    std::vector<EdgeDistributionTable> edge_distribution_tables_from_linear_solutions(
        const Graph& graph, const std::vector<std::vector<size_t>>& faces);
    std::vector<std::vector<int>> edge_assignments_from_linear_solutions(
        const Graph& graph, const std::vector<std::vector<size_t>>& faces);
    std::vector<EdgeDistributionTable> edge_distribution_tables(
        const Graph& graph, const std::vector<std::vector<size_t>>& faces);

   private:
    struct LinearSystemSolution
    {
        bool consistent = false;
        std::vector<bool> particular;
        std::vector<std::vector<bool>> nullspace;
    };

    LinearSystemSolution solve_marking_system(const Graph& graph,
                                              const std::vector<std::vector<size_t>>& faces);
    std::vector<std::string> encodeSymbols(const std::vector<int>& values);
};

#endif  // EDGES_MARK_HPP
