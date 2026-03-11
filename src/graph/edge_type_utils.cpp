/**
 * \file src/graph/edge_type_utils.cpp
 * \brief Implementation of edge type utils graph logic.
 */

#include "edge_type_utils.hpp"

#include <qmath.h>

#include "edge.hpp"
#include "edge_type.hpp"
#include "point_math.hpp"

QString handleToString(Edge_Handle handle)
{
    Edge_Handle pure = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
    int strand = (int) ((handle & Edge_Handle_Namespace::STRAND_MASK) >> 12);
    QString name;
    switch (pure) {
        case Edge_Handle_Namespace::TOP_LEFT:
            name = "tl";
            break;
        case Edge_Handle_Namespace::TOP_RIGHT:
            name = "tr";
            break;
        case Edge_Handle_Namespace::BOTTOM_LEFT:
            name = "bl";
            break;
        case Edge_Handle_Namespace::BOTTOM_RIGHT:
            name = "br";
            break;
        case Edge_Handle_Namespace::MID_TOP_LEFT:
            name = "mtl";
            break;
        case Edge_Handle_Namespace::MID_TOP_RIGHT:
            name = "mtr";
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_LEFT:
            name = "mbl";
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_RIGHT:
            name = "mbr";
            break;
        case Edge_Handle_Namespace::CENTER_TOP_LEFT:
            name = "ctl";
            break;
        case Edge_Handle_Namespace::CENTER_TOP_RIGHT:
            name = "ctr";
            break;
        case Edge_Handle_Namespace::CENTER_BOTTOM_LEFT:
            name = "cbl";
            break;
        case Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT:
            name = "cbr";
            break;
        default:
            name = QString("UNKNOWN(0x%1)").arg(pure, 4, 16, QChar('0'));
            break;
    }
    return QString("%1 (S%2)").arg(name).arg(strand);
}

QPointF get_handle_pos(const Edge* edge, Edge_Handle handle)
{
    Edge_Style style = edge->defaulted_style();
    Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
    int strand = (int) ((handle & Edge_Handle_Namespace::STRAND_MASK) >> 12);

    const long double edge_angle = deg2rad(edge->to_line().angle());
    const double h = style.crossing_distance / 2.0;
    const double spacing = style.spacing;
    const QString edge_type_name = style.edge_type ? style.edge_type->machine_name() : QString();
    const bool is_2strand = edge_type_name.startsWith("2strand");
    const bool is_3strand = edge_type_name.startsWith("3strand");

    double sx = 0.0;
    if (pure_handle & (Edge_Handle_Namespace::TOP_RIGHT | Edge_Handle_Namespace::MID_TOP_RIGHT |
                       Edge_Handle_Namespace::CENTER_TOP_RIGHT |
                       Edge_Handle_Namespace::BOTTOM_RIGHT | Edge_Handle_Namespace::MID_BOTTOM_RIGHT |
                       Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT))
        sx = +1.0;
    else if (pure_handle & (Edge_Handle_Namespace::TOP_LEFT | Edge_Handle_Namespace::MID_TOP_LEFT |
                            Edge_Handle_Namespace::CENTER_TOP_LEFT |
                            Edge_Handle_Namespace::BOTTOM_LEFT | Edge_Handle_Namespace::MID_BOTTOM_LEFT |
                            Edge_Handle_Namespace::CENTER_BOTTOM_LEFT))
        sx = -1.0;

    double row_y = 0.0;
    if (is_3strand) {
        const double top = 2.0 * h + spacing;
        const double mid_top = h + spacing;
        const double center_top = h;
        const double center_bottom = -h;
        const double mid_bottom = -h - spacing;
        const double bottom = -2.0 * h - spacing;

        if (pure_handle & (Edge_Handle_Namespace::TOP_LEFT | Edge_Handle_Namespace::TOP_RIGHT))
            row_y = top;
        else if (pure_handle &
                 (Edge_Handle_Namespace::MID_TOP_LEFT | Edge_Handle_Namespace::MID_TOP_RIGHT))
            row_y = mid_top;
        else if (pure_handle &
                 (Edge_Handle_Namespace::CENTER_TOP_LEFT | Edge_Handle_Namespace::CENTER_TOP_RIGHT))
            row_y = center_top;
        else if (pure_handle & (Edge_Handle_Namespace::CENTER_BOTTOM_LEFT |
                                Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT))
            row_y = center_bottom;
        else if (pure_handle &
                 (Edge_Handle_Namespace::MID_BOTTOM_LEFT | Edge_Handle_Namespace::MID_BOTTOM_RIGHT))
            row_y = mid_bottom;
        else if (pure_handle &
                 (Edge_Handle_Namespace::BOTTOM_LEFT | Edge_Handle_Namespace::BOTTOM_RIGHT))
            row_y = bottom;
    } else if (is_2strand) {
        const double top = h + spacing / 2.0;
        const double mid_top = spacing / 2.0;
        const double mid_bottom = -spacing / 2.0;
        const double bottom = -h - spacing / 2.0;

        if (pure_handle & (Edge_Handle_Namespace::TOP_LEFT | Edge_Handle_Namespace::TOP_RIGHT))
            row_y = top;
        else if (pure_handle &
                 (Edge_Handle_Namespace::MID_TOP_LEFT | Edge_Handle_Namespace::MID_TOP_RIGHT))
            row_y = mid_top;
        else if (pure_handle &
                 (Edge_Handle_Namespace::MID_BOTTOM_LEFT | Edge_Handle_Namespace::MID_BOTTOM_RIGHT))
            row_y = mid_bottom;
        else if (pure_handle &
                 (Edge_Handle_Namespace::BOTTOM_LEFT | Edge_Handle_Namespace::BOTTOM_RIGHT))
            row_y = bottom;
        else if (pure_handle &
                 (Edge_Handle_Namespace::CENTER_TOP_LEFT | Edge_Handle_Namespace::CENTER_TOP_RIGHT))
            row_y = mid_top;
        else if (pure_handle & (Edge_Handle_Namespace::CENTER_BOTTOM_LEFT |
                                Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT))
            row_y = mid_bottom;
    } else {
        if (pure_handle & (Edge_Handle_Namespace::TOP_LEFT | Edge_Handle_Namespace::TOP_RIGHT))
            row_y = h;
        else if (pure_handle &
                 (Edge_Handle_Namespace::BOTTOM_LEFT | Edge_Handle_Namespace::BOTTOM_RIGHT))
            row_y = -h;
        else if (pure_handle &
                 (Edge_Handle_Namespace::MID_TOP_LEFT | Edge_Handle_Namespace::MID_TOP_RIGHT))
            row_y = h;
        else if (pure_handle &
                 (Edge_Handle_Namespace::MID_BOTTOM_LEFT | Edge_Handle_Namespace::MID_BOTTOM_RIGHT))
            row_y = -h;
        else if (pure_handle &
                 (Edge_Handle_Namespace::CENTER_TOP_LEFT | Edge_Handle_Namespace::CENTER_TOP_RIGHT))
            row_y = h / 2.0;
        else if (pure_handle & (Edge_Handle_Namespace::CENTER_BOTTOM_LEFT |
                                Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT))
            row_y = -h / 2.0;
    }

    const double dx = sx * h;
    const double dy = row_y;

    // Rotation selon l'angle de l'arête
    const double rx = dx * qCos(edge_angle) - dy * qSin(edge_angle);
    const double ry = dx * qSin(edge_angle) + dy * qCos(edge_angle);

    // Point de base sur l'arête
    QPointF p = edge->to_line().pointAt(style.edge_slide);
    p.setX(p.x() + rx);
    p.setY(p.y() - ry);  // Qt: y down

    // Décalage latéral selon le strand (perpendiculaire à l'arête)
    if (style.strand_count > 1) {
        double strand_offset = 0;
        if (style.strand_count == 2) {
            strand_offset = (strand == 0) ? -style.spacing / 2.0 : style.spacing / 2.0;
        } else if (style.strand_count == 3) {
            // Pour 3 brins: strand 0 (gauche), strand 1 (centre), strand 2 (droite)
            strand_offset = (strand - 1) * style.spacing;
        }

        long double perp_angle = edge_angle + pi() / 2.0;
        p.setX(p.x() + strand_offset * qCos(perp_angle));
        p.setY(p.y() - strand_offset * qSin(perp_angle));
    }

    return p;
}
