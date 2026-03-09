#include "graph_validation.hpp"

#include <algorithm>
#include <functional>

#include <QHash>
#include <QObject>
#include <QSet>
#include <QVector>

#include "edge.hpp"
#include "edge_type.hpp"
#include "graph.hpp"
#include "node.hpp"

namespace
{
bool is_simple_edge(const Edge* edge)
{
    const Edge_Type* type = edge->style().edge_type;
    return !type || type->strand_count() == 1;
}

bool is_connected(const Graph& graph)
{
    const QList<Node*> nodes = graph.nodes();
    if (nodes.isEmpty()) return false;

    QSet<Node*> visited;
    QVector<Node*> stack;
    stack.push_back(nodes.first());
    visited.insert(nodes.first());

    while (!stack.isEmpty()) {
        Node* cur = stack.back();
        stack.pop_back();
        for (Edge* e : cur->edges()) {
            Node* next = e->other(cur);
            if (!next || visited.contains(next)) continue;
            visited.insert(next);
            stack.push_back(next);
        }
    }

    return visited.size() == nodes.size();
}

bool is_biconnected(const Graph& graph)
{
    const QList<Node*> nodes = graph.nodes();
    if (nodes.size() < 2) return false;
    if (!is_connected(graph)) return false;

    QHash<Node*, int> disc;
    QHash<Node*, int> low;
    QHash<Node*, Edge*> parent_edge;
    for (Node* n : nodes) {
        disc[n] = -1;
        low[n] = -1;
        parent_edge[n] = nullptr;
    }

    int time = 0;
    bool articulation_found = false;

    std::function<void(Node*)> dfs = [&](Node* u) {
        disc[u] = low[u] = ++time;
        int child_count = 0;

        for (Edge* e : u->edges()) {
            Node* v = e->other(u);
            if (!v) continue;

            if (disc[v] == -1) {
                parent_edge[v] = e;
                ++child_count;
                dfs(v);
                low[u] = std::min(low[u], low[v]);

                if (parent_edge[u] == nullptr && child_count > 1) {
                    articulation_found = true;
                }
                if (parent_edge[u] != nullptr && low[v] >= disc[u]) {
                    articulation_found = true;
                }
            } else if (e != parent_edge[u]) {
                low[u] = std::min(low[u], disc[v]);
            }
        }
    };

    dfs(nodes.first());

    for (Node* n : nodes) {
        if (disc[n] == -1) return false;
    }
    return !articulation_found;
}

bool contains_simple_sequence_longer_than_three(const Graph& graph)
{
    const QList<Edge*> all_edges = graph.edges();

    QHash<Node*, QList<const Edge*>> simple_adj;
    QList<const Edge*> simple_edges;
    for (Edge* e : all_edges) {
        if (!is_simple_edge(e)) continue;
        simple_edges.push_back(e);
        simple_adj[e->vertex1()].push_back(e);
        simple_adj[e->vertex2()].push_back(e);
    }

    if (simple_edges.isEmpty()) return false;

    auto simple_degree = [&](Node* n) { return simple_adj.value(n).size(); };
    auto is_breakpoint = [&](Node* n) { return simple_degree(n) != 2; };

    QSet<const Edge*> visited;

    // Walk open simple chains from breakpoints. Pure cycles (all deg=2) are ignored.
    for (Node* start : simple_adj.keys()) {
        if (!is_breakpoint(start)) continue;
        for (const Edge* first : simple_adj.value(start)) {
            if (visited.contains(first)) continue;

            int length = 1;
            visited.insert(first);
            Node* prev = start;
            Node* cur = first->other(start);

            while (cur && !is_breakpoint(cur)) {
                const QList<const Edge*>& inc = simple_adj.value(cur);
                const Edge* next = nullptr;
                for (const Edge* cand : inc) {
                    if (cand->other(cur) == prev) continue;
                    next = cand;
                    break;
                }

                if (!next || visited.contains(next)) break;
                visited.insert(next);
                ++length;
                prev = cur;
                cur = next->other(cur);
            }

            if (length > 3) {
                // qDebug() << "Validation: simple chain too long, length =" << length;
                return true;
            }
        }
    }

    return false;
}
}  // namespace

Graph_Validation_Result validate_graph(const Graph& graph)
{
    if (!is_connected(graph)) {
        return {false, QObject::tr("graph is not connected")};
    }
    if (!is_biconnected(graph)) {
        return {false, QObject::tr("graph is not biconnected")};
    }
    if (contains_simple_sequence_longer_than_three(graph)) {
        return {false, QObject::tr("graph contains a sequence of simple edges longer than 3")};
    }
    return {true, QString()};
}
