/**
 * \file src/widgets/knot_view/knot_style_controller.hpp
 * \brief Internal style command helper for Knot_View.
 */

#ifndef KNOT_STYLE_CONTROLLER_HPP
#define KNOT_STYLE_CONTROLLER_HPP

#include <QList>

#include "edge_style.hpp"
#include "node_style.hpp"

class QColor;
class Cusp_Shape;
class Edge_Type;
class Graph;
class Knot_View;

class Knot_Style_Controller
{
   public:
    explicit Knot_Style_Controller(Knot_View& view) : m_view(view) {}

    void copy_graph_style(const Graph& graph);
    void set_knot_colors(const QList<QColor>& colors);
    void set_knot_custom_colors(bool enabled);
    void set_stroke_width(double width);
    void set_join_style(Qt::PenJoinStyle style);
    void set_brush_style(Qt::BrushStyle style);

    void set_knot_handle_lenght(double value);
    void set_knot_crossing_distance(double value);
    void set_knot_spacing(double value);
    void set_knot_strand_count(int value);
    void set_knot_cusp_angle(double value);
    void set_knot_cusp_distance(double value);
    void set_knot_cusp_shape(Cusp_Shape* value);
    void set_knot_ege_slide(double value);

    void set_selection_crossing_distance(double value);
    void set_selection_spacing(double value);
    void set_selection_strand_count(int value);
    void set_selection_cusp_angle(double value);
    void set_selection_cusp_distance(double value);
    void set_selection_cusp_shape(Cusp_Shape* value);
    void set_selection_edge_type(Edge_Type* value);
    void set_selection_handle_lenght_nodes(double value);
    void set_selection_enabled_styles_nodes(Node_Style::Enabled_Styles value);
    void set_selection_handle_lenght_edges(double value);
    void set_selection_enabled_styles_edges(Edge_Style::Enabled_Styles value);
    void set_selection_edge_slide(double value);

   private:
    Knot_View& m_view;
};

#endif  // KNOT_STYLE_CONTROLLER_HPP
