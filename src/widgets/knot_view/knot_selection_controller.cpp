/**
 * \file src/widgets/knot_view/knot_selection_controller.cpp
 * \brief Internal selection and selection-editing helper for Knot_View.
 */

#include "knot_selection_controller.hpp"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "commands.hpp"
#include "edge.hpp"
#include "knot_view.hpp"
#include "node.hpp"

QList<Node*> Knot_Selection_Controller::selected_nodes() const
{
    QList<Node*> nodes;
    for (QGraphicsItem* item : m_view.scene()->selectedItems()) {
        Node* node = dynamic_cast<Node*>(item);
        if (node) nodes.push_back(node);
    }
    return nodes;
}

QList<Edge*> Knot_Selection_Controller::selected_edges() const
{
    QList<Edge*> edges;
    for (QGraphicsItem* item : m_view.scene()->selectedItems()) {
        Edge* edge = dynamic_cast<Edge*>(item);
        if (edge) edges.push_back(edge);
    }
    return edges;
}

void Knot_Selection_Controller::update_selection(bool select_edges)
{
    m_view.node_mover.set_nodes(selected_nodes());

    if (select_edges) {
        for (Edge* edge : m_view.m_graph.edges()) {
            edge->setSelected(edge->vertex1()->isSelected() && edge->vertex2()->isSelected());
        }
    }

    emit m_view.selection_changed(m_view.node_mover.nodes(), selected_edges());
}

void Knot_Selection_Controller::rubberband_select(QList<Node*> nodes, bool modifier)
{
    bool select = true;

    if (!modifier) {
        m_view.scene()->clearSelection();
    } else {
        select = false;
        for (Node* item : nodes) {
            if (!item->isSelected()) {
                select = true;
                break;
            }
        }
    }

    for (Node* item : nodes) {
        item->setSelected(select);
        for (Edge* edge : item->edges()) {
            edge->setSelected(select && edge->other(item)->isSelected());
        }
    }

    update_selection(false);
}

QList<Node*> Knot_Selection_Controller::nodes_in_rubberband() const
{
    QList<QGraphicsItem*> items =
        m_view.scene()->items(QRectF(m_view.rubberband.rect().translated(m_view.rubberband.pos())));
    QList<Node*> nodes;
    for (QGraphicsItem* item : items) {
        Node* node = dynamic_cast<Node*>(item);
        if (node) nodes.push_back(node);
    }
    return nodes;
}

void Knot_Selection_Controller::select_all()
{
    m_view.scene()->clearSelection();
    for (Node* const node : m_view.m_graph.nodes()) node->setSelected(true);

    update_selection();
}

void Knot_Selection_Controller::select_connected()
{
    QList<Node*> nodes = selected_nodes();
    while (!nodes.empty()) {
        Node* n1 = nodes.front();
        nodes.pop_front();
        for (Edge* edge : n1->edges()) {
            Node* n2 = edge->other(n1);
            if (!n2->isSelected()) {
                n2->setSelected(true);
                nodes.push_back(n2);
            }
        }
    }
    update_selection();
}

void Knot_Selection_Controller::snap_selection_to_grid()
{
    m_view.begin_macro(QObject::tr("Snap to Grid"));
    for (Node* node : selected_nodes()) {
        m_view.push_command(
            new Move_Node(node, node->pos(), m_view.m_grid.nearest(node->pos()), &m_view));
    }

    m_view.end_macro();
}

void Knot_Selection_Controller::erase_selection()
{
    QList<Node*> nodes = selected_nodes();
    if (!nodes.empty()) {
        m_view.begin_macro(QObject::tr("Delete"));
        for (Node* node : nodes) m_view.remove_node(node);
        m_view.end_macro();
    }
}

void Knot_Selection_Controller::connect_selection()
{
    QList<Node*> nodes = selected_nodes();

    if (nodes.size() < 2) return;

    m_view.begin_macro(QObject::tr("Connect Nodes"));

    for (QList<Node*>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
        for (QList<Node*>::iterator j = i + 1; j != nodes.end(); ++j) {
            if (!(*i)->has_edge_to(*j)) m_view.add_edge(*i, *j);
        }
    }
    m_view.end_macro();
}

void Knot_Selection_Controller::disconnect_selection()
{
    QList<Node*> nodes = selected_nodes();

    if (nodes.size() < 2) return;

    m_view.begin_macro(QObject::tr("Disconnect Nodes"));
    for (QList<Node*>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
        for (QList<Node*>::iterator j = i + 1; j != nodes.end(); ++j) {
            if ((*i)->has_edge_to(*j)) m_view.remove_edge((*i)->edge_to(*j));
        }
    }
    m_view.end_macro();
}

void Knot_Selection_Controller::merge_selection()
{
    QList<Node*> nodes = selected_nodes();

    if (nodes.size() < 2) return;

    m_view.begin_macro(QObject::tr("Merge Nodes"));

    QPointF pos;
    QList<Node*> outlinks;
    QList<Edge_Type*> outlinks_styles;

    for (QList<Node*>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
        pos += (*i)->pos() / nodes.size();
        for (Edge* edge : (*i)->edges()) {
            if (!nodes.contains(edge->other(*i)) && !outlinks.contains(edge->other(*i))) {
                outlinks.push_back(edge->other(*i));
                outlinks_styles.push_back(edge->style().edge_type);
            }
        }
        m_view.remove_node(*i);
    }

    Node* new_node = m_view.add_node(pos);
    for (int i = 0; i < outlinks.size(); ++i) {
        m_view.push_command(new Create_Edge(new Edge(new_node, outlinks[i], outlinks_styles[i]), &m_view));
    }

    m_view.end_macro();
}

void Knot_Selection_Controller::flip_horiz_selection()
{
    QList<Node*> nodes = selected_nodes();
    double center = 0;
    for (Node* node : nodes) {
        center += node->x() / nodes.size();
    }
    m_view.begin_macro(QObject::tr("Horizontal Flip"));
    for (Node* node : nodes) {
        m_view.push_command(
            new Move_Node(node, node->pos(), QPointF(center - (node->x() - center), node->y()), &m_view));
    }

    m_view.end_macro();
}

void Knot_Selection_Controller::flip_vert_selection()
{
    QList<Node*> nodes = selected_nodes();
    double center = 0;
    for (Node* node : nodes) {
        center += node->y() / nodes.size();
    }
    m_view.begin_macro(QObject::tr("Vertical Flip"));
    for (Node* node : nodes) {
        m_view.push_command(
            new Move_Node(node, node->pos(), QPointF(node->x(), center - (node->y() - center)), &m_view));
    }

    m_view.end_macro();
}
