/**
 * \file src/widgets/knot_view/knot_style_controller.cpp
 * \brief Internal style command helper for Knot_View.
 */

#include "knot_style_controller.hpp"

#include "commands.hpp"
#include "edge.hpp"
#include "graph.hpp"
#include "knot_view.hpp"
#include "node.hpp"

void Knot_Style_Controller::copy_graph_style(const Graph& graph)
{
    m_view.begin_macro("Copy Style");

    set_knot_colors(graph.colors());
    set_knot_custom_colors(graph.custom_colors());
    set_stroke_width(graph.width());
    set_join_style(graph.join_style());
    set_brush_style(graph.brush_style());
    set_knot_crossing_distance(graph.default_edge_style().crossing_distance);
    set_knot_cusp_angle(graph.default_node_style().cusp_angle);
    set_knot_cusp_distance(graph.default_node_style().cusp_distance);
    set_knot_cusp_shape(graph.default_node_style().cusp_shape);
    set_knot_handle_lenght(graph.default_node_style().handle_length);

    m_view.end_macro();
}

void Knot_Style_Controller::set_knot_colors(const QList<QColor>& colors)
{
    m_view.push_command(new Change_Colors(m_view.m_graph.colors(), colors, &m_view));
}

void Knot_Style_Controller::set_knot_custom_colors(bool enabled)
{
    m_view.push_command(new Custom_Colors(m_view.m_graph.custom_colors(), enabled, &m_view));
}

void Knot_Style_Controller::set_stroke_width(double width)
{
    m_view.push_command(new Knot_Width(m_view.m_graph.width(), width, &m_view));
}

void Knot_Style_Controller::set_join_style(Qt::PenJoinStyle style)
{
    m_view.push_command(new Pen_Join_Style(m_view.m_graph.join_style(), style, &m_view));
}

void Knot_Style_Controller::set_brush_style(Qt::BrushStyle style)
{
    m_view.push_command(new Brush_Style(m_view.m_graph.brush_style(), style, &m_view));
}

void Knot_Style_Controller::set_knot_handle_lenght(double value)
{
    m_view.push_command(
        new Knot_Style_Handle_Lenght(m_view.m_graph.default_node_style().handle_length, value, &m_view));
}

void Knot_Style_Controller::set_knot_crossing_distance(double value)
{
    m_view.push_command(new Knot_Style_Crossing_Distance(
        m_view.m_graph.default_edge_style().crossing_distance, value, &m_view));
}

void Knot_Style_Controller::set_knot_spacing(double value)
{
    m_view.push_command(
        new Knot_Style_Spacing(m_view.m_graph.default_edge_style().spacing, value, &m_view));
}

void Knot_Style_Controller::set_knot_strand_count(int value)
{
    m_view.push_command(
        new Knot_Style_Strand_Count(m_view.m_graph.default_edge_style().strand_count, value, &m_view));
}

void Knot_Style_Controller::set_knot_cusp_angle(double value)
{
    m_view.push_command(
        new Knot_Style_Cusp_Angle(m_view.m_graph.default_node_style().cusp_angle, value, &m_view));
}

void Knot_Style_Controller::set_knot_cusp_distance(double value)
{
    m_view.push_command(new Knot_Style_Cusp_Distance(
        m_view.m_graph.default_node_style().cusp_distance, value, &m_view));
}

void Knot_Style_Controller::set_knot_cusp_shape(Cusp_Shape* value)
{
    m_view.push_command(
        new Knot_Style_Cusp_Shape(m_view.m_graph.default_node_style().cusp_shape, value, &m_view));
}

void Knot_Style_Controller::set_knot_ege_slide(double value)
{
    m_view.push_command(
        new Knot_Style_Edge_Slide(m_view.m_graph.default_edge_style().edge_slide, value, &m_view));
}

void Knot_Style_Controller::set_selection_crossing_distance(double value)
{
    QList<Edge*> edges = m_view.selected_edges();
    QList<double> before;
    QList<double> after;
    for (Edge* edge : edges) {
        before.push_back(edge->style().crossing_distance);
        after.push_back(value);
    }
    m_view.push_command(new Edge_Style_Crossing_Distance(edges, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_spacing(double value)
{
    QList<Edge*> edges = m_view.selected_edges();
    QList<double> before;
    QList<double> after;
    for (Edge* edge : edges) {
        before.push_back(edge->style().spacing);
        after.push_back(value);
    }
    m_view.push_command(new Edge_Style_Spacing(edges, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_strand_count(int value)
{
    QList<Edge*> edges = m_view.selected_edges();
    QList<double> before;
    QList<double> after;
    for (Edge* edge : edges) {
        before.push_back(edge->style().strand_count);
        after.push_back((double) value);
    }
    m_view.push_command(new Edge_Style_Strand_Count(edges, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_cusp_angle(double value)
{
    QList<Node*> nodes = m_view.selected_nodes();
    QList<double> before;
    QList<double> after;
    for (Node* node : nodes) {
        before.push_back(node->style().cusp_angle);
        after.push_back(value);
    }
    m_view.push_command(new Node_Style_Cusp_Angle(nodes, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_cusp_distance(double value)
{
    QList<Node*> nodes = m_view.selected_nodes();
    QList<double> before;
    QList<double> after;
    for (Node* node : nodes) {
        before.push_back(node->style().cusp_distance);
        after.push_back(value);
    }
    m_view.push_command(new Node_Style_Cusp_Distance(nodes, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_cusp_shape(Cusp_Shape* value)
{
    QList<Node*> nodes = m_view.selected_nodes();
    QList<Cusp_Shape*> before;
    QList<Cusp_Shape*> after;
    for (Node* node : nodes) {
        before.push_back(node->style().cusp_shape);
        after.push_back(value);
    }
    m_view.push_command(new Node_Style_Cusp_Shape(nodes, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_edge_type(Edge_Type* value)
{
    m_view.begin_macro(QObject::tr("Change Edge Type"));

    for (Edge* edge : m_view.selected_edges()) {
        m_view.push_command(new Change_Edge_Type(edge, edge->style().edge_type, value, &m_view));
    }

    m_view.end_macro();
}

void Knot_Style_Controller::set_selection_handle_lenght_nodes(double value)
{
    QList<Node*> nodes = m_view.selected_nodes();
    QList<double> before;
    QList<double> after;
    for (Node* node : nodes) {
        before.push_back(node->style().handle_length);
        after.push_back(value);
    }
    m_view.push_command(new Node_Style_Handle_Lenght(nodes, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_enabled_styles_nodes(Node_Style::Enabled_Styles value)
{
    QList<Node*> nodes = m_view.selected_nodes();
    QList<Node_Style::Enabled_Styles> before;
    QList<Node_Style::Enabled_Styles> after;
    for (Node* node : nodes) {
        before.push_back(node->style().enabled_style);
        after.push_back(value);
    }
    m_view.push_command(new Node_Style_Enable(nodes, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_handle_lenght_edges(double value)
{
    QList<Edge*> edges = m_view.selected_edges();
    QList<double> before;
    QList<double> after;
    for (Edge* edge : edges) {
        before.push_back(edge->style().handle_length);
        after.push_back(value);
    }
    m_view.push_command(new Edge_Style_Handle_Lenght(edges, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_enabled_styles_edges(Edge_Style::Enabled_Styles value)
{
    QList<Edge*> edges = m_view.selected_edges();
    QList<Edge_Style::Enabled_Styles> before;
    QList<Edge_Style::Enabled_Styles> after;
    for (Edge* edge : edges) {
        before.push_back(edge->style().enabled_style);
        after.push_back(value);
    }
    m_view.push_command(new Edge_Style_Enable(edges, before, after, &m_view));
}

void Knot_Style_Controller::set_selection_edge_slide(double value)
{
    QList<Edge*> edges = m_view.selected_edges();
    QList<double> before;
    QList<double> after;
    for (Edge* edge : edges) {
        before.push_back(edge->style().edge_slide);
        after.push_back(value);
    }
    m_view.push_command(new Edge_Style_Edge_Slide(edges, before, after, &m_view));
}
