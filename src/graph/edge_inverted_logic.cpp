/**
 * \file src/graph/edge_inverted_logic.cpp
 * \brief Implementation of edge inverted logic graph logic.
 */

#include "edge_inverted_logic.hpp"

#include <QPen>

#include "edge_type.hpp"
#include "edge_type_utils.hpp"

void Edge_Inverted_Logic::paint_inverted(QPainter* painter, const Edge& edge)
{
    QPen pen = painter->pen();
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    int strands = edge.strand_count();
    if (edge.effective_edge_type() && edge.effective_edge_type()->strand_count() > strands)
        strands = edge.effective_edge_type()->strand_count();

    draw_editor_edge_lines(painter, edge, strands);
}
