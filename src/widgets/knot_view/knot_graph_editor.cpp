/**
 * \file src/widgets/knot_view/knot_graph_editor.cpp
 * \brief Internal graph editing helper for Knot_View.
 */

#include "knot_graph_editor.hpp"

#include <QGraphicsScene>

#include "commands.hpp"
#include "edge.hpp"
#include "knot_view.hpp"
#include "node.hpp"
#include "resource_manager.hpp"

Node* Knot_Graph_Editor::add_node(QPointF pos)
{
    Node* node = new Node(pos);
    m_view.push_command(new Create_Node(node, &m_view));
    return node;
}

Node* Knot_Graph_Editor::add_breaking_node(QPointF pos)
{
    Edge* edge = m_view.edge_at(pos);
    if (!edge) return add_node(pos);

    Node* v1 = edge->vertex1();
    Node* v2 = edge->vertex2();

    m_view.begin_macro(QObject::tr("Break Edge"));
    Node* new_node = add_node(edge->snap(pos));
    add_edge(v1, new_node);
    add_edge(new_node, v2);
    remove_edge(edge);
    m_view.end_macro();

    return new_node;
}

Edge* Knot_Graph_Editor::add_edge(Node* n1, Node* n2)
{
    Edge* edge = new Edge(n1, n2, resource_manager().default_edge_type());
    m_view.push_command(new Create_Edge(edge, &m_view));
    return edge;
}

void Knot_Graph_Editor::remove_edge(Edge* edge)
{
    m_view.push_command(new Remove_Edge(edge, &m_view));
}

void Knot_Graph_Editor::remove_node(Node* node)
{
    m_view.begin_macro(QObject::tr("Remove Node"));
    for (Edge* edge : node->edges()) {
        if (edge->scene() == m_view.scene()) remove_edge(edge);
    }
    m_view.push_command(new Remove_Node(node, &m_view));
    m_view.end_macro();
}

void Knot_Graph_Editor::update_knot()
{
    m_view.m_graph.render_knot();
    m_view.scene()->invalidate();
}
