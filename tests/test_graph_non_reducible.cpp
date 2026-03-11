/**
 * \file tests/test_graph_non_reducible.cpp
 * \brief Unit tests for graph non-reducible.
 */

#include <algorithm>
#include <cassert>

#include "edge_2strand.hpp"
#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

static int max_span_digit(const QString& span_formula)
{
    int pm = 0;
    for (QChar c : span_formula) {
        if (c.isDigit()) pm = std::max(pm, c.digitValue());
    }
    return pm;
}

static bool expected_non_reducible(const Graph_Properties* p)
{
    const int r = p->group_count();
    const int c = p->edge_count();
    const int pm = max_span_digit(p->span_formula());
    if (r <= pm + 1) return c >= (5 * pm - r - 1);
    return c >= (2 * (pm + r - 2));
}

int main()
{
    Node n1(QPointF(-80, -20));
    Node n2(QPointF(50, -20));
    Node n3(QPointF(-5, 101));

    Edge_Normal regular_type;
    Edge_2Strand two_strand_type;
    Edge_3Strand three_strand_type;

    Graph g;
    g.add_node(&n1);
    g.add_node(&n2);
    g.add_node(&n3);

    Edge e1(&n1, &n2, &regular_type);
    Edge e2(&n2, &n3, &regular_type);
    Edge e3(&n3, &n1, &regular_type);
    g.add_edge(&e1);
    g.add_edge(&e2);
    g.add_edge(&e3);

    e1.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));
    e2.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));
    e3.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));

    g.render_knot();
    assert(g.properties()->is_non_reducible() == expected_non_reducible(g.properties()));

    e3.set_style(Edge_Style(24, 10, 0.5, &two_strand_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    assert(g.properties()->is_non_reducible() == expected_non_reducible(g.properties()));

    e2.set_style(Edge_Style(24, 10, 0.5, &three_strand_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    assert(g.properties()->is_non_reducible() == expected_non_reducible(g.properties()));

    return 0;
}
