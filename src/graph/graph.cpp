/**
 * \file src/graph/graph.cpp
 * \brief Implementation of graph logic.
 */

#include "graph.hpp"

#include <QPaintEngine>

#include "graph_analyzer.hpp"
#include "edge.hpp"
#include "graph_properties.hpp"
#include "graph_renderer.hpp"
#include "node.hpp"
#include "path_builder.hpp"
#include "resource_manager.hpp"

Graph::Graph()
    : m_appearance(
          Node_Style(225,  // cusp angle
                     24,   // handle length
                     32,   // cusp distance
                     resource_manager().default_cusp_shape(), Node_Style::EVERYTHING),
          Edge_Style(24,   // handle length
                     10,   // crossing distance
                     0.5,  // edge slide
                     resource_manager().default_edge_type(), Edge_Style::EVERYTHING,
                     10,  // spacing
                     1    // strand count
                     )),
      m_properties(new Graph_Properties(nullptr))
{
    m_appearance.colors.push_back(Qt::black);
    set_join_style(Qt::RoundJoin);
    set_width(6);
    m_appearance.pen.setCapStyle(Qt::FlatCap);
    m_appearance.pen.setMiterLimit(128);
}

Graph::Graph(const Graph& other) : QGraphicsItem(), m_properties(new Graph_Properties(nullptr))
{
    *this = other;
}

Graph& Graph::operator=(const Graph& o)
{
    m_edges = o.m_edges;
    m_nodes = o.m_nodes;
    bounding_box = o.bounding_box;
    paths = o.paths;
    copy_style(o);
    setPos(o.pos());
    setTransform(o.transform());
    setVisible(o.isVisible());
    setCacheMode(o.cacheMode());
    return *this;
}

Graph::~Graph() { delete m_properties; }

void Graph::copy_style(const Graph& other) { m_appearance = other.m_appearance; }

void Graph::add_node(Node* n) { m_nodes.append(n); }

void Graph::add_edge(Edge* e)
{
    m_edges.append(e);
    e->attach();
    e->set_graph(this);
}

void Graph::remove_node(Node* n)
{
    m_nodes.removeOne(n);
    // n->setParentItem(nullptr);
}

void Graph::remove_edge(Edge* e)
{
    m_edges.removeOne(e);
    e->detach();
    e->set_graph(nullptr);
    // e->setParentItem(nullptr);
}

void Graph::set_colors(const QList<QColor>& l) { m_appearance.colors = l; }

void Graph::set_join_style(Qt::PenJoinStyle style) { m_appearance.pen.setJoinStyle(style); }

Qt::BrushStyle Graph::brush_style() const { return m_appearance.pen.brush().style(); }

void Graph::set_brush_style(Qt::BrushStyle s)
{
    QBrush b = m_appearance.pen.brush();
    b.setStyle(s);
    m_appearance.pen.setBrush(b);
}

void Graph::set_default_node_style(Node_Style style) { m_appearance.default_node_style = style; }

void Graph::set_default_edge_style(Edge_Style style) { m_appearance.default_edge_style = style; }

void Graph::set_width(double w) { m_appearance.pen.setWidthF(w); }

void Graph::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const_paint(painter, option, widget);
}

void Graph::const_paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) const
{
    if (!m_appearance.colors.empty()) {
        painter->setBrush(Qt::NoBrush);
        QPen p = m_appearance.pen;
        QBrush b = p.brush();
        if (!painter->paintEngine()->hasFeature(QPaintEngine::PatternBrush))
            b.setStyle(Qt::SolidPattern);

        for (int i = 0; i < paths.size(); ++i) {
            if (m_appearance.auto_color) {
                static const QList<QColor> loop_colors = {Qt::black, Qt::red,          Qt::green,
                                                          Qt::blue,  QColor("orange"), Qt::magenta};
                b.setColor(loop_colors[i % loop_colors.size()]);
            } else {
                b.setColor(m_appearance.colors[i % m_appearance.colors.size()]);
            }
            p.setBrush(b);
            painter->setPen(p);
            painter->drawPath(paths[i]);
        }
    }
}

void Graph::paint_graph(QPainter* painter, const QStyleOptionGraphicsItem* option,
                        QWidget* widget) const
{
    foreach (Edge* const e, m_edges) e->paint(painter, option, widget);
    foreach (Node* const n, m_nodes) {
        painter->translate(n->pos());
        n->paint(painter, option, widget);
        painter->translate(-n->pos());
    }
}

void Graph::render_knot() { Graph_Renderer::render(*this); }

Graph Graph::sub_graph(QList<Node*> nodes) const
{
    Graph graph(*this);
    graph.m_nodes.clear();
    graph.m_edges.clear();

    graph.m_nodes.reserve(nodes.size());
    graph.m_edges.reserve(nodes.size());

    foreach (Node* const n, nodes) {
        graph.m_nodes.push_back(n);

        foreach (Edge* const e, n->edges()) {
            if (nodes.contains(e->other(n)) && !graph.m_edges.contains(e)) {
                graph.m_edges.push_back(e);
            }
        }
    }

    graph.render_knot();

    return graph;
}

void Graph::enable_cache(bool enable) { setCacheMode(enable ? DeviceCoordinateCache : NoCache); }

bool Graph::cache_enabled() const { return cacheMode() != NoCache; }

void Graph::traverse(Path_Builder& path) { Graph_Renderer::traverse(*this, path); }

Traversal_Info Graph::traverse(Edge* edge, Edge::Handle handle, Path_Builder& path)
{
    return Graph_Renderer::traverse(*this, edge, handle, path);
}

void Graph::update_bounding_box() { Graph_Renderer::update_bounding_box(*this); }

QRectF Graph::full_image_bounding_rect() const
{
    return Graph_Renderer::full_image_bounding_rect(*this);
}

void Graph::update_properties() const { Graph_Analyzer::update(*this); }
