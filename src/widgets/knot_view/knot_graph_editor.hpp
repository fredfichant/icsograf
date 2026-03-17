/**
 * \file src/widgets/knot_view/knot_graph_editor.hpp
 * \brief Internal graph editing helper for Knot_View.
 */

#ifndef KNOT_GRAPH_EDITOR_HPP
#define KNOT_GRAPH_EDITOR_HPP

#include <QPointF>

class Edge;
class Knot_View;
class Node;

class Knot_Graph_Editor
{
   public:
    explicit Knot_Graph_Editor(Knot_View& view) : m_view(view) {}

    Node* add_node(QPointF pos);
    Node* add_breaking_node(QPointF pos);
    Edge* add_edge(Node* n1, Node* n2);
    void remove_edge(Edge* edge);
    void remove_node(Node* node);
    void update_knot();

   private:
    Knot_View& m_view;
};

#endif  // KNOT_GRAPH_EDITOR_HPP
