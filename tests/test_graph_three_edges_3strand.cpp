#include <cassert>

#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

int main()
{
    Node n1(QPointF(-200, 0));
    Node n2(QPointF(-50, 80));
    Node n3(QPointF(120, 60));
    Node n4(QPointF(80, -120));

    Edge_Normal regular_type;
    Edge_3Strand three_strand_type;

    Graph g;
    g.add_node(&n1);
    g.add_node(&n2);
    g.add_node(&n3);
    g.add_node(&n4);

    Edge e1(&n1, &n2, &regular_type);
    Edge e2(&n2, &n3, &regular_type);
    Edge e3(&n3, &n4, &regular_type);
    g.add_edge(&e1);
    g.add_edge(&e2);
    g.add_edge(&e3);

    e1.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));
    e2.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));
    e3.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));

    g.render_knot();
    const int baseline_groups = g.properties()->group_count();

    e3.set_style(Edge_Style(24, 10, 0.5, &three_strand_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();

    assert(e1.style().edge_type->machine_name() == "regular");
    assert(e2.style().edge_type->machine_name() == "regular");
    assert(g.properties()->group_count() >= baseline_groups);
    assert(g.properties()->group_count() <= baseline_groups + 2);

    return 0;
}
