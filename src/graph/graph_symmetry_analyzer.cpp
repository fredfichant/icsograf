#include "graph_symmetry_analyzer.hpp"

#include <QHash>
#include <QString>
#include <cmath>

#include "edge.hpp"
#include "edge_type.hpp"
#include "graph.hpp"
#include "node.hpp"

namespace
{
QString node_signature(const Node* node, const Graph& graph)
{
    // Only effective values are visible.  Disabled per-node fields must not
    // split an orbit when their graph-level default makes the nodes identical.
    const Node_Style style = node->style().default_to(graph.default_node_style());
    return QStringLiteral("%1|%2|%3|%4|%5|%6")
        .arg(node->edges().size())
        .arg(static_cast<int>(style.enabled_style))
        .arg(style.cusp_angle, 0, 'g', 17)
        .arg(style.handle_length, 0, 'g', 17)
        .arg(style.cusp_distance, 0, 'g', 17)
        .arg(reinterpret_cast<quintptr>(style.cusp_shape));
}

QString edge_signature(const Edge* edge)
{
    const Edge_Style style = edge->defaulted_style();
    const Edge_Type* type = edge->effective_edge_type();
    return QStringLiteral("%1|%2|%3|%4|%5|%6")
        .arg(type ? type->machine_name() : QString())
        .arg(style.handle_length, 0, 'g', 17)
        .arg(style.crossing_distance, 0, 'g', 17)
        .arg(style.edge_slide, 0, 'g', 17)
        .arg(style.spacing, 0, 'g', 17)
        .arg(edge->effective_strand_count());
}
}  // namespace

Graph_Symmetry_Result Graph_Symmetry_Analyzer::find_automorphisms(const Graph& graph,
                                                                    int permutation_limit)
{
    Graph_Symmetry_Result result;
    const QList<Node*> nodes = graph.nodes();
    const int count = nodes.size();
    QVector<QString> node_labels(count);
    QHash<const Node*, int> index;
    for (int i = 0; i < count; ++i) {
        index.insert(nodes[i], i);
        node_labels[i] = node_signature(nodes[i], graph);
    }

    QVector<QVector<QString>> adjacency(count, QVector<QString>(count));
    for (const Edge* edge : graph.edges()) {
        const int a = index.value(edge->vertex1(), -1);
        const int b = index.value(edge->vertex2(), -1);
        if (a < 0 || b < 0) continue;
        adjacency[a][b] = adjacency[b][a] = edge_signature(edge);
    }

    QVector<int> map(count, -1);
    QVector<bool> used(count, false);
    QVector<QVector<int>> automorphisms;
    int tried = 0;
    bool stopped = false;
    const auto search = [&](auto&& self, int source) -> void {
        if (stopped) return;
        if (source == count) {
            ++tried;
            if (tried > permutation_limit) {
                stopped = true;
                return;
            }
            automorphisms.append(map);
            return;
        }
        for (int target = 0; target < count; ++target) {
            if (used[target] || node_labels[source] != node_labels[target]) continue;
            bool valid = true;
            for (int previous = 0; previous < source; ++previous) {
                if (adjacency[source][previous] != adjacency[target][map[previous]]) {
                    valid = false;
                    break;
                }
            }
            if (!valid) continue;
            map[source] = target;
            used[target] = true;
            self(self, source + 1);
            used[target] = false;
            map[source] = -1;
        }
    };
    search(search, 0);

    result.complete = !stopped;
    result.group_order = automorphisms.size();
    QVector<int> parent(count);
    for (int i = 0; i < count; ++i) parent[i] = i;
    const auto root = [&](auto&& self, int value) -> int {
        return parent[value] == value ? value : parent[value] = self(self, parent[value]);
    };
    for (const QVector<int>& permutation : automorphisms) {
        bool identity = true;
        for (int i = 0; i < count; ++i) {
            if (permutation[i] != i) identity = false;
            const int a = root(root, i);
            const int b = root(root, permutation[i]);
            parent[a] = b;
        }
        if (!identity) result.generators.append(permutation);
    }
    QHash<int, QVector<int>> grouped;
    for (int i = 0; i < count; ++i) grouped[root(root, i)].append(i);
    result.orbits = grouped.values().toVector();
    // A structural permutation is a visible reflection when it equals a
    // reflection of the current node coordinates.  This deliberately keeps
    // geometry separate from the topological search above.
    constexpr double tolerance = 1e-6;
    for (const QVector<int>& permutation : automorphisms) {
        int source = -1;
        for (int i = 0; i < count; ++i)
            if (permutation[i] != i) {
                source = i;
                break;
            }
        if (source < 0) continue;
        const QPointF from = nodes[source]->pos();
        const QPointF to = nodes[permutation[source]]->pos();
        const QPointF normal = to - from;
        const double length2 = QPointF::dotProduct(normal, normal);
        if (length2 < tolerance) continue;
        const QPointF midpoint = (from + to) / 2.0;
        bool reflection = true;
        for (int i = 0; i < count; ++i) {
            const QPointF delta = nodes[i]->pos() - midpoint;
            const QPointF reflected = nodes[i]->pos() -
                                      normal * (2.0 * QPointF::dotProduct(delta, normal) / length2);
            if (QLineF(reflected, nodes[permutation[i]]->pos()).length() > tolerance) {
                reflection = false;
                break;
            }
        }
        if (!reflection) continue;
        const QPointF direction(-normal.y(), normal.x());
        const QLineF axis(midpoint - direction, midpoint + direction);
        bool duplicate = false;
        for (const QLineF& known : result.reflection_axes) {
            const QPointF known_normal(-known.dy(), known.dx());
            const double known_length2 = QPointF::dotProduct(known_normal, known_normal);
            if (std::abs(QPointF::dotProduct(midpoint - known.p1(), known_normal)) < tolerance &&
                std::abs(QPointF::dotProduct(normal, known_normal)) /
                        std::sqrt(length2 * known_length2) > 1.0 - tolerance) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) result.reflection_axes.append(axis);
    }
    return result;
}
