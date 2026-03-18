/**
 * \file graph_renderer.cpp
 * \brief Internal graph rendering/traversal helper implementation.
 */

#include "graph_renderer.hpp"

#include <QDebug>
#include <QLineF>
#include <QPainterPathStroker>

#include "edge_type.hpp"
#include "edges_mark.hpp"
#include "faces.hpp"
#include "graph.hpp"
#include "node.hpp"
#include "path_builder.hpp"

void Graph_Renderer::render(Graph& graph)
{
    graph.paths.clear();
    Path_Builder path;
    traverse(graph, path);
    graph.paths = path.build();
    update_bounding_box(graph);

    graph.update_properties();

    std::vector<std::vector<std::size_t>> faces = find_faces(graph);
    GraphMarker marker;
    std::map<const Edge*, EdgeMarking> markings = marker.mark_graph(graph, faces);

    for (Edge* const e : graph.m_edges) {
        if (markings.count(e)) {
            const EdgeMarking& mark = markings.at(e);
            if (mark.marking == "a") {
                e->set_marking_color(Qt::red);
            } else if (mark.marking == "0") {
                e->set_marking_color(Qt::blue);
            } else {
                e->set_marking_color(Qt::black);
            }
        } else {
            e->set_marking_color(Qt::black);
        }
    }

    graph.update();
}
// logique de traversal qui modifie temporairement m_edges
void Graph_Renderer::traverse(Graph& graph, Path_Builder& path)
{
    QList<Edge*> traversed_edges;
    traversed_edges.reserve(graph.m_edges.size());

    for (QList<Edge*>::iterator i = graph.m_edges.begin(); i != graph.m_edges.end(); ++i) (*i)->reset();

    while (!graph.m_edges.empty()) {
        Edge* edge = graph.m_edges.front();
        Edge::Handle handle = edge->not_traversed();

        if (handle == Edge_Handle_Namespace::NO_HANDLE) {
            traversed_edges.push_back(edge);
            graph.m_edges.pop_front();
            continue;
        }

        path.new_group();

        int safety = 0;
        while (!edge->traversed(handle)) {
            if (++safety > 1000) {
                qWarning() << "Infinite loop detected in traverse!";
                break;
            }

            Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
            bool is_internal = (pure_handle & 0x0FF0);

            if (!is_internal) {
                edge->mark_traversed(handle);

                Traversal_Info ti = traverse(graph, edge, handle, path);
                if (!ti.success || !ti.out.edge) break;

                edge = ti.out.edge;
                handle = ti.out.handle;
                edge->mark_traversed(handle);
            } else {
                edge->mark_traversed(handle);
            }

            Edge_Type* edge_type = edge->effective_edge_type();
            Edge::Handle next_handle = edge_type->traverse(edge, handle, path);

            if (next_handle == Edge_Handle_Namespace::NO_HANDLE)
                next_handle = edge_type->Edge_Type::traverse(edge, handle, path);
            handle = next_handle;
        }
    }

#if HAS_QT_4_8
    graph.m_edges.swap(traversed_edges);
#else
    qSwap(graph.m_edges, traversed_edges);
#endif
}

Traversal_Info Graph_Renderer::traverse(Graph& graph, Edge* edge, Edge::Handle handle,
                                        Path_Builder& path)
{
    Traversal_Info ti;
    ti.in.edge = edge;
    ti.in.handle = handle;
    ti.strand = (int) ((handle & Edge_Handle_Namespace::STRAND_MASK) >> 12);
    ti.node = edge->vertex_for(handle);
    ti.in.angle = QLineF(ti.node->pos(), ti.in.edge->other(ti.node)->pos()).angle();
    edge->mark_traversed(handle);

    Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
    if (ti.in.edge->vertex1() == ti.node) {
        if (pure_handle == Edge_Handle_Namespace::TOP_LEFT)
            ti.handside = Traversal_Info::RIGHT;
        else if (pure_handle == Edge_Handle_Namespace::BOTTOM_LEFT)
            ti.handside = Traversal_Info::LEFT;
        else
            return Traversal_Info();
    } else if (ti.in.edge->vertex2() == ti.node) {
        if (pure_handle == Edge_Handle_Namespace::BOTTOM_RIGHT)
            ti.handside = Traversal_Info::RIGHT;
        else if (pure_handle == Edge_Handle_Namespace::TOP_RIGHT)
            ti.handside = Traversal_Info::LEFT;
        else
            return Traversal_Info();
    } else {
        return Traversal_Info();
    }

    ti.out.angle = ti.in.angle;
    ti.angle_delta = 360;
    ti.out.edge = ti.in.edge;
    for (Edge* const i : ti.node->edges()) {
        if (i != ti.in.edge) {
            double angle_out = QLineF(ti.node->pos(), i->other(ti.node)->pos()).angle();
            double delta = ti.in.angle - angle_out;
            if (delta < 0) delta += 360;
            if (ti.handside == Traversal_Info::RIGHT) delta = 360 - delta;
            if (delta < ti.angle_delta) {
                ti.angle_delta = delta;
                ti.out.edge = i;
                ti.out.angle = angle_out;
            }
        }
    }

    Edge_Handle strand_bit = (Edge_Handle) (handle & Edge_Handle_Namespace::STRAND_MASK);

    if (ti.out.edge->vertex1() == ti.node) {
        ti.out.handle = (Edge_Handle) ((ti.handside == Traversal_Info::RIGHT
                                            ? Edge_Handle_Namespace::BOTTOM_LEFT
                                            : Edge_Handle_Namespace::TOP_LEFT) |
                                       strand_bit);
    } else if (ti.out.edge->vertex2() == ti.node) {
        ti.out.handle = (Edge_Handle) ((ti.handside == Traversal_Info::RIGHT
                                            ? Edge_Handle_Namespace::TOP_RIGHT
                                            : Edge_Handle_Namespace::BOTTOM_RIGHT) |
                                       strand_bit);
    }

    ti.success = true;
    ti.node->style().build(ti, path, graph.m_appearance.default_node_style);

    return ti;
}

void Graph_Renderer::update_bounding_box(Graph& graph)
{
    graph.bounding_box = QRectF();
    if (!graph.paths.empty()) {
        for (const QPainterPath& pp : graph.paths) graph.bounding_box |= pp.controlPointRect();
        graph.bounding_box.adjust(-graph.width() / 2, -graph.width() / 2, graph.width() / 2,
                                  graph.width() / 2);
    }
}

QRectF Graph_Renderer::full_image_bounding_rect(const Graph& graph)
{
    QPainterPathStroker pps;
    pps.setCapStyle(graph.m_appearance.pen.capStyle());
    pps.setJoinStyle(graph.m_appearance.pen.joinStyle());
    pps.setMiterLimit(graph.m_appearance.pen.miterLimit());
    pps.setWidth(graph.m_appearance.pen.widthF());

    QRectF bb = graph.bounding_box;
    for (const QPainterPath& pp : graph.paths) bb |= pps.createStroke(pp).boundingRect();

    return bb;
}
