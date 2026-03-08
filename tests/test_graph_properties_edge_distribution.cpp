#include <cassert>

#include "edge_normal.hpp"
#include "graph.hpp"

int main()
{
    Node n1(QPointF(0, 0));
    Node n2(QPointF(100, 0));
    Edge_Normal regular_type;

    Graph g;
    g.add_node(&n1);
    g.add_node(&n2);

    Edge e(&n1, &n2, &regular_type);
    g.add_edge(&e);
    e.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));

    g.render_knot();

    const int total = g.properties()->wa() + g.properties()->w0() + g.properties()->p0() +
                      g.properties()->pa();
    assert(total > 0);

    return 0;
}
