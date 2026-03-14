/**
 * \file src/graph/edge.cpp
 * \brief Implementation of edge graph logic.
 */

#include "edge.hpp"

#include <QDebug>

#include "edge_style.hpp"
#include "edge_type.hpp"
#include "graph.hpp"
#include "resource_manager.hpp"

QColor Edge::color_resting(Qt:: gray);
QColor Edge::color_highlighted(Qt::cyan);
QColor Edge::color_selected(128, 128, 128, 128);

Edge::Edge(Node* v1, Node* v2, Edge_Type* type)
    : v1(v1),
      v2(v2),
      available_handles(Edge_Handle_Namespace::TOP_LEFT | Edge_Handle_Namespace::TOP_RIGHT |
                        Edge_Handle_Namespace::BOTTOM_LEFT | Edge_Handle_Namespace::BOTTOM_RIGHT),
      m_graph(nullptr),
      m_marking_color(Qt::black)
{
    attach();
    setZValue(1);
    setFlag(QGraphicsItem::ItemIsSelectable);

    m_style.enabled_style |= Edge_Style::EDGE_TYPE;
    m_style.edge_type = type ? type : resource_manager().default_edge_type();
}

QRectF Edge::boundingRect() const
{
    QRectF bb = QRectF(v1->pos(), v2->pos()).normalized();
    bb.setTop(bb.top() - shapew / 2);
    bb.setLeft(bb.left() - shapew / 2);
    bb.setWidth(bb.width() + shapew);
    bb.setHeight(bb.height() + shapew);
    return bb;
}

void Edge::detach()
{
    v1->remove_edge(this);
    v2->remove_edge(this);
}

void Edge::attach()
{
    v1->add_edge(this);
    v2->add_edge(this);
}

void Edge::set_style(Edge_Style st)
{
    m_style = st;
    m_style.enabled_style |= Edge_Style::EDGE_TYPE;
    m_style.edge_type = st.edge_type ? st.edge_type : resource_manager().default_edge_type();
}

Edge_Style Edge::style() const { return m_style; }

Edge_Style& Edge::style() { return m_style; }

Edge_Style Edge::defaulted_style() const
{
    return m_style.default_to(m_graph->default_edge_style());
}

int Edge::strand_count() const { return defaulted_style().strand_count; }

void Edge::set_strand_count(int count)
{
    m_style.strand_count = count;
    m_style.enabled_style |= Edge_Style::STRAND_COUNT;
}

double Edge::spacing() const { return defaulted_style().spacing; }

void Edge::set_spacing(double s)
{
    m_style.spacing = s;
    m_style.enabled_style |= Edge_Style::SPACING;
}

void Edge::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (isSelected()) {
        QPen pen(color_selected, 2);
        pen.setCosmetic(true);
        painter->setPen(pen);
        QLineF nv = to_line().normalVector();
        nv.setLength((Node::external_radius() - 2) / painter->transform().m11());
        painter->drawLine(to_line().translated(nv.dx(), nv.dy()));
        painter->drawLine(to_line().translated(-nv.dx(), -nv.dy()));
    }

    if (visible && highlighted)
        m_style.edge_type->paint_highlighted(painter, *this);
    else if (visible || highlighted)
        m_style.edge_type->paint_regular(painter, *this);

    if (m_marking_color != Qt::black) {
        QPen p = painter->pen();
        p.setColor(m_marking_color);
        p.setWidthF(2.0);
        painter->setPen(p);
        painter->drawLine(to_line());
    }
    /*
    if (isSelected()) {  // Ou une autre condition
        m_style.edge_type->debug_draw_handles(painter, *this);
    }
    */
}

QPainterPath Edge::shape() const
{
    QPointF startp = v1->pos();
    QPointF endp = v2->pos();

    QPainterPath line;
    line.moveTo(startp);
    line.lineTo(endp);
    QPainterPathStroker pps;
    pps.setWidth(shapew);

    return pps.createStroke(line);
}
