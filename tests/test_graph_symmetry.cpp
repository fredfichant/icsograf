#include <cassert>

#include "edge.hpp"
#include "graph.hpp"
#include "graph_symmetry_analyzer.hpp"
#include "node.hpp"

int main()
{
    Graph graph;
    Node first(QPointF(0, 0));
    Node second(QPointF(10, 0));
    Node third(QPointF(10, 10));
    Node fourth(QPointF(0, 10));
    graph.add_node(&first);
    graph.add_node(&second);
    graph.add_node(&third);
    graph.add_node(&fourth);
    Edge one(&first, &second);
    Edge two(&second, &third);
    Edge three(&third, &fourth);
    Edge four(&fourth, &first);
    graph.add_edge(&one);
    graph.add_edge(&two);
    graph.add_edge(&three);
    graph.add_edge(&four);

    const Graph_Symmetry_Result square = Graph_Symmetry_Analyzer::find_automorphisms(graph);
    assert(square.complete);
    assert(square.group_order == 8);
    assert(square.orbits.size() == 1);
    assert(square.orbits.front().size() == 4);
    assert(square.reflection_axes.size() == 4);

    four.set_strand_count(2);
    const Graph_Symmetry_Result marked = Graph_Symmetry_Analyzer::find_automorphisms(graph);
    assert(marked.group_order == 2);
    return 0;
}
