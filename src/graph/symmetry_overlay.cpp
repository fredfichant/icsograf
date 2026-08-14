#include "symmetry_overlay.hpp"

#include <algorithm>
#include <QPainter>
#include <QFont>

#include "graph.hpp"
#include "node.hpp"

Symmetry_Overlay::Symmetry_Overlay(const Graph& graph) : m_graph(graph) { setZValue(1000); }

void Symmetry_Overlay::set_result(const QVector<QLineF>& axes, const QVector<QVector<int>>& orbits)
{
    prepareGeometryChange();
    m_axes = axes;
    m_orbits = orbits;
    update();
}

QRectF Symmetry_Overlay::boundingRect() const
{
    return m_graph.boundingRect().adjusted(-10000, -10000, 10000, 10000);
}

void Symmetry_Overlay::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPen axis_pen(QColor(220, 80, 65), 2, Qt::DashLine);
    axis_pen.setCosmetic(true);
    painter->setPen(axis_pen);
    for (const QLineF& axis : m_axes) {
        QLineF line(axis);
        line.setLength(10000);
        line.setP1(axis.center() - (line.p2() - line.p1()) / 2.0);
        painter->drawLine(line);
    }
    const QList<Node*> nodes = m_graph.nodes();
    const QVector<QColor> colors = {QColor(50, 130, 210), QColor(230, 145, 45), QColor(115, 175, 90)};
    for (int orbit_index = 0; orbit_index < m_orbits.size(); ++orbit_index) {
        QPen pen(colors[orbit_index % colors.size()], 3);
        pen.setCosmetic(true);
        painter->setPen(pen);
        for (int vertex : m_orbits[orbit_index]) {
            if (vertex < 0 || vertex >= nodes.size()) continue;
            const QPointF position = nodes[vertex]->pos();
            painter->drawEllipse(position, 9, 9);
            QFont font = painter->font();
            font.setBold(true);
            font.setPointSizeF(10.0 / std::max(0.001, std::abs(painter->transform().m11())));
            painter->setFont(font);
            const QString label = QString::number(vertex + 1);
            const QPointF label_position = position + QPointF(11, -11);
            QPen outline(Qt::white, 3);
            outline.setCosmetic(true);
            painter->setPen(outline);
            painter->drawText(label_position, label);
            QPen label_pen(Qt::black, 1);
            label_pen.setCosmetic(true);
            painter->setPen(label_pen);
            painter->drawText(label_position, label);
        }
    }
}
