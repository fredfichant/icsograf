/**
 * \file src/graph/edge_handle_calculator.cpp
 * \brief Implementation of edge handle calculator graph logic.
 */

#include "edge_handle_calculator.hpp"

#include <qmath.h>

#include "edge.hpp"
#include "edge_style.hpp"
#include "edge_type_utils.hpp"
#include "point_math.hpp"

QLineF Standard_Edge_Handle_Calculator::calculate(const Edge* edge, Edge_Handle handle) const
{
    return standard_handle(edge, handle);
}

QLineF Standard_Edge_Handle_Calculator::standard_handle(const Edge* edge, Edge_Handle handle)
{
    QPointF p1 = get_handle_pos(edge, handle);
    Edge_Style style = edge->defaulted_style();
    long double handle_angle = 0;
    Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);

    if (pure_handle & (Edge_Handle_Namespace::TOP_RIGHT | Edge_Handle_Namespace::MID_TOP_RIGHT |
                       Edge_Handle_Namespace::CENTER_TOP_RIGHT))
        handle_angle = pi() / 4.0;
    else if (pure_handle & (Edge_Handle_Namespace::TOP_LEFT | Edge_Handle_Namespace::MID_TOP_LEFT |
                            Edge_Handle_Namespace::CENTER_TOP_LEFT))
        handle_angle = pi() * 3.0 / 4.0;
    else if (pure_handle &
             (Edge_Handle_Namespace::BOTTOM_LEFT | Edge_Handle_Namespace::MID_BOTTOM_LEFT |
              Edge_Handle_Namespace::CENTER_BOTTOM_LEFT))
        handle_angle = pi() * 5.0 / 4.0;
    else if (pure_handle &
             (Edge_Handle_Namespace::BOTTOM_RIGHT | Edge_Handle_Namespace::MID_BOTTOM_RIGHT |
              Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT))
        handle_angle = pi() * 7.0 / 4.0;

    handle_angle += deg2rad(edge->to_line().angle());

    QPointF p2(p1.x() + style.handle_length * qCos(handle_angle),
               p1.y() - style.handle_length * qSin(handle_angle));

    return QLineF(p1, p2);
}
