/**
 * \file src/widgets/knot_view/knot_view_io.cpp
 * \brief Internal I/O and clipboard helper for Knot_View.
 */

#include "knot_view_io.hpp"

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QFile>
#include <QMimeData>

#include "commands.hpp"
#include "knot_view.hpp"
#include "resource_manager.hpp"
#include "undo_manager.hpp"
#include "xml_exporter.hpp"
#include "xml_loader.hpp"

bool Knot_View_IO::load_file(QIODevice& device, QString action_name)
{
    Graph loaded;
    if (!import_xml(device, loaded)) return false;

    m_view.begin_macro(action_name);

    for (Node* n : m_view.m_graph.nodes()) {
        m_view.push_command(new Remove_Node(n, &m_view));
    }
    for (Edge* e : m_view.m_graph.edges()) {
        m_view.push_command(new Remove_Edge(e, &m_view));
    }

    m_view.push_command(new Knot_Width(m_view.m_graph.width(), loaded.width(), &m_view));
    m_view.push_command(new Change_Colors(m_view.m_graph.colors(), loaded.colors(), &m_view));
    m_view.push_command(
        new Custom_Colors(m_view.m_graph.custom_colors(), loaded.custom_colors(), &m_view));
    m_view.push_command(
        new Pen_Join_Style(m_view.m_graph.join_style(), loaded.join_style(), &m_view));
    m_view.push_command(
        new Brush_Style(m_view.m_graph.brush_style(), loaded.brush_style(), &m_view));
    m_view.push_command(new Knot_Style_All(m_view.m_graph.default_node_style(),
                                           loaded.default_node_style(),
                                           m_view.m_graph.default_edge_style(),
                                           loaded.default_edge_style(), &m_view));

    for (Node* n : loaded.nodes()) {
        m_view.push_command(new Create_Node(n, &m_view));
    }
    for (Edge* e : loaded.edges()) {
        m_view.push_command(new Create_Edge(e, &m_view));
    }

    m_view.end_macro();
    m_view.undo_stack_pointer()->setClean();
    m_view.view_fit();

    return true;
}

bool Knot_View_IO::load_file(QString file_name)
{
    if (!file_name.isEmpty()) {
        QFile file(file_name);
        if (load_file(file, QObject::tr("Load File"))) {
            m_view.setWindowFilePath(file_name);
            m_view.m_file_name = file_name;
            return true;
        }
    } else {
        resource_manager().settings.get_knot_style(m_view.m_graph);
    }
    return false;
}

bool Knot_View_IO::save_file(QString file_name)
{
    QFile file(file_name);
    if (export_xml(m_view.m_graph, file)) {
        m_view.setWindowFilePath(file_name);
        m_view.m_file_name = file_name;

        m_view.undo_stack_pointer()->setClean();
        return true;
    }
    return false;
}

void Knot_View_IO::copy_selection()
{
    Graph copy = m_view.m_graph.sub_graph(m_view.selected_nodes());
    QMimeData* mime_data = new QMimeData;
    export_xml_mime_data(mime_data, copy);
    QApplication::clipboard()->setMimeData(mime_data);
}

void Knot_View_IO::paste_selection()
{
    const QMimeData* mimeData = QApplication::clipboard()->mimeData();

    Graph graph;
    QByteArray clip_data;

    if (mimeData->hasFormat("application/x-knotter")) {
        clip_data = mimeData->data("application/x-knotter");
    } else if (mimeData->hasFormat("text/xml")) {
        clip_data = mimeData->data("text/xml");
    } else {
        return;
    }

    QBuffer read_data(&clip_data);

    if (!import_xml(read_data, graph)) return;

    if (!insert(graph, QObject::tr("Paste"))) {
        for (Node* n : graph.nodes()) delete n;
        for (Edge* e : graph.edges()) delete e;
    }
}

void Knot_View_IO::cut_selection()
{
    copy_selection();
    m_view.begin_macro(QObject::tr("Cut"));
    for (Node* n : m_view.selected_nodes()) m_view.remove_node(n);
    m_view.end_macro();
}

bool Knot_View_IO::insert(const Graph& graph, QString macro_name)
{
    if (graph.nodes().isEmpty() || m_view.mouse_mode & Knot_View::MOVE_NODES) return false;

    m_view.scene()->clearSelection();
    m_view.m_undoManager->pushMacro(new Knot_Insert_Macro(false, macro_name, &m_view));

    for (Node* n : graph.nodes()) {
        m_view.push_command(new Create_Node(n, &m_view));
        n->setSelected(true);
    }

    for (Edge* e : graph.edges()) {
        m_view.push_command(new Create_Edge(e, &m_view));
    }

    m_view.set_mouse_mode(m_view.mouse_mode | Knot_View::MOVE_NODES | Knot_View::EXTERNAL);

    m_view.node_mover.set_nodes(graph.nodes());
    m_view.node_mover.initialize_movement(graph.nodes().front()->pos());
    emit m_view.selection_changed(m_view.node_mover.nodes(), m_view.selected_edges());

    QPointF p = m_view.mapToScene(m_view.mapFromGlobal(QCursor::pos()));
    m_view.node_mover.set_pos(p);
    m_view.move_center = m_view.mapFromGlobal(QCursor::pos());

    m_view.end_macro();
    m_view.m_undoManager->pushMacro(new Knot_Insert_Macro(true, macro_name, &m_view));

    return true;
}
