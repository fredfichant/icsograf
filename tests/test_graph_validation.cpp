#include <cassert>

#include "edge_2strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"
#include "graph_validation.hpp"

static Edge_Style style_for(Edge_Type* type)
{
    return Edge_Style(24, 10, 0.5, type, Edge_Style::EVERYTHING, 10, 1);
}

int main()
{
    Edge_Normal normal;
    Edge_2Strand two_strand;

    {
        // Disconnected graph
        Node a(QPointF(0, 0));
        Node b(QPointF(100, 0));
        Node c(QPointF(300, 0));
        Node d(QPointF(400, 0));
        Edge e1(&a, &b, &normal);
        Edge e2(&c, &d, &normal);
        Graph g;
        g.add_node(&a);
        g.add_node(&b);
        g.add_node(&c);
        g.add_node(&d);
        g.add_edge(&e1);
        g.add_edge(&e2);
        g.render_knot();

        const Graph_Validation_Result r = validate_graph(g);
        assert(!r.valid);
        assert(r.reason.contains("connected"));
    }

    {
        // Connected but not biconnected: chain of 4 nodes
        Node a(QPointF(0, 0));
        Node b(QPointF(100, 0));
        Node c(QPointF(200, 0));
        Node d(QPointF(300, 0));
        Edge e1(&a, &b, &normal);
        Edge e2(&b, &c, &normal);
        Edge e3(&c, &d, &normal);
        Graph g;
        g.add_node(&a);
        g.add_node(&b);
        g.add_node(&c);
        g.add_node(&d);
        g.add_edge(&e1);
        g.add_edge(&e2);
        g.add_edge(&e3);
        g.render_knot();

        const Graph_Validation_Result r = validate_graph(g);
        assert(!r.valid);
        assert(r.reason.contains("biconnected"));
    }

    {
        // Biconnected and contains an open simple-edge chain of length 4 (invalid)
        Node a(QPointF(0, 0));
        Node b(QPointF(240, 0));
        Node x1(QPointF(60, 60));
        Node x2(QPointF(120, 60));
        Node x3(QPointF(180, 60));
        Edge e1(&a, &x1, &normal);
        Edge e2(&x1, &x2, &normal);
        Edge e3(&x2, &x3, &normal);
        Edge e4(&x3, &b, &normal);
        Edge e5(&a, &b, &two_strand);
        Graph g;
        g.add_node(&a);
        g.add_node(&b);
        g.add_node(&x1);
        g.add_node(&x2);
        g.add_node(&x3);
        g.add_edge(&e1);
        g.add_edge(&e2);
        g.add_edge(&e3);
        g.add_edge(&e4);
        g.add_edge(&e5);
        e5.set_style(style_for(&two_strand));
        g.render_knot();

        const Graph_Validation_Result r = validate_graph(g);
        assert(!r.valid);
        assert(r.reason.contains("simple edges"));
    }

    {
        // Valid: triangle with one non-simple edge breaks long simple sequence rule
        Node a(QPointF(0, 0));
        Node b(QPointF(100, 0));
        Node c(QPointF(50, 80));
        Edge e1(&a, &b, &normal);
        Edge e2(&b, &c, &normal);
        Edge e3(&c, &a, &normal);
        Graph g;
        g.add_node(&a);
        g.add_node(&b);
        g.add_node(&c);
        g.add_edge(&e1);
        g.add_edge(&e2);
        g.add_edge(&e3);
        e3.set_style(style_for(&two_strand));
        g.render_knot();

        const Graph_Validation_Result r = validate_graph(g);
        assert(r.valid);
        assert(r.reason.isEmpty());
    }

    return 0;
}
