/**
 * \file src/graph/edge_type.cpp
 * \brief Implementation of edge type graph logic.
 */

#include "edge_type.hpp"

#include <QDebug>
#include <QVector>

#include "edge.hpp"
#include "edge_type_utils.hpp"
#include "path_builder.hpp"

Edge_Type::Edge_Type() {}

void Edge_Type::paint_regular(QPainter* painter, const Edge& edge)
{
    int strands = edge.strand_count();
    if (strand_count() > strands)
        strands = strand_count();
    double width = 4;
    if (strands == 2)
        width = 8;
    else if (strands == 3)
        width = 12;

    QPen pen(Edge::color_resting, width);
    pen.setCosmetic(true);
    painter->setPen(pen);

    paint(painter, edge);
}
void Edge_Type::paint_highlighted(QPainter* painter, const Edge& edge)
{
    int strands = edge.strand_count();
    if (strand_count() > strands)
        strands = strand_count();
    double width = 4;
    if (strands == 2)
        width = 8;
    else if (strands == 3)
        width = 12;

    QPen pen(Edge::color_highlighted, width);
    pen.setCosmetic(true);
    painter->setPen(pen);

    paint(painter, edge);
}
void Edge_Type::paint(QPainter* painter, const Edge& edge) { painter->drawLine(edge.to_line()); }

//debug
void Edge_Type::debug_draw_handles(QPainter* painter, const Edge& edge) const
{
    painter->save();

    QVector<Edge_Handle> all_handles = {
        Edge_Handle_Namespace::TOP_LEFT,           Edge_Handle_Namespace::TOP_RIGHT,
        Edge_Handle_Namespace::BOTTOM_LEFT,        Edge_Handle_Namespace::BOTTOM_RIGHT,
        Edge_Handle_Namespace::MID_TOP_LEFT,       Edge_Handle_Namespace::MID_TOP_RIGHT,
        Edge_Handle_Namespace::MID_BOTTOM_LEFT,    Edge_Handle_Namespace::MID_BOTTOM_RIGHT,
        Edge_Handle_Namespace::CENTER_TOP_LEFT,    Edge_Handle_Namespace::CENTER_TOP_RIGHT,
        Edge_Handle_Namespace::CENTER_BOTTOM_LEFT, Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT};

    
    for (int s = 0; s < edge.strand_count(); ++s) {
        Edge_Handle strand_bit = (Edge_Handle) ((s & 0xF) << 12);

        for (Edge_Handle pure : all_handles) {
            Edge_Handle handle = (Edge_Handle) (pure | strand_bit);
            QPointF pos = get_handle_pos(&edge, handle);

            painter->setPen(Qt::black);
            if (pure & 0x000F)
                painter->setBrush(QColor(255, 0, 0, 100));
            else if (pure & 0x00F0)
                painter->setBrush(QColor(0, 255, 0, 100));
            else if (pure & 0x0F00)
                painter->setBrush(QColor(0, 0, 255, 100));

            painter->drawEllipse(pos, 5, 5);
            painter->setPen(Qt::black);
            painter->drawText(pos + QPointF(8, -8), handleToString((Edge_Handle)handle));
        }
    }

    painter->restore();
}

Edge_Handle Edge_Type::traverse(Edge* edge, Edge_Handle handle, Path_Builder& path) const
{
    Edge_Handle pure = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
    int strand = (int) ((handle & Edge_Handle_Namespace::STRAND_MASK) >> 12);

    Edge_Handle out_pure = Edge_Handle_Namespace::NO_HANDLE;

    switch (pure) {
        case Edge_Handle_Namespace::TOP_LEFT:
            out_pure = Edge_Handle_Namespace::TOP_RIGHT;
            break;
        case Edge_Handle_Namespace::TOP_RIGHT:
            out_pure = Edge_Handle_Namespace::TOP_LEFT;
            break;
        case Edge_Handle_Namespace::BOTTOM_LEFT:
            out_pure = Edge_Handle_Namespace::BOTTOM_RIGHT;
            break;
        case Edge_Handle_Namespace::BOTTOM_RIGHT:
            out_pure = Edge_Handle_Namespace::BOTTOM_LEFT;
            break;
        case Edge_Handle_Namespace::MID_TOP_LEFT:
            out_pure = Edge_Handle_Namespace::MID_TOP_RIGHT;
            break;
        case Edge_Handle_Namespace::MID_TOP_RIGHT:
            out_pure = Edge_Handle_Namespace::MID_TOP_LEFT;
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_LEFT:
            out_pure = Edge_Handle_Namespace::MID_BOTTOM_RIGHT;
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_RIGHT:
            out_pure = Edge_Handle_Namespace::MID_BOTTOM_LEFT;
            break;
        case Edge_Handle_Namespace::CENTER_TOP_LEFT:
            out_pure = Edge_Handle_Namespace::CENTER_TOP_RIGHT;
            break;
        case Edge_Handle_Namespace::CENTER_TOP_RIGHT:
            out_pure = Edge_Handle_Namespace::CENTER_TOP_LEFT;
            break;
        case Edge_Handle_Namespace::CENTER_BOTTOM_LEFT:
            out_pure = Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT;
            break;
        case Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT:
            out_pure = Edge_Handle_Namespace::CENTER_BOTTOM_LEFT;
            break;
        default:
            break;
    }

    if (out_pure != Edge_Handle_Namespace::NO_HANDLE) {
        Edge_Handle out = (Edge_Handle) (out_pure | (strand << 12));
        path.add_line(get_handle_pos(edge, handle), get_handle_pos(edge, out));
        return out;
    }
    return Edge_Handle_Namespace::NO_HANDLE;
}
