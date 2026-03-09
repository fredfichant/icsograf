#include <cassert>

#include "edge_2strand.hpp"
#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

static int edge_distribution_total(const Graph_Properties* props)
{
    return props->wa() + props->w0() + props->p0() + props->pa();
}

int main()
{
    Node n1(QPointF(0, 0));
    Node n2(QPointF(120, 0));

    Edge_Normal regular_type;
    Edge_2Strand two_strand_type;
    Edge_2Strand_Inverted two_strand_inverted_type;
    Edge_3Strand three_strand_type;

    Graph g;
    g.add_node(&n1);
    g.add_node(&n2);

    Edge e(&n1, &n2, &regular_type);
    g.add_edge(&e);

    e.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    assert(g.properties()->edge_count() == 1);
    assert(g.properties()->vertex_degree_distribution().value(1, 0) == 2);
    assert(edge_distribution_total(g.properties()) == 1);

    e.set_style(Edge_Style(24, 10, 0.5, &two_strand_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    assert(g.properties()->edge_count() == 2);
    assert(g.properties()->vertex_degree_distribution().value(2, 0) == 2);
    assert(edge_distribution_total(g.properties()) == 2);
    assert(g.properties()->p0() + g.properties()->pa() == 0);

    e.set_style(Edge_Style(24, 10, 0.5, &three_strand_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    assert(g.properties()->edge_count() == 3);
    assert(g.properties()->vertex_degree_distribution().value(3, 0) == 2);
    assert(edge_distribution_total(g.properties()) == 3);
    assert(g.properties()->p0() + g.properties()->pa() == 0);

    e.set_style(
        Edge_Style(24, 10, 0.5, &two_strand_inverted_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    assert(g.properties()->edge_count() == 2);
    assert(g.properties()->vertex_degree_distribution().value(2, 0) == 2);
    assert(edge_distribution_total(g.properties()) == 2);
    assert(g.properties()->wa() + g.properties()->w0() == 0);

    return 0;
}
