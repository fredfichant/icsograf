#include <cassert>

#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

int main()
{
    Node n1(QPointF(0, 0));
    Node n2(QPointF(120, 0));

    Edge_Normal regular_type;
    Edge_3Strand three_strand_type;

    Graph g;
    g.add_node(&n1);
    g.add_node(&n2);

    Edge e(&n1, &n2, &regular_type);
    g.add_edge(&e);

    e.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();

    const int baseline_faces = g.properties()->face_count();
    const int baseline_deg2 = g.properties()->face_degree_distribution().value(2, 0);

    e.set_style(Edge_Style(24, 10, 0.5, &three_strand_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    assert(g.properties()->face_count() == baseline_faces + 2);
    assert(g.properties()->face_degree_distribution().value(2, 0) == baseline_deg2 + 2);

    e.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    assert(g.properties()->face_count() == baseline_faces);
    assert(g.properties()->face_degree_distribution().value(2, 0) == baseline_deg2);

    return 0;
}
