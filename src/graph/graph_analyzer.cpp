/**
 * \file graph_analyzer.cpp
 * \brief Internal graph analysis helper implementation.
 */

#include "graph_analyzer.hpp"

#include <algorithm>
#include <vector>

#include <QLineF>
#include <QMap>

#include "edge.hpp"
#include "edge_type.hpp"
#include "edges_mark.hpp"
#include "faces.hpp"
#include "graph_properties.hpp"
#include "graph.hpp"
#include "node.hpp"
#include "path_builder.hpp"
#include "traversal_info.hpp"

void Graph_Analyzer::update(const Graph& graph)
{
    const auto edge_weight = [](const Edge* e) {
        const Edge_Type* type = e->effective_edge_type();
        if (!type) return 1;
        const QString name = type->machine_name();
        if (name.startsWith("2strand")) return 2;
        if (name.startsWith("3strand")) return 3;
        return 1;
    };
    graph.m_properties->set_node_count(graph.m_nodes.size());
    int weighted_edge_count = 0;
    for (Edge* e : graph.m_edges) weighted_edge_count += edge_weight(e);
    graph.m_properties->set_edge_count(weighted_edge_count);
    graph.m_properties->set_group_count(graph.paths.size());

    QMap<int, int> vertex_dist;
    for (Node* n : graph.m_nodes) {
        int deg = 0;
        for (const Edge* e : n->edges()) deg += edge_weight(e);
        vertex_dist[deg] = vertex_dist.value(deg, 0) + 1;
    }
    graph.m_properties->set_vertex_degree_distribution(vertex_dist);

    std::vector<std::vector<std::size_t>> faces = find_faces(graph);
    int face_adjustment = 0;
    for (Edge* e : graph.m_edges) face_adjustment += edge_weight(e) - 1;
    graph.m_properties->set_face_adjustment(face_adjustment);
    graph.m_properties->set_face_count(faces.size());

    QMap<int, int> face_dist;
    for (const auto& face : faces) {
        int deg = face.size();
        face_dist[deg] = face_dist.value(deg, 0) + 1;
    }
    graph.m_properties->set_face_degree_distribution(face_dist);

    GraphMarker marker;
    const std::vector<EdgeDistributionTable> edge_tables = marker.edge_distribution_tables(graph, faces);
    if (!edge_tables.empty()) {
        const EdgeDistributionTable& first = edge_tables.front();
        graph.m_properties->set_edge_distribution(first.wa, first.w0, first.p0, first.pa);
    } else {
        graph.m_properties->set_edge_distribution(0, 0, 0, 0);
    }
    graph.m_properties->set_edge_distribution_tables(edge_tables);

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
        const QString type_name =
            edge->effective_edge_type() ? edge->effective_edge_type()->machine_name()
                                        : QString("regular");
        const bool inverted = type_name.contains("inverted");

        if (type_name.startsWith("3strand")) {
            const bool top_a = same_pair(a, b, Edge_Handle_Namespace::TOP_LEFT,
                                         Edge_Handle_Namespace::MID_TOP_RIGHT);
            const bool top_b = same_pair(a, b, Edge_Handle_Namespace::TOP_RIGHT,
                                         Edge_Handle_Namespace::MID_TOP_LEFT);
            if (top_a || top_b) return (!inverted ? top_a : top_b) ? Crossing_Event::OVER
                                                                   : Crossing_Event::UNDER;

            const bool center_a = same_pair(a, b, Edge_Handle_Namespace::CENTER_TOP_LEFT,
                                            Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT);
            const bool center_b = same_pair(a, b, Edge_Handle_Namespace::CENTER_TOP_RIGHT,
                                            Edge_Handle_Namespace::CENTER_BOTTOM_LEFT);
            if (center_a || center_b)
                return (!inverted ? center_a : center_b) ? Crossing_Event::OVER
                                                         : Crossing_Event::UNDER;

            const bool bottom_a = same_pair(a, b, Edge_Handle_Namespace::MID_BOTTOM_LEFT,
                                            Edge_Handle_Namespace::BOTTOM_RIGHT);
            const bool bottom_b = same_pair(a, b, Edge_Handle_Namespace::MID_BOTTOM_RIGHT,
                                            Edge_Handle_Namespace::BOTTOM_LEFT);
            if (bottom_a || bottom_b)
                return (!inverted ? bottom_a : bottom_b) ? Crossing_Event::OVER
                                                         : Crossing_Event::UNDER;
            return Crossing_Event::NONE;
        }

        if (type_name.startsWith("2strand")) {
            const bool top_a = same_pair(a, b, Edge_Handle_Namespace::TOP_LEFT,
                                         Edge_Handle_Namespace::MID_TOP_RIGHT);
            const bool top_b = same_pair(a, b, Edge_Handle_Namespace::TOP_RIGHT,
                                         Edge_Handle_Namespace::MID_TOP_LEFT);
            if (top_a || top_b) return (!inverted ? top_a : top_b) ? Crossing_Event::OVER
                                                                   : Crossing_Event::UNDER;

            const bool bottom_a = same_pair(a, b, Edge_Handle_Namespace::MID_BOTTOM_LEFT,
                                            Edge_Handle_Namespace::BOTTOM_RIGHT);
            const bool bottom_b = same_pair(a, b, Edge_Handle_Namespace::MID_BOTTOM_RIGHT,
                                            Edge_Handle_Namespace::BOTTOM_LEFT);
            if (bottom_a || bottom_b)
                return (!inverted ? bottom_a : bottom_b) ? Crossing_Event::OVER
                                                         : Crossing_Event::UNDER;
            return Crossing_Event::NONE;
        }

        const bool diag_a = same_pair(a, b, Edge_Handle_Namespace::TOP_LEFT,
                                      Edge_Handle_Namespace::BOTTOM_RIGHT);
        const bool diag_b = same_pair(a, b, Edge_Handle_Namespace::TOP_RIGHT,
                                      Edge_Handle_Namespace::BOTTOM_LEFT);
        if (diag_a || diag_b)
            return (!inverted ? diag_a : diag_b) ? Crossing_Event::OVER : Crossing_Event::UNDER;

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

    for (Edge* e : graph.m_edges) e->reset();

    Path_Builder sink;
    std::vector<QString> component_words;
    while (true) {
        Edge* edge = nullptr;
        Edge::Handle handle = Edge_Handle_Namespace::NO_HANDLE;
        for (Edge* candidate : graph.m_edges) {
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

            Edge_Type* edge_type = edge->effective_edge_type();
            Edge::Handle next_handle = edge_type->traverse(edge, handle, sink);
            if (next_handle == Edge_Handle_Namespace::NO_HANDLE)
                next_handle = edge_type->Edge_Type::traverse(edge, handle, sink);
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
    graph.m_properties->set_span_formula(span_formula_value);

    int pm = 0;
    for (QChar c : span_formula_value) {
        if (c.isDigit()) pm = std::max(pm, c.digitValue());
    }

    const int r = graph.m_properties->group_count();
    const int c = graph.m_properties->edge_count();
    bool is_non_reducible = false;
    if (r <= pm + 1) {
        is_non_reducible = c >= (5 * pm - r - 1);
    } else {
        is_non_reducible = c >= (2 * (pm + r - 2));
    }
    graph.m_properties->set_is_non_reducible(is_non_reducible);
}
