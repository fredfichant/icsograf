#include "edge_3strand.hpp"

#include <qmath.h>

#include <QDebug>
#include <QVector>

#include "edge.hpp"
#include "edge_handle_calculator.hpp"
#include "edge_inverted_logic.hpp"
#include "edge_type_utils.hpp"
#include "point_math.hpp"

QLineF Edge_3Strand::handle(const Edge* edge, Edge::Handle handle) const
{
    return Standard_Edge_Handle_Calculator::standard_handle(edge, handle);
}

Edge::Handle Edge_3Strand::traverse(Edge* edge, Edge::Handle hand, Path_Builder& path) const
{
    Edge_Handle strand_bit = (Edge_Handle) (hand & Edge_Handle_Namespace::STRAND_MASK);
    Edge_Handle pure_hand = (Edge_Handle) (hand & Edge_Handle_Namespace::HANDLE_MASK);
    int strand = (int) ((hand & Edge_Handle_Namespace::STRAND_MASK) >> 12);

    struct Route
    {
        Edge_Handle from;
        Edge_Handle to;
        int strand_change;
    };

    QVector<Route> routes;
    routes.append({Edge_Handle_Namespace::TOP_LEFT, Edge_Handle_Namespace::MID_TOP_RIGHT, -1});
    routes.append(
        {Edge_Handle_Namespace::MID_TOP_RIGHT, Edge_Handle_Namespace::CENTER_TOP_RIGHT, -1});
    routes.append(
        {Edge_Handle_Namespace::CENTER_TOP_RIGHT, Edge_Handle_Namespace::CENTER_BOTTOM_LEFT, 1});
    routes.append(
        {Edge_Handle_Namespace::CENTER_BOTTOM_LEFT, Edge_Handle_Namespace::MID_BOTTOM_LEFT, -1});
    routes.append(
        {Edge_Handle_Namespace::MID_BOTTOM_LEFT, Edge_Handle_Namespace::BOTTOM_RIGHT, -1});

    routes.append({Edge_Handle_Namespace::TOP_RIGHT, Edge_Handle_Namespace::MID_TOP_LEFT, -1});
    routes.append(
        {Edge_Handle_Namespace::MID_TOP_LEFT, Edge_Handle_Namespace::CENTER_TOP_LEFT, -1});
    routes.append(
        {Edge_Handle_Namespace::CENTER_TOP_LEFT, Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT, 1});
    routes.append(
        {Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT, Edge_Handle_Namespace::MID_BOTTOM_RIGHT, -1});
    routes.append(
        {Edge_Handle_Namespace::MID_BOTTOM_RIGHT, Edge_Handle_Namespace::BOTTOM_LEFT, -1});

    for (const Route& r : routes) {
        if (r.from == pure_hand) {
            int next_strand = strand;
            if (r.strand_change != -1) {
                if (strand == 0)
                    next_strand = r.strand_change;
                else if (strand == 1)
                    next_strand = 1;
                else if (strand == 2)
                    next_strand = 2 - r.strand_change;
            }

            Edge_Handle next_strand_bit =
                (next_strand == strand) ? strand_bit : (Edge_Handle) (next_strand << 12);
            Edge_Handle next = (Edge_Handle) (r.to | next_strand_bit);

            bool is_gap = (pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT &&
                           r.to == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) ||
                          (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT &&
                           r.to == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT);

            bool is_under = (strand == 1);

            if (!(is_under && is_gap)) {
                path.add_line(get_handle_pos(edge, hand), get_handle_pos(edge, next));
            }

            return (Edge::Handle) next;
        }
    }

    return Edge_Handle_Namespace::NO_HANDLE;
}

QString Edge_3Strand::name() const { return QObject::tr("3-Strand Crossing"); }
QString Edge_3Strand::machine_name() const { return "3strand"; }

Edge::Handle Edge_3Strand_Inverted::traverse(Edge* edge, Edge::Handle hand,
                                             Path_Builder& path) const
{
    Edge_Handle strand_bit = (Edge_Handle) (hand & Edge_Handle_Namespace::STRAND_MASK);
    Edge_Handle pure_hand = (Edge_Handle) (hand & Edge_Handle_Namespace::HANDLE_MASK);
    Edge_Handle next = Edge_Handle_Namespace::NO_HANDLE;

    if (pure_hand == Edge_Handle_Namespace::TOP_LEFT)
        next = Edge_Handle_Namespace::MID_TOP_RIGHT;
    else if (pure_hand == Edge_Handle_Namespace::MID_TOP_RIGHT)
        next = Edge_Handle_Namespace::CENTER_TOP_RIGHT;
    else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT)
        next = Edge_Handle_Namespace::CENTER_BOTTOM_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT)
        next = Edge_Handle_Namespace::MID_BOTTOM_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::MID_BOTTOM_LEFT)
        next = Edge_Handle_Namespace::BOTTOM_RIGHT;

    else if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT)
        next = Edge_Handle_Namespace::MID_TOP_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::MID_TOP_LEFT)
        next = Edge_Handle_Namespace::CENTER_TOP_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT)
        next = Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT;
    else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT)
        next = Edge_Handle_Namespace::MID_BOTTOM_RIGHT;
    else if (pure_hand == Edge_Handle_Namespace::MID_BOTTOM_RIGHT)
        next = Edge_Handle_Namespace::BOTTOM_LEFT;

    if (next != Edge_Handle_Namespace::NO_HANDLE) {
        bool is_over_path = (pure_hand == Edge_Handle_Namespace::TOP_RIGHT ||
                             pure_hand == Edge_Handle_Namespace::MID_TOP_LEFT ||
                             pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT ||
                             pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT ||
                             pure_hand == Edge_Handle_Namespace::MID_BOTTOM_RIGHT ||
                             pure_hand == Edge_Handle_Namespace::BOTTOM_LEFT ||
                             next == Edge_Handle_Namespace::TOP_RIGHT ||
                             next == Edge_Handle_Namespace::MID_TOP_LEFT ||
                             next == Edge_Handle_Namespace::CENTER_TOP_LEFT ||
                             next == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT ||
                             next == Edge_Handle_Namespace::MID_BOTTOM_RIGHT ||
                             next == Edge_Handle_Namespace::BOTTOM_LEFT);

        bool is_gap = ((pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT &&
                        next == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) ||
                       (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT &&
                        next == Edge_Handle_Namespace::CENTER_TOP_RIGHT));

        if (is_over_path || !is_gap)
            path.add_line(get_handle_pos(edge, hand),
                          get_handle_pos(edge, (Edge::Handle) (next | strand_bit)));
    }

    return (Edge::Handle) (next | strand_bit);
}

QString Edge_3Strand_Inverted::name() const { return QObject::tr("3-Strand Inverted"); }
QString Edge_3Strand_Inverted::machine_name() const { return "3strand_inverted"; }

void Edge_3Strand_Inverted::paint(QPainter* painter, const Edge& edge)
{
    Edge_Inverted_Logic::paint_inverted(painter, edge);
}
