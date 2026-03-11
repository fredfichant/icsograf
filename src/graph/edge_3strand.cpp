/**
 * \file src/graph/edge_3strand.cpp
 * \brief Implementation of edge 3-strand graph logic.
 */

#include "edge_3strand.hpp"

#include "edge.hpp"
#include "edge_handle_calculator.hpp"
#include "edge_inverted_logic.hpp"
#include "edge_type_utils.hpp"

QLineF Edge_3Strand::handle(const Edge* edge, Edge::Handle handle) const
{
    return Standard_Edge_Handle_Calculator::standard_handle(edge, handle);
}

static Edge::Handle traverse_3strand_impl(Edge* edge, Edge::Handle hand, Path_Builder& path,
                                          bool inverted)
{
    Edge_Handle strand_bit = (Edge_Handle) (hand & Edge_Handle_Namespace::STRAND_MASK);
    Edge_Handle pure_hand = (Edge_Handle) (hand & Edge_Handle_Namespace::HANDLE_MASK);
    Edge_Handle next_pure = Edge_Handle_Namespace::NO_HANDLE;
    const auto h_traversed = [&](Edge_Handle pure) {
        return edge->traversed((Edge_Handle) (pure | strand_bit));
    };

    switch (pure_hand) {
        case Edge_Handle_Namespace::TOP_LEFT:
            next_pure = Edge_Handle_Namespace::MID_TOP_RIGHT;
            break;
        case Edge_Handle_Namespace::MID_TOP_RIGHT:
            if (!h_traversed(Edge_Handle_Namespace::TOP_LEFT))
                next_pure = Edge_Handle_Namespace::TOP_LEFT;
            else if (!h_traversed(Edge_Handle_Namespace::CENTER_TOP_RIGHT))
                next_pure = Edge_Handle_Namespace::CENTER_TOP_RIGHT;
            else
                next_pure = Edge_Handle_Namespace::TOP_LEFT;
            break;
        case Edge_Handle_Namespace::CENTER_TOP_RIGHT:
            if (!h_traversed(Edge_Handle_Namespace::CENTER_BOTTOM_LEFT))
                next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_LEFT;
            else if (!h_traversed(Edge_Handle_Namespace::MID_TOP_RIGHT))
                next_pure = Edge_Handle_Namespace::MID_TOP_RIGHT;
            else
                next_pure = Edge_Handle_Namespace::MID_TOP_RIGHT;
            break;
        case Edge_Handle_Namespace::CENTER_BOTTOM_LEFT:
            if (!h_traversed(Edge_Handle_Namespace::MID_BOTTOM_LEFT))
                next_pure = Edge_Handle_Namespace::MID_BOTTOM_LEFT;
            else if (!h_traversed(Edge_Handle_Namespace::CENTER_TOP_RIGHT))
                next_pure = Edge_Handle_Namespace::CENTER_TOP_RIGHT;
            else
                next_pure = Edge_Handle_Namespace::MID_BOTTOM_LEFT;
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_LEFT:
            if (!h_traversed(Edge_Handle_Namespace::BOTTOM_RIGHT))
                next_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
            else if (!h_traversed(Edge_Handle_Namespace::CENTER_BOTTOM_LEFT))
                next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_LEFT;
            else
                next_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
            break;
        case Edge_Handle_Namespace::BOTTOM_RIGHT:
            next_pure = Edge_Handle_Namespace::MID_BOTTOM_LEFT;
            break;

        case Edge_Handle_Namespace::TOP_RIGHT:
            next_pure = Edge_Handle_Namespace::MID_TOP_LEFT;
            break;
        case Edge_Handle_Namespace::MID_TOP_LEFT:
            if (!h_traversed(Edge_Handle_Namespace::TOP_RIGHT))
                next_pure = Edge_Handle_Namespace::TOP_RIGHT;
            else if (!h_traversed(Edge_Handle_Namespace::CENTER_TOP_LEFT))
                next_pure = Edge_Handle_Namespace::CENTER_TOP_LEFT;
            else
                next_pure = Edge_Handle_Namespace::TOP_RIGHT;
            break;
        case Edge_Handle_Namespace::CENTER_TOP_LEFT:
            if (!h_traversed(Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT))
                next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT;
            else if (!h_traversed(Edge_Handle_Namespace::MID_TOP_LEFT))
                next_pure = Edge_Handle_Namespace::MID_TOP_LEFT;
            else
                next_pure = Edge_Handle_Namespace::MID_TOP_LEFT;
            break;
        case Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT:
            if (!h_traversed(Edge_Handle_Namespace::MID_BOTTOM_RIGHT))
                next_pure = Edge_Handle_Namespace::MID_BOTTOM_RIGHT;
            else if (!h_traversed(Edge_Handle_Namespace::CENTER_TOP_LEFT))
                next_pure = Edge_Handle_Namespace::CENTER_TOP_LEFT;
            else
                next_pure = Edge_Handle_Namespace::MID_BOTTOM_RIGHT;
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_RIGHT:
            if (!h_traversed(Edge_Handle_Namespace::BOTTOM_LEFT))
                next_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
            else if (!h_traversed(Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT))
                next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT;
            else
                next_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
            break;
        case Edge_Handle_Namespace::BOTTOM_LEFT:
            next_pure = Edge_Handle_Namespace::MID_BOTTOM_RIGHT;
            break;
        default:
            break;
    }

    if (next_pure == Edge_Handle_Namespace::NO_HANDLE) return Edge_Handle_Namespace::NO_HANDLE;

    const Edge_Handle next = (Edge_Handle) (next_pure | strand_bit);

    const bool is_connector =
        (pure_hand == Edge_Handle_Namespace::MID_TOP_LEFT &&
         next_pure == Edge_Handle_Namespace::CENTER_TOP_LEFT) ||
        (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT &&
         next_pure == Edge_Handle_Namespace::MID_TOP_LEFT) ||
        (pure_hand == Edge_Handle_Namespace::MID_TOP_RIGHT &&
         next_pure == Edge_Handle_Namespace::CENTER_TOP_RIGHT) ||
        (pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT &&
         next_pure == Edge_Handle_Namespace::MID_TOP_RIGHT) ||
        (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT &&
         next_pure == Edge_Handle_Namespace::MID_BOTTOM_LEFT) ||
        (pure_hand == Edge_Handle_Namespace::MID_BOTTOM_LEFT &&
         next_pure == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) ||
        (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT &&
         next_pure == Edge_Handle_Namespace::MID_BOTTOM_RIGHT) ||
        (pure_hand == Edge_Handle_Namespace::MID_BOTTOM_RIGHT &&
         next_pure == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT);

    const bool top_over =
        !inverted
            ? ((pure_hand == Edge_Handle_Namespace::TOP_LEFT &&
                next_pure == Edge_Handle_Namespace::MID_TOP_RIGHT) ||
               (pure_hand == Edge_Handle_Namespace::MID_TOP_RIGHT &&
                next_pure == Edge_Handle_Namespace::TOP_LEFT))
            : ((pure_hand == Edge_Handle_Namespace::TOP_RIGHT &&
                next_pure == Edge_Handle_Namespace::MID_TOP_LEFT) ||
               (pure_hand == Edge_Handle_Namespace::MID_TOP_LEFT &&
                next_pure == Edge_Handle_Namespace::TOP_RIGHT));

    const bool center_over =
        !inverted ? ((pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT &&
                      next_pure == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT) ||
                     (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT &&
                      next_pure == Edge_Handle_Namespace::CENTER_TOP_LEFT))
                  : ((pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT &&
                      next_pure == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) ||
                     (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT &&
                      next_pure == Edge_Handle_Namespace::CENTER_TOP_RIGHT));

    const bool bottom_over =
        !inverted
            ? ((pure_hand == Edge_Handle_Namespace::MID_BOTTOM_LEFT &&
                next_pure == Edge_Handle_Namespace::BOTTOM_RIGHT) ||
               (pure_hand == Edge_Handle_Namespace::BOTTOM_RIGHT &&
                next_pure == Edge_Handle_Namespace::MID_BOTTOM_LEFT))
            : ((pure_hand == Edge_Handle_Namespace::MID_BOTTOM_RIGHT &&
                next_pure == Edge_Handle_Namespace::BOTTOM_LEFT) ||
               (pure_hand == Edge_Handle_Namespace::BOTTOM_LEFT &&
                next_pure == Edge_Handle_Namespace::MID_BOTTOM_RIGHT));

    if (is_connector || top_over || center_over || bottom_over)
        path.add_line(get_handle_pos(edge, hand), get_handle_pos(edge, next));

    return (Edge::Handle) next;
}

Edge::Handle Edge_3Strand::traverse(Edge* edge, Edge::Handle hand, Path_Builder& path) const
{
    return traverse_3strand_impl(edge, hand, path, false);
}

QString Edge_3Strand::name() const { return QObject::tr("3-Strand Crossing"); }
QString Edge_3Strand::machine_name() const { return "3strand"; }

Edge::Handle Edge_3Strand_Inverted::traverse(Edge* edge, Edge::Handle hand,
                                             Path_Builder& path) const
{
    return traverse_3strand_impl(edge, hand, path, true);
}

QString Edge_3Strand_Inverted::name() const { return QObject::tr("3-Strand Inverted"); }
QString Edge_3Strand_Inverted::machine_name() const { return "3strand_inverted"; }

void Edge_3Strand_Inverted::paint(QPainter* painter, const Edge& edge)
{
    Edge_Inverted_Logic::paint_inverted(painter, edge);
}
