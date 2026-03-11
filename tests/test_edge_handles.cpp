/**
 * \file tests/test_edge_handles.cpp
 * \brief Unit tests for edge handles.
 */

#include <cassert>
#include <cmath>

#include "edge_2strand.hpp"
#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "edge_type_utils.hpp"
#include "graph.hpp"

static bool near(double a, double b, double eps = 1e-6) { return std::abs(a - b) <= eps; }

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
    }

    void set_style(Edge_Type* type, double crossing_distance, double spacing)
    {
        e.set_style(Edge_Style(24, crossing_distance, 0.5, type, Edge_Style::EVERYTHING, spacing, 1));
    }
};

int main()
{
    Edge_Normal normal;
    Edge_2Strand two;
    Edge_3Strand three;

    {
        Edge_Fixture fx(&normal);
        fx.set_style(&normal, 20.0, 10.0);

        QPointF tl = get_handle_pos(&fx.e, Edge_Handle_Namespace::TOP_LEFT);
        QPointF tr = get_handle_pos(&fx.e, Edge_Handle_Namespace::TOP_RIGHT);
        QPointF bl = get_handle_pos(&fx.e, Edge_Handle_Namespace::BOTTOM_LEFT);
        QPointF br = get_handle_pos(&fx.e, Edge_Handle_Namespace::BOTTOM_RIGHT);

        assert(near(tl.x(), 40.0) && near(tl.y(), -10.0));
        assert(near(tr.x(), 60.0) && near(tr.y(), -10.0));
        assert(near(bl.x(), 40.0) && near(bl.y(), 10.0));
        assert(near(br.x(), 60.0) && near(br.y(), 10.0));
    }

    {
        Edge_Fixture fx(&two);
        fx.set_style(&two, 10.0, 12.0);

        QPointF top = get_handle_pos(&fx.e, Edge_Handle_Namespace::TOP_LEFT);
        QPointF mid_top = get_handle_pos(&fx.e, Edge_Handle_Namespace::MID_TOP_LEFT);
        QPointF mid_bottom = get_handle_pos(&fx.e, Edge_Handle_Namespace::MID_BOTTOM_LEFT);
        QPointF bottom = get_handle_pos(&fx.e, Edge_Handle_Namespace::BOTTOM_LEFT);

        assert(top.y() < mid_top.y());
        assert(mid_top.y() < mid_bottom.y());
        assert(mid_bottom.y() < bottom.y());
        assert(near(std::abs(top.y() - mid_top.y()), 5.0));
        assert(near(std::abs(mid_bottom.y() - bottom.y()), 5.0));
    }

    {
        Edge_Fixture fx(&three);
        fx.set_style(&three, 10.0, 12.0);

        QPointF top = get_handle_pos(&fx.e, Edge_Handle_Namespace::TOP_LEFT);
        QPointF mid_top = get_handle_pos(&fx.e, Edge_Handle_Namespace::MID_TOP_LEFT);
        QPointF center_top = get_handle_pos(&fx.e, Edge_Handle_Namespace::CENTER_TOP_LEFT);
        QPointF center_bottom = get_handle_pos(&fx.e, Edge_Handle_Namespace::CENTER_BOTTOM_LEFT);
        QPointF mid_bottom = get_handle_pos(&fx.e, Edge_Handle_Namespace::MID_BOTTOM_LEFT);
        QPointF bottom = get_handle_pos(&fx.e, Edge_Handle_Namespace::BOTTOM_LEFT);

        assert(top.y() < mid_top.y());
        assert(mid_top.y() < center_top.y());
        assert(center_top.y() < center_bottom.y());
        assert(center_bottom.y() < mid_bottom.y());
        assert(mid_bottom.y() < bottom.y());
        assert(near(std::abs(mid_top.y() - center_top.y()), 12.0));
        assert(near(std::abs(center_bottom.y() - mid_bottom.y()), 12.0));
    }

    return 0;
}
