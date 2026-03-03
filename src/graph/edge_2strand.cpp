#include "edge_2strand.hpp"

#include <qmath.h>

#include <QDebug>

#include "edge.hpp"
#include "edge_type_utils.hpp"
#include "point_math.hpp"

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
            next_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
        } else if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT) {
            next_pure = Edge_Handle_Namespace::CENTER_TOP_LEFT;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT) {
            next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_LEFT;
            next_strand = inverted ? 1 : 0;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) {
            next_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
        }
    } else {
        if (pure_hand == Edge_Handle_Namespace::TOP_LEFT) {
            next_pure = Edge_Handle_Namespace::CENTER_TOP_RIGHT;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT) {
            next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT;
            next_strand = inverted ? 0 : 1;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT) {
            next_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
        } else if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT) {
            next_pure = Edge_Handle_Namespace::CENTER_TOP_LEFT;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT) {
            next_pure = Edge_Handle_Namespace::CENTER_BOTTOM_LEFT;
            next_strand = inverted ? 0 : 1;
        } else if (pure_hand == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) {
            next_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
        }
    }

    if (next_pure != Edge_Handle_Namespace::NO_HANDLE) {
        Edge_Handle next_strand_bit =
            (next_strand == strand) ? strand_bit : (Edge_Handle) (next_strand << 12);
        Edge_Handle next = (Edge_Handle) (next_pure | next_strand_bit);

        bool is_gap = (pure_hand == Edge_Handle_Namespace::CENTER_TOP_RIGHT &&
                       next_pure == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT) ||
                      (pure_hand == Edge_Handle_Namespace::CENTER_TOP_LEFT &&
                       next_pure == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT);

        bool is_under = (inverted) ? (strand == 0) : (strand == 1);

        if (!(is_under && is_gap)) {
            path.add_line(get_handle_pos(edge, hand), get_handle_pos(edge, next));
        }

        return (Edge::Handle) next;
    }

    return Edge_Handle_Namespace::NO_HANDLE;
}

QLineF Edge_2Strand::handle(const Edge* edge, Edge::Handle handle) const
{
    QPointF p1 = get_handle_pos(edge, handle);
    Edge_Style style = edge->defaulted_style();
    long double handle_angle = 0;
    Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
    if (pure_handle & (Edge_Handle_Namespace::TOP_RIGHT | Edge_Handle_Namespace::MID_TOP_RIGHT |
                       Edge_Handle_Namespace::CENTER_TOP_RIGHT))
        handle_angle = pi() / 4.0;
    else if (pure_handle & (Edge_Handle_Namespace::TOP_LEFT | Edge_Handle_Namespace::MID_TOP_LEFT |
                            Edge_Handle_Namespace::CENTER_TOP_LEFT))
        handle_angle = pi() * 3.0 / 4.0;
    else if (pure_handle &
             (Edge_Handle_Namespace::BOTTOM_LEFT | Edge_Handle_Namespace::MID_BOTTOM_LEFT |
              Edge_Handle_Namespace::CENTER_BOTTOM_LEFT))
        handle_angle = pi() * 5.0 / 4.0;
    else if (pure_handle &
             (Edge_Handle_Namespace::BOTTOM_RIGHT | Edge_Handle_Namespace::MID_BOTTOM_RIGHT |
              Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT))
        handle_angle = pi() * 7.0 / 4.0;
    handle_angle += deg2rad(edge->to_line().angle());

    QPointF p2(p1.x() + style.handle_length * qCos(handle_angle),
               p1.y() - style.handle_length * qSin(handle_angle));
    return QLineF(p1, p2);
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
    QPen pen = painter->pen();
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    painter->drawLine(edge.to_line());
}
