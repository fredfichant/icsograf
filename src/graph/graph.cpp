#include "graph.hpp"

#include <QPaintEngine>

#include "edge_type.hpp"
#include "edges_mark.hpp"
#include "faces.hpp"
#include "resource_manager.hpp"

Graph::Graph()
    : m_default_node_style(225,  // cusp angle
                           24,   // handle length
                           32,   // cusp distance
                           resource_manager().default_cusp_shape(), Node_Style::EVERYTHING),
      m_default_edge_style(24,   // handle length
                           10,   // crossing distance
                           0.5,  // edge slide
                           resource_manager().default_edge_type(), Edge_Style::EVERYTHING,
                           10,  // spacing
                           1    // strand count
                           ),
      auto_color(true),
      m_properties(new Graph_Properties(nullptr))
{
    m_colors.push_back(Qt::black);
    set_join_style(Qt::RoundJoin);
    set_width(6);
    pen.setCapStyle(Qt::FlatCap);
    pen.setMiterLimit(128);
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

void Graph::copy_style(const Graph& other)
{
    m_colors = other.m_colors;
    m_default_node_style = other.m_default_node_style;
    m_default_edge_style = other.m_default_edge_style;
    auto_color = other.auto_color;
    pen = other.pen;
}

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

void Graph::set_colors(const QList<QColor>& l) { m_colors = l; }

void Graph::set_join_style(Qt::PenJoinStyle style) { pen.setJoinStyle(style); }

Qt::BrushStyle Graph::brush_style() const { return pen.brush().style(); }

void Graph::set_brush_style(Qt::BrushStyle s)
{
    QBrush b = pen.brush();
    b.setStyle(s);
    pen.setBrush(b);
}

void Graph::set_default_node_style(Node_Style style) { m_default_node_style = style; }

void Graph::set_default_edge_style(Edge_Style style) { m_default_edge_style = style; }

void Graph::set_width(double w) { pen.setWidthF(w); }

void Graph::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const_paint(painter, option, widget);
}

void Graph::const_paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) const
{
    if (!m_colors.empty()) {
        painter->setBrush(Qt::NoBrush);
        QPen p = pen;
        QBrush b = p.brush();
        if (!painter->paintEngine()->hasFeature(QPaintEngine::PatternBrush))
            b.setStyle(Qt::SolidPattern);

        for (int i = 0; i < paths.size(); ++i) {
            if (auto_color) {
                static const QList<QColor> loop_colors = {Qt::black, Qt::red,          Qt::green,
                                                          Qt::blue,  QColor("orange"), Qt::magenta};
                b.setColor(loop_colors[i % loop_colors.size()]);
            } else
                b.setColor(m_colors[i % m_colors.size()]);
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
void Graph::render_knot()
{
    paths.clear();
    Path_Builder path;
    traverse(path);
    paths = path.build();
    update_bounding_box();

    update_properties();

    // Step 4: reflect edge marking
    std::vector<std::vector<std::size_t>> faces = find_faces(*this);
    GraphMarker marker;
    std::map<const Edge*, EdgeMarking> markings = marker.mark_graph(*this, faces);

    for (Edge* const e : m_edges) {
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

    update();
}

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

void Graph::traverse(Path_Builder& path)
{
    QList<Edge*> traversed_edges;
    traversed_edges.reserve(m_edges.size());

    for (QList<Edge*>::iterator i = m_edges.begin(); i != m_edges.end(); ++i) (*i)->reset();

    // cycle while there are edges with untraversed handles
    while (!m_edges.empty()) {
        // pick first edge/handle
        Edge* edge = m_edges.front();
        Edge::Handle handle = edge->not_traversed();

        if (handle == Edge_Handle_Namespace::NO_HANDLE) {
            // removed completed edge
            traversed_edges.push_back(edge);
            m_edges.pop_front();
            continue;
        }

        // begin new knot thread
        path.new_group();

        // loop around a knotline loop item
        while (!edge->traversed(handle)) {
            Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
            bool is_internal = (pure_handle & 0x0FF0);

            if (!is_internal) {
                // external handle : hop to next edge through the node
                edge->mark_traversed(handle);

                Traversal_Info ti = traverse(edge, handle, path);
                if (!ti.success || !ti.out.edge) break;

                edge = ti.out.edge;
                handle = ti.out.handle;
                edge->mark_traversed(handle);
            } else {
                // internal handle : just mark traversed
                edge->mark_traversed(handle);
            }

            // draw + get next handle on the same edge (keeps STRAND bits)
            handle = edge->style().edge_type->traverse(edge, handle, path);
        }
    }

#if HAS_QT_4_8
    m_edges.swap(traversed_edges);
#else
    qSwap(m_edges, traversed_edges);
#endif
}
Traversal_Info Graph::traverse(Edge* edge, Edge::Handle handle, Path_Builder& path)
{
    // set input values
    Traversal_Info ti;
    ti.in.edge = edge;
    ti.in.handle = handle;
    ti.strand = (int) ((handle & Edge_Handle_Namespace::STRAND_MASK) >> 12);
    ti.node = edge->vertex_for(handle);
    ti.in.angle = QLineF(ti.node->pos(), ti.in.edge->other(ti.node)->pos()).angle();
    edge->mark_traversed(handle);

    // select handside (as viewed from the edge)
    Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
    if (ti.in.edge->vertex1() == ti.node) {
        // RH = TL,  LH = BL
        if (pure_handle == Edge_Handle_Namespace::TOP_LEFT)
            ti.handside = Traversal_Info::RIGHT;
        else if (pure_handle == Edge_Handle_Namespace::BOTTOM_LEFT)
            ti.handside = Traversal_Info::LEFT;
        else
            return Traversal_Info();  // Wrong handle
    } else if (ti.in.edge->vertex2() == ti.node) {
        // RH = BR,  LH = TR
        if (pure_handle == Edge_Handle_Namespace::BOTTOM_RIGHT)
            ti.handside = Traversal_Info::RIGHT;
        else if (pure_handle == Edge_Handle_Namespace::TOP_RIGHT)
            ti.handside = Traversal_Info::LEFT;
        else
            return Traversal_Info();  // Wrong handle
    } else
        return Traversal_Info();  // Wrong edge

    ti.out.angle = ti.in.angle;
    ti.angle_delta = 360;
    ti.out.edge = ti.in.edge;
    // select the next edge as the one with the smallest angle difference
    // angle direction is based on handside
    foreach (Edge* const i, ti.node->edges()) {
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

    // output values
    Edge_Handle strand_bit = (Edge_Handle) (handle & Edge_Handle_Namespace::STRAND_MASK);

    // select output handles based on handside
    if (ti.out.edge->vertex1() == ti.node) {
        // RH -> BL,  LH -> TL
        ti.out.handle = (Edge_Handle) ((ti.handside == Traversal_Info::RIGHT
                                            ? Edge_Handle_Namespace::BOTTOM_LEFT
                                            : Edge_Handle_Namespace::TOP_LEFT) |
                                       strand_bit);
    } else if (ti.out.edge->vertex2() == ti.node) {
        // RH -> TR,  LH -> BR
        ti.out.handle = (Edge_Handle) ((ti.handside == Traversal_Info::RIGHT
                                            ? Edge_Handle_Namespace::TOP_RIGHT
                                            : Edge_Handle_Namespace::BOTTOM_RIGHT) |
                                       strand_bit);
    }

    ti.success = true;

    ti.node->style().build(ti, path, m_default_node_style);

    return ti;
}

void Graph::update_bounding_box()
{
    bounding_box = QRectF();
    if (!paths.empty()) {
        foreach (const QPainterPath& pp, paths) bounding_box |= pp.controlPointRect();
        bounding_box.adjust(-width() / 2, -width() / 2, width() / 2, width() / 2);
    }
}

QRectF Graph::full_image_bounding_rect() const
{
    QPainterPathStroker pps;
    pps.setCapStyle(pen.capStyle());
    pps.setJoinStyle(pen.joinStyle());
    pps.setMiterLimit(pen.miterLimit());
    pps.setWidth(pen.widthF());

    QRectF bb = bounding_box;
    foreach (const QPainterPath& pp, paths) bb |= pps.createStroke(pp).boundingRect();

    return bb;
}

void Graph::update_properties() const
{
    m_properties->set_node_count(m_nodes.size());
    m_properties->set_edge_count(m_edges.size());
    m_properties->set_group_count(paths.size());

    // Calculate vertex degree distribution
    QMap<int, int> vertex_dist;
    for (Node* n : m_nodes) {
        int deg = n->edges().size();
        vertex_dist[deg] = vertex_dist.value(deg, 0) + 1;
    }
    m_properties->set_vertex_degree_distribution(vertex_dist);

    // Calculate faces and face degree distribution
    std::vector<std::vector<std::size_t>> faces = find_faces(*this);
    m_properties->set_face_count(faces.size());

    QMap<int, int> face_dist;
    for (const auto& face : faces) {
        int deg = face.size();
        face_dist[deg] = face_dist.value(deg, 0) + 1;
    }
    m_properties->set_face_degree_distribution(face_dist);

    // Calculate edge distribution (wa, w0, p0, pa)
    GraphMarker marker;
    std::map<const Edge*, EdgeMarking> markings = marker.mark_graph(*this, faces);

    int wa = 0, w0 = 0, p0 = 0, pa = 0;
    for (auto const& [edge, mark] : markings) {
        (void) edge;
        if (mark.marking == "wa")
            wa++;
        else if (mark.marking == "w0")
            w0++;
        else if (mark.marking == "p0")
            p0++;
        else if (mark.marking == "pa")
            pa++;
    }
    m_properties->set_edge_distribution(wa, w0, p0, pa);
}
