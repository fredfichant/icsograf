#include "edge_2strand.hpp"
#include <qmath.h>
#include <QDebug>
#include "edge.hpp"
#include "edge_handle_calculator.hpp"
#include "edge_inverted_logic.hpp"
#include "edge_type_utils.hpp"
#include "point_math.hpp"

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
static Edge::Handle traverse_2strand_impl(Edge* edge, Edge::Handle hand, Path_Builder& path,
                                          bool inverted)
{
    Edge_Handle strand_bit = (Edge_Handle) (hand & Edge_Handle_Namespace::STRAND_MASK);
    Edge_Handle pure_hand = (Edge_Handle) (hand & Edge_Handle_Namespace::HANDLE_MASK);
    int strand = (int) (strand_bit >> 12);

    Edge_Handle next_pure = Edge_Handle_Namespace::NO_HANDLE;
    int next_strand = strand;

    if (strand == 0) {
        if (pure_hand == Edge_Handle_Namespace::TOP_LEFT) {
            next_pure = Edge_Handle_Namespace::CENTER_TOP_RIGHT;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT) {
            next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT;
            next_strand = inverted ? 1 : 0;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT) {
            next_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
        } else if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT) {
            next_pure = Edge_Handle_Namespace::CENTER_TOP_LEFT;
             next_strand = inverted ? 1 : 0;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT) {
            next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_LEFT;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) {
            next_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
        }
    } else {
        if (pure_hand == Edge_Handle_Namespace::TOP_LEFT) {
            next_pure = Edge_Handle_Namespace::CENTER_TOP_RIGHT;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT) {
            next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT;
            next_strand = inverted ? 0 : 1;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT) {
            next_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
        } else if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT) {
            next_pure = Edge_Handle_Namespace::CENTER_TOP_LEFT;
             next_strand = inverted ? 0 : 1;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT) {
            next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_LEFT; 
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) {
            next_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
        }
    }

    if (next_pure != Edge_Handle_Namespace::NO_HANDLE) {
        Edge_Handle next_strand_bit =
            (next_strand == strand) ? strand_bit : (Edge_Handle) (next_strand << 12);
        Edge_Handle next = (Edge_Handle) (next_pure | next_strand_bit);

        bool is_gap = (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT &&
                       next_pure == Edge_Handle_Namespace::BOTTOM_LEFT) ||
                      (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT &&
                       next_pure == Edge_Handle_Namespace::TOP_RIGHT);

        bool is_under = (inverted) ? (strand == 0) : (strand == 1);

        if (!(is_under && is_gap)) {
            path.add_line(get_handle_pos(edge, hand), get_handle_pos(edge, next));
        }

        return (Edge::Handle) next;
    }

    return Edge_Handle_Namespace::NO_HANDLE;
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
