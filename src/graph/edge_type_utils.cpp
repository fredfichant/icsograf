#include "edge_type_utils.hpp"

#include <qmath.h>

#include "edge.hpp"
#include "point_math.hpp"

QString handleToString(Edge_Handle handle)
{
    Edge_Handle pure = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
    int strand = (int) ((handle & Edge_Handle_Namespace::STRAND_MASK) >> 12);
    QString name;
    switch (pure) {
        case Edge_Handle_Namespace::TOP_LEFT:
            name = "TOP_LEFT";
            break;
        case Edge_Handle_Namespace::TOP_RIGHT:
            name = "TOP_RIGHT";
            break;
        case Edge_Handle_Namespace::BOTTOM_LEFT:
            name = "BOTTOM_LEFT";
            break;
        case Edge_Handle_Namespace::BOTTOM_RIGHT:
            name = "BOTTOM_RIGHT";
            break;
        case Edge_Handle_Namespace::MID_TOP_LEFT:
            name = "MID_TOP_LEFT";
            break;
        case Edge_Handle_Namespace::MID_TOP_RIGHT:
            name = "MID_TOP_RIGHT";
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_LEFT:
            name = "MID_BOTTOM_LEFT";
            break;
        case Edge_Handle_Namespace::MID_BOTTOM_RIGHT:
            name = "MID_BOTTOM_RIGHT";
            break;
        case Edge_Handle_Namespace::CENTER_TOP_LEFT:
            name = "CENTER_TOP_LEFT";
            break;
        case Edge_Handle_Namespace::CENTER_TOP_RIGHT:
            name = "CENTER_TOP_RIGHT";
            break;
        case Edge_Handle_Namespace::CENTER_BOTTOM_LEFT:
            name = "CENTER_BOTTOM_LEFT";
            break;
        case Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT:
            name = "CENTER_BOTTOM_RIGHT";
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
    const double base_distance = style.crossing_distance / 2.0;

    // Déterminer le quadrant (signes en coordonnées mathématiques)
    double sx = 0.0, sy = 0.0;
    if (pure_handle & (Edge_Handle_Namespace::TOP_RIGHT | Edge_Handle_Namespace::MID_TOP_RIGHT |
                       Edge_Handle_Namespace::CENTER_TOP_RIGHT)) {
        sx = +1.0;
        sy = +1.0;
    } else if (pure_handle &
               (Edge_Handle_Namespace::TOP_LEFT | Edge_Handle_Namespace::MID_TOP_LEFT |
                Edge_Handle_Namespace::CENTER_TOP_LEFT)) {
        sx = -1.0;
        sy = +1.0;
    } else if (pure_handle &
               (Edge_Handle_Namespace::BOTTOM_LEFT | Edge_Handle_Namespace::MID_BOTTOM_LEFT |
                Edge_Handle_Namespace::CENTER_BOTTOM_LEFT)) {
        sx = -1.0;
        sy = -1.0;
    } else if (pure_handle &
               (Edge_Handle_Namespace::BOTTOM_RIGHT | Edge_Handle_Namespace::MID_BOTTOM_RIGHT |
                Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT)) {
        sx = +1.0;
        sy = -1.0;
    }

    // Déterminer le niveau et calculer les distances
    double dx = 0, dy = 0;

    if (pure_handle & 0x000F) {  // Level 0 (OUTER)
        dx = sx * base_distance;
        dy = sy * (base_distance * 3.0);  // Plus éloigné sur l'axe Y
    } else if (pure_handle & 0x00F0) {    // Level 1 (MID)
        dx = sx * base_distance;
        dy = sy * (base_distance * 2.0);
    } else if (pure_handle & 0x0F00) {  // Level 2 (CENTER)
        dx = sx * base_distance;
        dy = sy * base_distance;
    }

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
