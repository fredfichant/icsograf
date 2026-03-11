/**
 * \file src/graph/node_cusp_shape.cpp
 * \brief Implementation of node cusp shape graph logic.
 */

#include "node_cusp_shape.hpp"

#include "edge_type.hpp"
#include "traversal_info.hpp"

QPointF Cusp_Shape::cusp_point(const Traversal_Info& ti, double def_dist) const
{
    QLineF bisect(0, 0, 1, 1);
    // place line in the cusp node and give it the right direction
    bisect.setP1(ti.node->pos());
    int d = 1;
    if (ti.handside == Traversal_Info::LEFT) d = -1;
    double delta = ti.in.angle + d * ti.angle_delta / 2;
    bisect.setAngle(delta);
    bisect.setLength(def_dist);
    return bisect.p2();
}

void Cusp_Shape::default_path(Path_Builder& path, QLineF start, QLineF finish) const
{
    double distance = point_distance(start.p1(), finish.p1());
    if (distance < start.length() + finish.length()) {
        QPointF sum = finish.p1() + start.p2() - finish.p2();
        QLineF connect(start.p1(), finish.p1());
        QPointF proj = project(sum, connect);
        double proj_distance = point_distance(start.p1(), proj);
        if (proj_distance >= connect.length()) {
            double factor = distance / (start.length() + finish.length());
            start.setLength(start.length() * factor);
            finish.setLength(finish.length() * factor);
        }
    }
    path.add_cubic(start.p1(), start.p2(), finish.p2(), finish.p1());
}

void Cusp_Rounded::draw_joint(Path_Builder& path, const Traversal_Info& ti,
                              const Node_Style& style) const
{
    QLineF start = ti.in.edge->style().edge_type->handle(ti.in.edge, ti.in.handle);
    QLineF finish = ti.out.edge->style().edge_type->handle(ti.out.edge, ti.out.handle);

    if (ti.angle_delta > style.cusp_angle)  // draw cusp
    {
        QPointF cusp_pt = cusp_point(ti, style.cusp_distance);

        QLineF handle(start.p1(), finish.p1());
        handle.translate(cusp_pt - start.p1());
        handle.setLength(style.handle_length);
        QPointF h2 = handle.p2();
        handle.setLength(-style.handle_length);
        QPointF h1 = handle.p2();

        path.add_cubic(start.p1(), start.p2(), h1, cusp_pt);
        path.add_cubic(finish.p1(), finish.p2(), h2, cusp_pt);

    } else {
        default_path(path, start, finish);
    }
}
