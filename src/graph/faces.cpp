/**
 * \file src/graph/faces.cpp
 * \brief Implementation of faces graph logic.
 */

#include "faces.hpp"

#include <algorithm>
#include <cstdint>
#include <map>
#include <vector>

#include "graph.hpp"
#include "node.hpp"

namespace {  // Anonymous namespace for implementation details

struct Point
{
    int64_t x, y;

    Point(int64_t x_ = 0, int64_t y_ = 0) : x(x_), y(y_) {}

    Point operator-(const Point& p) const { return Point(x - p.x, y - p.y); }

    int64_t cross(const Point& p) const { return x * p.y - y * p.x; }

    int64_t cross(const Point& p, const Point& q) const { return (p - *this).cross(q - *this); }

    int half() const { return int(y < 0 || (y == 0 && x < 0)); }
};

// The algorithm from the user prompt
std::vector<std::vector<size_t>> find_faces_impl(std::vector<Point>& vertices,
                                                 std::vector<std::vector<size_t>>& adj)
{
    size_t n = vertices.size();
    if (n == 0) {
        return {};
    }
    std::vector<std::vector<char>> used(n);
    for (size_t i = 0; i < n; i++) {
        used[i].resize(adj[i].size());
        used[i].assign(adj[i].size(), 0);
        auto compare = [&](size_t l, size_t r) {
            Point pl = vertices[l] - vertices[i];
            Point pr = vertices[r] - vertices[i];
            if (pl.half() != pr.half()) return pl.half() < pr.half();
            return pl.cross(pr) > 0;
        };
        std::sort(adj[i].begin(), adj[i].end(), compare);
    }
    std::vector<std::vector<size_t>> faces;
    for (size_t i = 0; i < n; i++) {
        for (size_t edge_id = 0; edge_id < adj[i].size(); edge_id++) {
            if (used[i][edge_id]) {
                continue;
            }
            std::vector<size_t> face;
            size_t v = i;
            size_t e = edge_id;
            while (v < used.size() && e < used[v].size() && !used[v][e]) {
                used[v][e] = true;
                face.push_back(v);
                size_t u = adj[v][e];
                auto it =
                    std::lower_bound(adj[u].begin(), adj[u].end(), v, [&](size_t l, size_t r) {
                        Point pl = vertices[l] - vertices[u];
                        Point pr = vertices[r] - vertices[u];
                        if (pl.half() != pr.half()) return pl.half() < pr.half();
                        return pl.cross(pr) > 0;
                    });
                size_t e1 = (it - adj[u].begin() + 1);
                if (e1 == adj[u].size()) {
                    e1 = 0;
                }
                v = u;
                e = e1;
            }
            std::reverse(face.begin(), face.end());
            if (face.size() < 3) continue;  // Not a real face
            Point p1 = vertices[face[0]];
            __int128 sum = 0;
            for (size_t j = 0; j < face.size(); ++j) {
                Point p2 = vertices[face[j]];
                Point p3 = vertices[face[(j + 1) % face.size()]];
                sum += (p2 - p1).cross(p3 - p2);
            }
            if (sum <= 0) {
                faces.insert(faces.begin(), face);
            } else {
                faces.emplace_back(face);
            }
        }
    }
    return faces;
}

}  // end anonymous namespace

std::vector<std::vector<std::size_t>> find_faces(const Graph& graph)
{
    const QList<Node*>& nodes = graph.nodes();
    if (nodes.isEmpty()) {
        return {};
    }

    std::vector<Point> vertices;
    vertices.reserve(nodes.size());

    std::map<Node*, size_t> node_to_idx;

    for (int i = 0; i < nodes.size(); ++i) {
        Node* node = nodes[i];
        // Scale to integer coordinates
        vertices.push_back(Point(node->pos().x() * 1000, node->pos().y() * 1000));
        node_to_idx[node] = i;
    }

    std::vector<std::vector<size_t>> adj(nodes.size());
    for (auto* edge : graph.edges()) {
        Node* n1 = edge->vertex1();
        Node* n2 = edge->vertex2();
        if (node_to_idx.count(n1) && node_to_idx.count(n2)) {
            size_t idx1 = node_to_idx.at(n1);
            size_t idx2 = node_to_idx.at(n2);
            adj[idx1].push_back(idx2);
            adj[idx2].push_back(idx1);
        }
    }

    return find_faces_impl(vertices, adj);
}