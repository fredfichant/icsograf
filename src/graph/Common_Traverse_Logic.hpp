#include <cmath>

#include "edge.hpp"
#include "edge_type.hpp"
#include "point_math.hpp"

Edge_Handle Common_Traverse_Logic(Edge* edge, Edge_Handle hand, Path_Builder& path,
                                  bool inverted_crossing)
{
    // Extract strand index (s) and pure handle (pure_hand)
    int s = 0;
    if (hand & Edge_Handle_Namespace::STRAND_MASK) {
        if (hand & Edge_Handle_Namespace::STRAND_2)
            s = 2;
        else if (hand & Edge_Handle_Namespace::STRAND_1)
            s = 1;
    }

    Edge_Handle pure_hand = (Edge_Handle) (hand & Edge_Handle_Namespace::HANDLE_MASK);

    Edge_Handle pure_next = Edge_Handle_Namespace::NO_HANDLE;
    if (pure_hand == Edge_Handle_Namespace::TOP_RIGHT)
        pure_next = Edge_Handle_Namespace::BOTTOM_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::BOTTOM_RIGHT)
        pure_next = Edge_Handle_Namespace::TOP_LEFT;
    else if (pure_hand == Edge_Handle_Namespace::BOTTOM_LEFT)
        pure_next = Edge_Handle_Namespace::TOP_RIGHT;
    else if (pure_hand == Edge_Handle_Namespace::TOP_LEFT)
        pure_next = Edge_Handle_Namespace::BOTTOM_RIGHT;

    // The next handle preserves the strand index
    Edge_Handle next = (Edge_Handle) (pure_next | (hand & Edge_Handle_Namespace::STRAND_MASK));

    Edge_Style style = edge->defaulted_style();
    int strand_count = edge->style().edge_type->strand_count();
    double spacing = style.spacing();

    // Base points for the crossing (at the center of the crossing)
    // We use a temporary Edge_Normal_1_Strand to get the canonical handle positions
    QLineF h_base_line = Edge_Normal_1_Strand().handle(edge, pure_hand);
    QLineF n_base_line = Edge_Normal_1_Strand().handle(edge, pure_next);
    QPointF p1_base = h_base_line.p1();
    QPointF p2_base = n_base_line.p1();

    // Calculate offset for this specific strand
    double offset = 0;
    if (strand_count == 2) {
        offset = (s == 0) ? -spacing / 2.0 : spacing / 2.0;
    } else if (strand_count == 3) {
        offset = (s == 0) ? -spacing : (s == 1 ? 0.0 : spacing);
    }

    // Perpendicular angle for offset calculation
    QLineF main_line = edge->to_line();
    long double edge_angle_rad = deg2rad(main_line.angle());
    long double perp_angle_rad = edge_angle_rad + (M_PI / 2.0);

    QPointF p1_offset(p1_base.x() + offset * std::cos(perp_angle_rad),
                      p1_base.y() - offset * std::sin(perp_angle_rad));
    QPointF p2_offset(p2_base.x() + offset * std::cos(perp_angle_rad),
                      p2_base.y() - offset * std::sin(perp_angle_rad));

    // For better smoothness with the cubic cusps at nodes, we use a quadratic Bezier
    // for the crossing instead of a simple line. The control point is the midpoint.
    QPointF control_offset = (p1_offset + p2_offset) / 2.0;

    // Only add the line if this side of the crossing should be rendered
    if ((inverted_crossing && (pure_hand == Edge_Handle_Namespace::TOP_RIGHT ||
                               pure_next == Edge_Handle_Namespace::TOP_RIGHT)) ||
        (!inverted_crossing && (pure_hand == Edge_Handle_Namespace::TOP_LEFT ||
                                pure_next == Edge_Handle_Namespace::TOP_LEFT))) {
        path.add_quad(p1_offset, control_offset, p2_offset);
    }

    return next;
}
