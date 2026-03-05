#include "edge_inverted_logic.hpp"
#include <QPen>

void Edge_Inverted_Logic::paint_inverted(QPainter* painter, const Edge& edge)
{
    QPen pen = painter->pen();
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    painter->drawLine(edge.to_line());
}
