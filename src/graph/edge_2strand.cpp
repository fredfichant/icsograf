/**
 * \file src/graph/edge_2strand.cpp
 * \brief Implementation of edge 2-strand graph logic.
 */

#include "edge_2strand.hpp"

#include "edge.hpp"
#include "edge_handle_calculator.hpp"
#include "edge_inverted_logic.hpp"
#include "edge_type_utils.hpp"

/**
 * \brief Implements the traversal logic for a 2-strand edge.
 *
 * \details This function determines the path of a strand through a 2-strand crossing.
 * It handles the routing from entry handles (corners) to internal control points
 * (center handles) and finally to exit handles. It also handles the visual "gap"
 * logic to represent over/under crossings by selectively adding lines to the
 * Path_Builder.
 *
 * The logic defines the internal routing:
 * - TL -> CTR -> CBR -> BL
 * - TR -> CTL -> CBL -> BR
 *
 * \param edge Pointer to the edge being traversed.
 * \param hand The current handle (position and strand) being processed.
 * \param path Reference to the Path_Builder where the geometry is added.
 * \param inverted Boolean flag indicating if the crossing is inverted.
 *                 If true, strand 0 is considered "under" and strands may swap
 *                 indices in the central segment. If false, strand 1 is "under".
 * \return The next handle in the traversal sequence, or Edge_Handle_Namespace::NO_HANDLE
 *         if the end of the edge is reached.
 */
static bool is_connector_segment(Edge_Handle a, Edge_Handle b)
{
    return (a == Edge_Handle_Namespace::MID_TOP_LEFT &&
            b == Edge_Handle_Namespace::MID_BOTTOM_LEFT) ||
           (a == Edge_Handle_Namespace::MID_BOTTOM_LEFT &&
            b == Edge_Handle_Namespace::MID_TOP_LEFT) ||
           (a == Edge_Handle_Namespace::MID_TOP_RIGHT &&
            b == Edge_Handle_Namespace::MID_BOTTOM_RIGHT) ||
           (a == Edge_Handle_Namespace::MID_BOTTOM_RIGHT &&
            b == Edge_Handle_Namespace::MID_TOP_RIGHT);
}

static bool is_top_square_over(Edge_Handle a, Edge_Handle b, bool inverted)
{
    if (!inverted)
        return (a == Edge_Handle_Namespace::TOP_LEFT &&
                b == Edge_Handle_Namespace::MID_TOP_RIGHT) ||
               (a == Edge_Handle_Namespace::MID_TOP_RIGHT &&
                b == Edge_Handle_Namespace::TOP_LEFT);
    return (a == Edge_Handle_Namespace::TOP_RIGHT && b == Edge_Handle_Namespace::MID_TOP_LEFT) ||
           (a == Edge_Handle_Namespace::MID_TOP_LEFT && b == Edge_Handle_Namespace::TOP_RIGHT);
}

static bool is_bottom_square_over(Edge_Handle a, Edge_Handle b, bool inverted)
{
    if (!inverted) {
        return (a == Edge_Handle_Namespace::MID_BOTTOM_LEFT &&
                b == Edge_Handle_Namespace::BOTTOM_RIGHT) ||
               (a == Edge_Handle_Namespace::BOTTOM_RIGHT &&
                b == Edge_Handle_Namespace::MID_BOTTOM_LEFT);
    }
    return (a == Edge_Handle_Namespace::MID_BOTTOM_RIGHT &&
            b == Edge_Handle_Namespace::BOTTOM_LEFT) ||
           (a == Edge_Handle_Namespace::BOTTOM_LEFT &&
            b == Edge_Handle_Namespace::MID_BOTTOM_RIGHT);
}

static Edge::Handle traverse_2strand_impl(Edge* edge, Edge::Handle hand, Path_Builder& path,
                                          bool inverted)
{
    Edge_Handle strand_bit = (Edge_Handle) (hand & Edge_Handle_Namespace::STRAND_MASK);
    Edge_Handle pure_hand = (Edge_Handle) (hand & Edge_Handle_Namespace::HANDLE_MASK);
    Edge_Handle next_pure = Edge_Handle_Namespace::NO_HANDLE;
    const auto h_traversed = [&](Edge_Handle pure) {
        return edge->traversed((Edge_Handle) (pure | strand_bit));
    };
    const auto ext_traversed = [&](Edge_Handle pure_external) {
        return edge->traversed((Edge_Handle) (pure_external | strand_bit));
    };

    switch (pure_hand) {
        case Edge_Handle_Namespace::TOP_LEFT:
            next_pure = Edge_Handle_Namespace::MID_TOP_RIGHT;
            break;
        case Edge_Handle_Namespace::MID_TOP_RIGHT:
            if (!ext_traversed(Edge_Handle_Namespace::TOP_LEFT))
                next_pure = Edge_Handle_Namespace::TOP_LEFT;
            else if (!h_traversed(Edge_Handle_Namespace::MID_BOTTOM_RIGHT))
                next_pure = Edge_Handle_Namespace::MID_BOTTOM_RIGHT;
            else
                next_pure = Edge_Handle_Namespace::TOP_LEFT;
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_RIGHT:
            if (!ext_traversed(Edge_Handle_Namespace::BOTTOM_LEFT))
                next_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
            else if (!h_traversed(Edge_Handle_Namespace::MID_TOP_RIGHT))
                next_pure = Edge_Handle_Namespace::MID_TOP_RIGHT;
            else
                next_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
            break;
        case Edge_Handle_Namespace::BOTTOM_LEFT:
            next_pure = Edge_Handle_Namespace::MID_BOTTOM_RIGHT;
            break;
        case Edge_Handle_Namespace::TOP_RIGHT:
            next_pure = Edge_Handle_Namespace::MID_TOP_LEFT;
            break;
        case Edge_Handle_Namespace::MID_TOP_LEFT:
            if (!ext_traversed(Edge_Handle_Namespace::TOP_RIGHT))
                next_pure = Edge_Handle_Namespace::TOP_RIGHT;
            else if (!h_traversed(Edge_Handle_Namespace::MID_BOTTOM_LEFT))
                next_pure = Edge_Handle_Namespace::MID_BOTTOM_LEFT;
            else
                next_pure = Edge_Handle_Namespace::TOP_RIGHT;
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_LEFT:
            if (!ext_traversed(Edge_Handle_Namespace::BOTTOM_RIGHT))
                next_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
            else if (!h_traversed(Edge_Handle_Namespace::MID_TOP_LEFT))
                next_pure = Edge_Handle_Namespace::MID_TOP_LEFT;
            else
                next_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
            break;
        case Edge_Handle_Namespace::BOTTOM_RIGHT:
            next_pure = Edge_Handle_Namespace::MID_BOTTOM_LEFT;
            break;
        default:
            break;
    }

    if (next_pure == Edge_Handle_Namespace::NO_HANDLE) return Edge_Handle_Namespace::NO_HANDLE;

    Edge_Handle next = (Edge_Handle) (next_pure | strand_bit);
    if (is_connector_segment(pure_hand, next_pure) || is_top_square_over(pure_hand, next_pure, inverted) ||
        is_bottom_square_over(pure_hand, next_pure, inverted)) {
        path.add_line(get_handle_pos(edge, hand), get_handle_pos(edge, next));
    }

    return (Edge::Handle) next;
}

/**
 * \brief Calculates the control handle geometry for a specific position on the 2-strand edge.
 *
 * Uses the standard diagonal handle calculation logic.
 */
QLineF Edge_2Strand::handle(const Edge* edge, Edge::Handle handle) const
{
    return Standard_Edge_Handle_Calculator::standard_handle(edge, handle);
}

Edge::Handle Edge_2Strand::traverse(Edge* edge, Edge::Handle hand, Path_Builder& path) const
{
    return traverse_2strand_impl(edge, hand, path, false);
}

QString Edge_2Strand::name() const { return QObject::tr("2-Strand Crossing"); }
QString Edge_2Strand::machine_name() const { return "2strand"; }

Edge::Handle Edge_2Strand_Inverted::traverse(Edge* edge, Edge::Handle hand,
                                             Path_Builder& path) const
{
    return traverse_2strand_impl(edge, hand, path, true);
}

QString Edge_2Strand_Inverted::name() const { return QObject::tr("2-Strand Inverted"); }
QString Edge_2Strand_Inverted::machine_name() const { return "2strand_inverted"; }

void Edge_2Strand_Inverted::paint(QPainter* painter, const Edge& edge)
{
    Edge_Inverted_Logic::paint_inverted(painter, edge);
}
