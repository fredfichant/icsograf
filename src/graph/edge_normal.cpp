/**
 * \file src/graph/edge_normal.cpp
 * \brief Implementation of edge normal graph logic.
 */

#include "edge_normal.hpp"
#include <qmath.h>
#include "edge.hpp"
#include "edge_handle_calculator.hpp"
#include "edge_inverted_logic.hpp"
#include "edge_type_utils.hpp"
#include "point_math.hpp"

/**
 * \brief Calculates the geometry for a specific handle on the edge.
 *
 * Uses the standard diagonal handle calculation logic.
 */
QLineF Edge_Normal::handle(const Edge* edge, Edge::Handle handle) const
{
    return Standard_Edge_Handle_Calculator::standard_handle(edge, handle);
}

/**
 * \brief Implementation of traversal logic for regular and inverted crossings.
 */
static Edge::Handle traverse_normal_impl(const Edge_Type* type, Edge* edge, Edge::Handle hand,
                                         Path_Builder& path, bool inverted)
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

    Edge_Handle over_handle = inverted ? Edge_Handle_Namespace::TOP_RIGHT : Edge_Handle_Namespace::TOP_LEFT;

    if (pure_hand == over_handle || next == over_handle) {
        path.add_line(type->handle(edge, hand).p1(),
                      type->handle(edge, (Edge::Handle) (next | strand_bit)).p1());
    }

    return (Edge::Handle) (next | strand_bit);
}

/**
 * \brief Traverses the edge for a standard crossing.
 *
 * Connects TL <-> BR and TR <-> BL. Draws the TL <-> BR strand (Over).
 */
Edge::Handle Edge_Normal::traverse(Edge* edge, Edge::Handle hand, Path_Builder& path) const
{
    return traverse_normal_impl(this, edge, hand, path, false);
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
    return traverse_normal_impl(this, edge, hand, path, true);
}
QString Edge_Inverted::name() const { return QObject::tr("Inverted"); }
QString Edge_Inverted::machine_name() const { return "inverted"; }

/**
 * \brief Paints the edge in the editor (dashed line).
 */
void Edge_Inverted::paint(QPainter* painter, const Edge& edge)
{
    Edge_Inverted_Logic::paint_inverted(painter, edge);
}

