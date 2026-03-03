#include "edge_normal.hpp"

#include <qmath.h>

#include "edge.hpp"
#include "edge_type_utils.hpp"
#include "point_math.hpp"

/**
 * \brief Calculates the geometry for a specific handle on the edge.
 *
 * Determines the position and control point angle based on the handle type (corner), spacing, and edge rotation.
 */
QLineF Edge_Normal::handle(const Edge* edge, Edge::Handle handle) const
{
    Edge_Style style = edge->defaulted_style();
    Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
    int strand = (int) ((handle & Edge_Handle_Namespace::STRAND_MASK) >> 12);

    long double handle_angle = 0;
    if (pure_handle == Edge_Handle_Namespace::TOP_RIGHT)
        handle_angle = pi() / 4.0;
    else if (pure_handle == Edge_Handle_Namespace::TOP_LEFT)
        handle_angle = pi() * 3.0 / 4.0;
    else if (pure_handle == Edge_Handle_Namespace::BOTTOM_LEFT)
        handle_angle = pi() * 5.0 / 4.0;
    else if (pure_handle == Edge_Handle_Namespace::BOTTOM_RIGHT)
        handle_angle = pi() * 7.0 / 4.0;

    const long double edge_angle = deg2rad(edge->to_line().angle());

    double sx = 0.0, sy = 0.0;

    if (pure_handle == Edge_Handle_Namespace::TOP_RIGHT ||
        pure_handle == Edge_Handle_Namespace::MID_TOP_RIGHT ||
        pure_handle == Edge_Handle_Namespace::CENTER_TOP_RIGHT) {
        sx = +1.0;
        sy = +1.0;
    } else if (pure_handle == Edge_Handle_Namespace::TOP_LEFT ||
               pure_handle == Edge_Handle_Namespace::MID_TOP_LEFT ||
               pure_handle == Edge_Handle_Namespace::CENTER_TOP_LEFT) {
        sx = -1.0;
        sy = +1.0;
    } else if (pure_handle == Edge_Handle_Namespace::BOTTOM_LEFT ||
               pure_handle == Edge_Handle_Namespace::MID_BOTTOM_LEFT ||
               pure_handle == Edge_Handle_Namespace::CENTER_BOTTOM_LEFT) {
        sx = -1.0;
        sy = -1.0;
    } else if (pure_handle == Edge_Handle_Namespace::BOTTOM_RIGHT ||
               pure_handle == Edge_Handle_Namespace::MID_BOTTOM_RIGHT ||
               pure_handle == Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT) {
        sx = +1.0;
        sy = -1.0;
    }

    double level = 0.0;
    if (pure_handle & 0x00F0)
        level = 1.0;  // Level 1
    else if (pure_handle & 0x0F00)
        level = 2.0;  // Level 2

    double step = style.spacing;
    if (style.strand_count == 2) step = style.spacing / 2.0;

    const double dist = (style.crossing_distance / 2.0) - (level * step);

    const double vx = sx * dist;
    const double vy = sy * dist;

    const double rx = vx * qCos(edge_angle) - vy * qSin(edge_angle);
    const double ry = vx * qSin(edge_angle) + vy * qCos(edge_angle);

    QPointF p1 = edge->to_line().pointAt(style.edge_slide);
    p1.setX(p1.x() + rx);
    p1.setY(p1.y() - ry);

    if (style.strand_count > 1) {
        double offset = 0;
        if (style.strand_count == 2)
            offset = (strand == 0) ? -style.spacing / 2.0 : style.spacing / 2.0;
        else if (style.strand_count == 3)
            offset = (strand == 0) ? -style.spacing : (strand == 1 ? 0 : style.spacing);

        long double perp_angle = edge_angle + pi() / 2.0;
        p1.setX(p1.x() + offset * qCos(perp_angle));
        p1.setY(p1.y() - offset * qSin(perp_angle));
    }

    QPointF p2;
    p2.setX(p1.x() + style.handle_length * qCos(handle_angle));
    p2.setY(p1.y() - style.handle_length * qSin(handle_angle));

    return QLineF(p1, p2);
}

/**
 * \brief Traverses the edge for a standard crossing.
 *
 * Connects TL <-> BR and TR <-> BL. Draws the TL <-> BR strand (Over).
 */
Edge::Handle Edge_Normal::traverse(Edge* edge, Edge::Handle hand, Path_Builder& path) const
{
    Edge_Handle strand_bit = (Edge_Handle) (hand & Edge_Handle_Namespace::STRAND_MASK);
    Edge_Handle pure_hand = (Edge_Handle) (hand & Edge_Handle_Namespace::HANDLE_MASK);
    Edge_Handle next = Edge_Handle_Namespace::NO_HANDLE;
    if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT)
        next = Edge_Handle_Namespace::BOTTOM_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::BOTTOM_RIGHT)
        next = Edge_Handle_Namespace::TOP_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::BOTTOM_LEFT)
        next = Edge_Handle_Namespace::TOP_RIGHT;
    else if (pure_hand == Edge_Handle_Namespace::TOP_LEFT)
        next = Edge_Handle_Namespace::BOTTOM_RIGHT;

    if (pure_hand == Edge_Handle_Namespace::TOP_LEFT || next == Edge_Handle_Namespace::TOP_LEFT)
        path.add_line(handle(edge, hand).p1(),
                      handle(edge, (Edge::Handle) (next | strand_bit)).p1());
    return (Edge::Handle) (next | strand_bit);
}
QString Edge_Normal::name() const { return QObject::tr("Regular"); }
QString Edge_Normal::machine_name() const { return "regular"; }

/**
 * \brief Traverses the edge for an inverted crossing.
 *
 * Connects TL <-> BR and TR <-> BL. Draws the TR <-> BL strand (Over).
 */
Edge::Handle Edge_Inverted::traverse(Edge* edge, Edge::Handle hand, Path_Builder& path) const
{
    Edge_Handle strand_bit = (Edge_Handle) (hand & Edge_Handle_Namespace::STRAND_MASK);
    Edge_Handle pure_hand = (Edge_Handle) (hand & Edge_Handle_Namespace::HANDLE_MASK);
    Edge_Handle next = Edge_Handle_Namespace::NO_HANDLE;
    if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT)
        next = Edge_Handle_Namespace::BOTTOM_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::BOTTOM_RIGHT)
        next = Edge_Handle_Namespace::TOP_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::BOTTOM_LEFT)
        next = Edge_Handle_Namespace::TOP_RIGHT;
    else if (pure_hand == Edge_Handle_Namespace::TOP_LEFT)
        next = Edge_Handle_Namespace::BOTTOM_RIGHT;

    if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT || next == Edge_Handle_Namespace::TOP_RIGHT)
        path.add_line(handle(edge, hand).p1(),
                      handle(edge, (Edge::Handle) (next | strand_bit)).p1());
    return (Edge::Handle) (next | strand_bit);
}
QString Edge_Inverted::name() const { return QObject::tr("Inverted"); }
QString Edge_Inverted::machine_name() const { return "inverted"; }

/**
 * \brief Paints the edge in the editor (dashed line).
 */
void Edge_Inverted::paint(QPainter* painter, const Edge& edge)
{
    QPen pen = painter->pen();
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    painter->drawLine(edge.to_line());
}
