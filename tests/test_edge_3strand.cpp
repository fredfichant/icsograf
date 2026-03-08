#include <cassert>

#include "edge_3strand.hpp"
#include "graph.hpp"

struct Edge_Fixture
{
    Node n1;
    Node n2;
    Graph g;
    Edge e;

    explicit Edge_Fixture(Edge_Type* type)
        : n1(QPointF(0, 0)), n2(QPointF(100, 0)), e(&n1, &n2, type)
    {
        g.add_node(&n1);
        g.add_node(&n2);
        g.add_edge(&e);
        e.set_style(Edge_Style(24, 10, 0.5, type, Edge_Style::EVERYTHING, 10, 1));
        e.reset();
    }
};

static bool has_segments(Path_Builder& path)
{
    QList<QPainterPath> paths = path.build();
    return !paths.isEmpty();
}

int main()
{
    Edge_3Strand normal;
    Edge_3Strand_Inverted inverted;

    {
        Edge_Fixture fx(&normal);

        Path_Builder p1;
        Edge_Handle next = normal.traverse(&fx.e, Edge_Handle_Namespace::TOP_LEFT, p1);
        assert((next & Edge_Handle_Namespace::HANDLE_MASK) == Edge_Handle_Namespace::MID_TOP_RIGHT);
        assert(has_segments(p1));

        Path_Builder p2;
        fx.e.mark_traversed(Edge_Handle_Namespace::TOP_LEFT);
        fx.e.mark_traversed(Edge_Handle_Namespace::MID_TOP_RIGHT);
        Edge_Handle next2 = normal.traverse(&fx.e, Edge_Handle_Namespace::MID_TOP_RIGHT, p2);
        assert((next2 & Edge_Handle_Namespace::HANDLE_MASK) ==
               Edge_Handle_Namespace::CENTER_TOP_RIGHT);
        assert(has_segments(p2));

        Path_Builder p3;
        Edge_Handle in = (Edge_Handle) (Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT |
                                         Edge_Handle_Namespace::STRAND_1);
        Edge_Handle out = normal.traverse(&fx.e, in, p3);
        assert((out & Edge_Handle_Namespace::HANDLE_MASK) ==
               Edge_Handle_Namespace::MID_BOTTOM_RIGHT);
        assert((out & Edge_Handle_Namespace::STRAND_MASK) == Edge_Handle_Namespace::STRAND_1);
        assert(has_segments(p3));

        Path_Builder p4;
        normal.traverse(&fx.e, Edge_Handle_Namespace::TOP_RIGHT, p4);
        assert(!has_segments(p4));

        Path_Builder p5;
        Edge_Handle n5 = normal.traverse(&fx.e, Edge_Handle_Namespace::BOTTOM_LEFT, p5);
        assert((n5 & Edge_Handle_Namespace::HANDLE_MASK) == Edge_Handle_Namespace::MID_BOTTOM_RIGHT);
        assert(!has_segments(p5));

        Path_Builder p6;
        Edge_Handle n6 = normal.traverse(&fx.e, Edge_Handle_Namespace::MID_BOTTOM_RIGHT, p6);
        assert((n6 & Edge_Handle_Namespace::HANDLE_MASK) == Edge_Handle_Namespace::BOTTOM_LEFT);
        assert(!has_segments(p6));

        Path_Builder p7;
        fx.e.mark_traversed(Edge_Handle_Namespace::MID_TOP_LEFT);
        Edge_Handle n7 = normal.traverse(&fx.e, Edge_Handle_Namespace::CENTER_TOP_LEFT, p7);
        assert((n7 & Edge_Handle_Namespace::HANDLE_MASK) ==
               Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT);
        assert(has_segments(p7));
    }

    {
        Edge_Fixture fx(&inverted);

        Path_Builder p1;
        inverted.traverse(&fx.e, Edge_Handle_Namespace::TOP_LEFT, p1);
        assert(!has_segments(p1));

        Path_Builder p2;
        inverted.traverse(&fx.e, Edge_Handle_Namespace::TOP_RIGHT, p2);
        assert(has_segments(p2));

        Path_Builder p3;
        inverted.traverse(&fx.e, Edge_Handle_Namespace::CENTER_BOTTOM_LEFT, p3);
        assert(has_segments(p3));

        Path_Builder p4;
        inverted.traverse(&fx.e, Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT, p4);
        assert(has_segments(p4));
    }

    return 0;
}
