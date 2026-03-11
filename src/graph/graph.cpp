/**
 * \file src/graph/graph.cpp
 * \brief Implementation of graph graph logic.
 */

#include "graph.hpp"

#include <algorithm>
#include <cctype>
#include <QPaintEngine>
#include <QDebug>

#include "edge_type.hpp"
#include "edge_type_utils.hpp"
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
        // qDebug() << "--- START NEW STRAND ---";

        // loop around a knotline loop item
        int safety = 0;
        while (!edge->traversed(handle)) {
            if (++safety > 1000) {
                qWarning() << "Infinite loop detected in traverse!";
                break;
            }
            // qDebug() << "Current Pos: Edge" << edge << "Handle" << handleToString(handle);

            Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
            bool is_internal = (pure_handle & 0x0FF0);

            if (!is_internal) {
                // external handle : hop to next edge through the node
                edge->mark_traversed(handle);

                Traversal_Info ti = traverse(edge, handle, path);
                if (!ti.success || !ti.out.edge) {
                    // qDebug() << "  -> Node traversal failed (End of line or Error)";
                    break;
                }

                edge = ti.out.edge;
                handle = ti.out.handle;
                // qDebug() << "  -> Jumped Node to: Edge" << edge << "Handle" << handleToString(handle);
                edge->mark_traversed(handle);
            } else {
                // internal handle : just mark traversed
                edge->mark_traversed(handle);
            }

            // draw + get next handle on the same edge (keeps STRAND bits)
            Edge::Handle next_handle = edge->style().edge_type->traverse(edge, handle, path);

            if (next_handle == Edge_Handle_Namespace::NO_HANDLE) {
                // Fallback : si le type spécifique échoue (ex: bug sur BL->BR), on force le passage via l'implémentation de base
                next_handle = edge->style().edge_type->Edge_Type::traverse(edge, handle, path);
            }
            handle = next_handle;
            // qDebug() << "  -> Internal Traverse to:" << handleToString(handle);
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
    const auto edge_weight = [](const Edge* e) {
        const Edge_Type* type = e->style().edge_type;
        if (!type) return 1;
        const QString name = type->machine_name();
        if (name.startsWith("2strand")) return 2;
        if (name.startsWith("3strand")) return 3;
        return 1;
    };
    const auto edge_is_inverted = [](const Edge* e) {
        const Edge_Type* type = e->style().edge_type;
        return type && type->machine_name().contains("inverted");
    };

    m_properties->set_node_count(m_nodes.size());
    int weighted_edge_count = 0;
    for (Edge* e : m_edges) weighted_edge_count += edge_weight(e);
    m_properties->set_edge_count(weighted_edge_count);
    m_properties->set_group_count(paths.size());

    // Calculate vertex degree distribution
    QMap<int, int> vertex_dist;
    for (Node* n : m_nodes) {
        int deg = 0;
        for (const Edge* e : n->edges()) {
            deg += edge_weight(e);
        }
        vertex_dist[deg] = vertex_dist.value(deg, 0) + 1;
    }
    m_properties->set_vertex_degree_distribution(vertex_dist);

    // Calculate faces and face degree distribution
    std::vector<std::vector<std::size_t>> faces = find_faces(*this);
    int face_adjustment = 0;
    for (Edge* e : m_edges) {
        face_adjustment += edge_weight(e) - 1;
    }
    m_properties->set_face_adjustment(face_adjustment);
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
        const int weight = edge_weight(edge);
        const bool inverted = edge_is_inverted(edge);
        const std::string& m = mark.marking;

        if (m == "wa") {
            wa += weight;
            continue;
        }
        if (m == "w0") {
            w0 += weight;
            continue;
        }
        if (m == "pa") {
            pa += weight;
            continue;
        }
        if (m == "p0") {
            p0 += weight;
            continue;
        }

        // Current GraphMarker output uses "a"/"0" (possibly comma-separated).
        std::size_t start = 0;
        while (start <= m.size()) {
            const std::size_t comma = m.find(',', start);
            const std::string token =
                m.substr(start, comma == std::string::npos ? std::string::npos : comma - start);
            if (!token.empty()) {
                const unsigned char c = static_cast<unsigned char>(token[0]);
                const bool is_a = std::isalpha(c) != 0;
                const bool is_0 = std::isdigit(c) != 0;
                if (is_a) {
                    if (inverted)
                        pa += weight;
                    else
                        wa += weight;
                } else if (is_0) {
                    if (inverted)
                        p0 += weight;
                    else
                        w0 += weight;
                }
            }

            if (comma == std::string::npos) break;
            start = comma + 1;
        }
    }
    m_properties->set_edge_distribution(wa, w0, p0, pa);

    enum class Crossing_Event
    {
        NONE,
        OVER,
        UNDER
    };

    const auto same_pair = [](Edge_Handle a, Edge_Handle b, Edge_Handle x, Edge_Handle y) {
        return (a == x && b == y) || (a == y && b == x);
    };

    const auto crossing_event = [&](const Edge* edge, Edge_Handle in, Edge_Handle out) {
        const Edge_Handle a = (Edge_Handle) (in & Edge_Handle_Namespace::HANDLE_MASK);
        const Edge_Handle b = (Edge_Handle) (out & Edge_Handle_Namespace::HANDLE_MASK);
        const QString type_name = edge->style().edge_type ? edge->style().edge_type->machine_name()
                                                          : QString("regular");
        const bool inverted = type_name.contains("inverted");

        if (type_name.startsWith("3strand")) {
            const bool top_a = same_pair(a, b, Edge_Handle_Namespace::TOP_LEFT,
                                         Edge_Handle_Namespace::MID_TOP_RIGHT);
            const bool top_b = same_pair(a, b, Edge_Handle_Namespace::TOP_RIGHT,
                                         Edge_Handle_Namespace::MID_TOP_LEFT);
            if (top_a || top_b) {
                const bool top_over = !inverted ? top_a : top_b;
                return top_over ? Crossing_Event::OVER : Crossing_Event::UNDER;
            }

            const bool center_a = same_pair(a, b, Edge_Handle_Namespace::CENTER_TOP_LEFT,
                                            Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT);
            const bool center_b = same_pair(a, b, Edge_Handle_Namespace::CENTER_TOP_RIGHT,
                                            Edge_Handle_Namespace::CENTER_BOTTOM_LEFT);
            if (center_a || center_b) {
                const bool center_over = !inverted ? center_a : center_b;
                return center_over ? Crossing_Event::OVER : Crossing_Event::UNDER;
            }

            const bool bottom_a = same_pair(a, b, Edge_Handle_Namespace::MID_BOTTOM_LEFT,
                                            Edge_Handle_Namespace::BOTTOM_RIGHT);
            const bool bottom_b = same_pair(a, b, Edge_Handle_Namespace::MID_BOTTOM_RIGHT,
                                            Edge_Handle_Namespace::BOTTOM_LEFT);
            if (bottom_a || bottom_b) {
                const bool bottom_over = !inverted ? bottom_a : bottom_b;
                return bottom_over ? Crossing_Event::OVER : Crossing_Event::UNDER;
            }
            return Crossing_Event::NONE;
        }

        if (type_name.startsWith("2strand")) {
            const bool top_a = same_pair(a, b, Edge_Handle_Namespace::TOP_LEFT,
                                         Edge_Handle_Namespace::MID_TOP_RIGHT);
            const bool top_b = same_pair(a, b, Edge_Handle_Namespace::TOP_RIGHT,
                                         Edge_Handle_Namespace::MID_TOP_LEFT);
            if (top_a || top_b) {
                const bool top_over = !inverted ? top_a : top_b;
                return top_over ? Crossing_Event::OVER : Crossing_Event::UNDER;
            }

            const bool bottom_a = same_pair(a, b, Edge_Handle_Namespace::MID_BOTTOM_LEFT,
                                            Edge_Handle_Namespace::BOTTOM_RIGHT);
            const bool bottom_b = same_pair(a, b, Edge_Handle_Namespace::MID_BOTTOM_RIGHT,
                                            Edge_Handle_Namespace::BOTTOM_LEFT);
            if (bottom_a || bottom_b) {
                const bool bottom_over = !inverted ? bottom_a : bottom_b;
                return bottom_over ? Crossing_Event::OVER : Crossing_Event::UNDER;
            }
            return Crossing_Event::NONE;
        }

        const bool diag_a = same_pair(a, b, Edge_Handle_Namespace::TOP_LEFT,
                                      Edge_Handle_Namespace::BOTTOM_RIGHT);
        const bool diag_b = same_pair(a, b, Edge_Handle_Namespace::TOP_RIGHT,
                                      Edge_Handle_Namespace::BOTTOM_LEFT);
        if (diag_a || diag_b) {
            const bool over = !inverted ? diag_a : diag_b;
            return over ? Crossing_Event::OVER : Crossing_Event::UNDER;
        }

        return Crossing_Event::NONE;
    };

    const auto circular_lexicographic_word = [](const std::vector<int>& spans) {
        if (spans.empty()) return QString("0");
        const int n = (int) spans.size();
        int best = 0;
        for (int i = 1; i < n; ++i) {
            bool better = false;
            bool worse = false;
            for (int k = 0; k < n; ++k) {
                const int lhs = spans[(i + k) % n];
                const int rhs = spans[(best + k) % n];
                if (lhs < rhs) {
                    better = true;
                    break;
                }
                if (lhs > rhs) {
                    worse = true;
                    break;
                }
            }
            if (better && !worse) best = i;
        }

        QString out;
        for (int k = 0; k < n; ++k) out += QString::number(spans[(best + k) % n]);
        return out.isEmpty() ? QString("0") : out;
    };

    const auto node_jump = [](Edge* in_edge, Edge::Handle in_handle, Edge*& out_edge,
                              Edge::Handle& out_handle) {
        Node* node = in_edge->vertex_for(in_handle);
        if (!node) return false;

        const double in_angle = QLineF(node->pos(), in_edge->other(node)->pos()).angle();
        const Edge_Handle pure_handle = (Edge_Handle) (in_handle & Edge_Handle_Namespace::HANDLE_MASK);

        Traversal_Info::handside_type handside = Traversal_Info::LEFT;
        if (in_edge->vertex1() == node) {
            if (pure_handle == Edge_Handle_Namespace::TOP_LEFT)
                handside = Traversal_Info::RIGHT;
            else if (pure_handle == Edge_Handle_Namespace::BOTTOM_LEFT)
                handside = Traversal_Info::LEFT;
            else
                return false;
        } else if (in_edge->vertex2() == node) {
            if (pure_handle == Edge_Handle_Namespace::BOTTOM_RIGHT)
                handside = Traversal_Info::RIGHT;
            else if (pure_handle == Edge_Handle_Namespace::TOP_RIGHT)
                handside = Traversal_Info::LEFT;
            else
                return false;
        } else {
            return false;
        }

        out_edge = in_edge;
        double best_delta = 360.0;
        for (Edge* candidate : node->edges()) {
            if (candidate == in_edge) continue;
            const double out_angle = QLineF(node->pos(), candidate->other(node)->pos()).angle();
            double delta = in_angle - out_angle;
            if (delta < 0) delta += 360.0;
            if (handside == Traversal_Info::RIGHT) delta = 360.0 - delta;
            if (delta < best_delta) {
                best_delta = delta;
                out_edge = candidate;
            }
        }

        const Edge_Handle strand_bit = (Edge_Handle) (in_handle & Edge_Handle_Namespace::STRAND_MASK);
        if (out_edge->vertex1() == node) {
            out_handle = (Edge_Handle) ((handside == Traversal_Info::RIGHT
                                             ? Edge_Handle_Namespace::BOTTOM_LEFT
                                             : Edge_Handle_Namespace::TOP_LEFT) |
                                        strand_bit);
            return true;
        }
        if (out_edge->vertex2() == node) {
            out_handle = (Edge_Handle) ((handside == Traversal_Info::RIGHT
                                             ? Edge_Handle_Namespace::TOP_RIGHT
                                             : Edge_Handle_Namespace::BOTTOM_RIGHT) |
                                        strand_bit);
            return true;
        }
        return false;
    };

    const auto spans_from_events = [](const std::vector<Crossing_Event>& events) {
        std::vector<int> spans;
        if (events.empty()) {
            spans.push_back(0);
            return spans;
        }

        std::vector<int> under_positions;
        under_positions.reserve(events.size());
        int over_without_under = 0;
        for (int i = 0; i < (int) events.size(); ++i) {
            if (events[i] == Crossing_Event::UNDER) {
                under_positions.push_back(i);
            } else if (events[i] == Crossing_Event::OVER) {
                over_without_under++;
            }
        }

        if (under_positions.empty()) {
            spans.push_back(over_without_under);
            return spans;
        }

        for (int start : under_positions) {
            int count = 0;
            int idx = (start + 1) % (int) events.size();
            while (idx != start && events[idx] != Crossing_Event::UNDER) {
                if (events[idx] == Crossing_Event::OVER) count++;
                idx = (idx + 1) % (int) events.size();
            }
            spans.push_back(count);
        }
        return spans;
    };

    for (Edge* e : m_edges) e->reset();

    Path_Builder sink;
    std::vector<QString> component_words;
    while (true) {
        Edge* edge = nullptr;
        Edge::Handle handle = Edge_Handle_Namespace::NO_HANDLE;
        for (Edge* candidate : m_edges) {
            const Edge::Handle candidate_handle = candidate->not_traversed();
            if (candidate_handle != Edge_Handle_Namespace::NO_HANDLE) {
                edge = candidate;
                handle = candidate_handle;
                break;
            }
        }
        if (!edge || handle == Edge_Handle_Namespace::NO_HANDLE) break;

        std::vector<Crossing_Event> events;
        int safety = 0;
        while (!edge->traversed(handle)) {
            if (++safety > 5000) break;

            const Edge_Handle pure_handle = (Edge_Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
            const bool is_internal = (pure_handle & 0x0FF0) != 0;

            if (!is_internal) {
                edge->mark_traversed(handle);

                Edge* jumped_edge = edge;
                Edge::Handle jumped_handle = handle;
                if (!node_jump(edge, handle, jumped_edge, jumped_handle)) break;

                edge = jumped_edge;
                handle = jumped_handle;
                edge->mark_traversed(handle);
            } else {
                edge->mark_traversed(handle);
            }

            Edge::Handle next_handle = edge->style().edge_type->traverse(edge, handle, sink);
            if (next_handle == Edge_Handle_Namespace::NO_HANDLE)
                next_handle = edge->style().edge_type->Edge_Type::traverse(edge, handle, sink);
            if (next_handle == Edge_Handle_Namespace::NO_HANDLE) break;

            events.push_back(crossing_event(edge, handle, next_handle));
            handle = next_handle;
        }

        component_words.push_back(circular_lexicographic_word(spans_from_events(events)));
    }

    QString span_formula_value("0");
    if (!component_words.empty()) {
        std::sort(component_words.begin(), component_words.end());
        QString joined;
        for (std::size_t i = 0; i < component_words.size(); ++i) {
            if (i != 0) joined += "|";
            joined += component_words[i];
        }
        span_formula_value = joined;
    }
    m_properties->set_span_formula(span_formula_value);

    int pm = 0;
    for (QChar c : span_formula_value) {
        if (c.isDigit()) pm = std::max(pm, c.digitValue());
    }

    const int r = m_properties->group_count();
    const int c = m_properties->edge_count();
    bool is_non_reducible = false;
    if (r <= pm + 1) {
        is_non_reducible = c >= (5 * pm - r - 1);
    } else {
        is_non_reducible = c >= (2 * (pm + r - 2));
    }
    m_properties->set_is_non_reducible(is_non_reducible);
}
