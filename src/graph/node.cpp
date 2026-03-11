/**
 * \file src/graph/node.cpp
 * \brief Implementation of node graph logic.
 */

#include "node.hpp"

#include "edge.hpp"

int Node::radius = 5;
QColor Node::color_resting(Qt::red);
QColor Node::color_highlighted(Qt::yellow);
QColor Node::color_selected(Qt::darkGray);

Node::Node(QPointF pos)
{
    setPos(pos);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setZValue(2);
}

void Node::add_edge(Edge* e)
{
    if (!m_edges.contains(e)) m_edges.append(e);
}

void Node::remove_edge(Edge* e) { m_edges.removeOne(e); }

bool Node::has_edge_to(const Node* n) const
{
    foreach (Edge* e, m_edges) {
        if (e->other(this) == n) return true;
    }
    return false;
}

Edge* Node::edge_to(const Node* n) const
{
    foreach (Edge* e, m_edges) {
        if (e->other(this) == n) return e;
    }
    return nullptr;
}

/*double Node::distance_squared(QPointF to) const
{
    return point_distance_squared(pos(),to);
}*/

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QRectF r(-radius, -radius, radius * 2, radius * 2);

    if (isSelected()) {
        painter->setPen(QPen(color_selected, 2));
        painter->setBrush(Qt::white);
        painter->drawRect(r);
    }

    painter->setPen(Qt::black);

    if (visible && highlighted) {
        painter->setBrush(color_highlighted);
        r = boundingRect();
    } else if (visible || highlighted)
        painter->setBrush(color_resting);
    else
        return;
    painter->drawEllipse(r);
}

QRectF Node::boundingRect() const
{
    return QRectF(-external_radius(), -external_radius(), external_radius() * 2,
                  external_radius() * 2);
}

void Node::move(QPointF p)
{
    setPos(p);
    emit moved(p);
}
