/**
 * \file tests/test_graph_first_edge_3strand.cpp
 * \brief Unit tests for graph first edge 3-strand.
 */

#include <cassert>

#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

static void run_case(Edge_Type* type)
{
    Node n1(QPointF(-160, -80));
    Node n2(QPointF(-40, -10));
    Node n3(QPointF(90, -90));

    Edge_Normal regular_type;

    Graph g;
    g.add_node(&n1);
    g.add_node(&n2);
    g.add_node(&n3);

    // First drawn edge is the one changed to 3-strand.
    Edge e1(&n1, &n2, &regular_type);
    Edge e2(&n2, &n3, &regular_type);
    g.add_edge(&e1);
    g.add_edge(&e2);

    e1.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));
    e2.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));

    g.render_knot();
    const int baseline_groups = g.properties()->group_count();

    e1.set_style(Edge_Style(24, 10, 0.5, type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();

    // Must stay bounded: no runaway loop creating many groups.
    assert(g.properties()->group_count() >= baseline_groups);
    assert(g.properties()->group_count() <= baseline_groups + 3);
    assert(e2.style().edge_type->machine_name() == "regular");
}

int main()
{
    Edge_3Strand normal;
    Edge_3Strand_Inverted inverted;

    run_case(&normal);
    run_case(&inverted);

    return 0;
}
