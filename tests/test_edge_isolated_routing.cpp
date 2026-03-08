#include <cassert>

#include "edge_2strand.hpp"
#include "edge_3strand.hpp"
#include "graph.hpp"

struct Edge_Fixture
{
    Node n1;
    Node n2;
    Graph g;
    Edge e;

    explicit Edge_Fixture(Edge_Type* type)
        : n1(QPointF(0, 0)), n2(QPointF(200, 0)), e(&n1, &n2, type)
    {
        g.add_node(&n1);
        g.add_node(&n2);
        g.add_edge(&e);
        e.set_style(Edge_Style(24, 10, 0.5, type, Edge_Style::EVERYTHING, 10, 1));
        e.reset();
    }
};

int main()
{
    {
        Edge_2Strand type2;
        Edge_Fixture fx(&type2);

        Path_Builder p1;
        fx.e.mark_traversed(Edge_Handle_Namespace::TOP_LEFT);
        Edge_Handle h = type2.traverse(&fx.e, Edge_Handle_Namespace::TOP_LEFT, p1);
        assert((h & Edge_Handle_Namespace::HANDLE_MASK) == Edge_Handle_Namespace::MID_TOP_RIGHT);

        Path_Builder p2;
        fx.e.mark_traversed(h);
        h = type2.traverse(&fx.e, h, p2);
        assert((h & Edge_Handle_Namespace::HANDLE_MASK) ==
               Edge_Handle_Namespace::MID_BOTTOM_RIGHT);

        Path_Builder p3;
        fx.e.mark_traversed(h);
        h = type2.traverse(&fx.e, h, p3);
        assert((h & Edge_Handle_Namespace::HANDLE_MASK) == Edge_Handle_Namespace::BOTTOM_LEFT);

        Path_Builder p4;
        fx.e.mark_traversed(Edge_Handle_Namespace::BOTTOM_LEFT);
        Edge_Handle hb = type2.traverse(&fx.e, Edge_Handle_Namespace::BOTTOM_LEFT, p4);
        assert((hb & Edge_Handle_Namespace::HANDLE_MASK) ==
               Edge_Handle_Namespace::MID_BOTTOM_RIGHT);

        Path_Builder p5;
        fx.e.mark_traversed(hb);
        hb = type2.traverse(&fx.e, hb, p5);
        assert((hb & Edge_Handle_Namespace::HANDLE_MASK) == Edge_Handle_Namespace::BOTTOM_LEFT);
    }

    {
        Edge_3Strand type3;
        Edge_Fixture fx(&type3);

        Path_Builder p1;
        fx.e.mark_traversed(Edge_Handle_Namespace::TOP_LEFT);
        Edge_Handle h = type3.traverse(&fx.e, Edge_Handle_Namespace::TOP_LEFT, p1);
        assert((h & Edge_Handle_Namespace::HANDLE_MASK) == Edge_Handle_Namespace::MID_TOP_RIGHT);

        Path_Builder p2;
        fx.e.mark_traversed(h);
        h = type3.traverse(&fx.e, h, p2);
        assert((h & Edge_Handle_Namespace::HANDLE_MASK) ==
               Edge_Handle_Namespace::CENTER_TOP_RIGHT);

        Path_Builder p3;
        fx.e.mark_traversed(h);
        h = type3.traverse(&fx.e, h, p3);
        assert((h & Edge_Handle_Namespace::HANDLE_MASK) ==
               Edge_Handle_Namespace::CENTER_BOTTOM_LEFT);

        Path_Builder p4;
        fx.e.mark_traversed(h);
        h = type3.traverse(&fx.e, h, p4);
        assert((h & Edge_Handle_Namespace::HANDLE_MASK) == Edge_Handle_Namespace::MID_BOTTOM_LEFT);
    }

    return 0;
}
