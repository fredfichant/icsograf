#include <cassert>

#include "edge_2strand.hpp"
#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

static int digit_sum(const QString& word)
{
    int total = 0;
    for (QChar c : word) {
        assert(c.isDigit());
        total += c.digitValue();
    }
    return total;
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
    assert(g.properties()->span_formula() == "111");

    e3.set_style(Edge_Style(24, 10, 0.5, &two_strand_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    const QString span2 = g.properties()->span_formula();
    assert(span2.size() == 4);
    assert(digit_sum(span2) == 4);

    e2.set_style(Edge_Style(24, 10, 0.5, &three_strand_type, Edge_Style::EVERYTHING, 10, 1));
    g.render_knot();
    const QString span3 = g.properties()->span_formula();
    assert(span3.size() == 6);
    assert(digit_sum(span3) == 6);

    return 0;
}
