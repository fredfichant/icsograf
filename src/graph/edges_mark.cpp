/**
 * \file src/graph/edges_mark.cpp
 * \brief Implementation of edges mark graph logic.
 */

#include "edges_mark.hpp"

#include <algorithm>
#include <QLineF>
#include <set>
#include <tuple>

#include "edge.hpp"
#include "edge_handle.hpp"
#include "edge_normal.hpp"
#include "edge_type.hpp"
#include "graph.hpp"
#include "node.hpp"
#include "path_builder.hpp"
#include "traversal_info.hpp"

namespace {

class BinaryLinearSystem
{
   private:
    int n;
    std::vector<std::vector<int>> A;  // Dynamic size matrix, using int (0/1)
    int m;

   public:
    BinaryLinearSystem(int vars) : n(vars), m(0) {}

    void addEquation(const std::vector<int>& vars, bool rhs)
    {
        if (vars.empty() && !rhs) return;
        std::vector<int> row(n + 1, 0);
        for (int v : vars) {
            if (v < n) row[v] = 1;
        }
        row[n] = rhs ? 1 : 0;
        A.push_back(row);
        m++;
    }

    std::tuple<bool, std::vector<bool>, std::vector<std::vector<bool>>> solve()
    {
        int r = 0;
        std::vector<int> pivot_col(m, -1);

        for (int col = 0; col < n && r < m; col++) {
            int pivot = r;
            while (pivot < m && !A[pivot][col]) pivot++;
            if (pivot == m) continue;
            std::swap(A[r], A[pivot]);
            for (int i = 0; i < m; i++) {
                if (i != r && A[i][col]) {
                    // XOR row i with row r
                    for (int j = col; j <= n; j++) {
                        A[i][j] ^= A[r][j];
                    }
                }
            }
            pivot_col[r] = col;
            r++;
        }

        for (int i = r; i < m; i++) {
            if (A[i][n]) return {false, {}, {}};
        }

        std::vector<bool> sol(n, false);
        for (int i = 0; i < r; i++) {
            if (pivot_col[i] != -1) sol[pivot_col[i]] = A[i][n];
        }

        std::vector<std::vector<bool>> nullspace;
        std::vector<bool> is_free(n, true);
        for (int i = 0; i < r; i++) {
            if (pivot_col[i] != -1) is_free[pivot_col[i]] = false;
        }

        for (int col = 0; col < n; col++) {
            if (is_free[col]) {
                std::vector<bool> basis_vec(n, false);
                basis_vec[col] = true;
                for (int i = 0; i < r; i++) {
                    if (pivot_col[i] != -1 && A[i][col]) basis_vec[pivot_col[i]] = true;
                }
                nullspace.push_back(basis_vec);
            }
        }
        return {true, sol, nullspace};
    }
};

int edge_weight(const Edge* edge)
{
    const Edge_Type* type = edge->style().edge_type;
    if (!type) return 1;
    const QString name = type->machine_name();
    if (name.startsWith("2strand")) return 2;
    if (name.startsWith("3strand")) return 3;
    return 1;
}

bool edge_is_inverted(const Edge* edge)
{
    const Edge_Type* type = edge->style().edge_type;
    return type && type->machine_name().contains("inverted");
}

bool node_jump(Edge* in_edge, Edge::Handle in_handle, Edge*& out_edge, Edge::Handle& out_handle)
{
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
}

}  // namespace

GraphMarker::LinearSystemSolution GraphMarker::solve_marking_system(
    const Graph& graph, const std::vector<std::vector<size_t>>& faces)
{
    const QList<Edge*>& edges_list = graph.edges();
    const QList<Node*>& nodes_list = graph.nodes();
    int m = edges_list.size();
    if (m == 0) return {};

    BinaryLinearSystem system(m);
    std::map<const Edge*, int> edge_to_id;
    std::map<std::pair<int, int>, int> uv_to_id;

    for (int i = 0; i < m; ++i) {
        edge_to_id[edges_list[i]] = i;
        int u = nodes_list.indexOf(edges_list[i]->vertex1());
        int v = nodes_list.indexOf(edges_list[i]->vertex2());
        uv_to_id[{std::min(u, v), std::max(u, v)}] = i;
    }

    // Node constraints
    for (int i = 0; i < nodes_list.size(); ++i) {
        std::vector<int> incident_ids;
        for (auto* e : nodes_list[i]->edges()) {
            if (edge_to_id.count(e)) incident_ids.push_back(edge_to_id[e]);
        }
        system.addEquation(incident_ids, false);
    }

    // Face constraints
    for (const auto& face_nodes : faces) {
        std::vector<int> boundary_ids;
        for (size_t i = 0; i < face_nodes.size(); ++i) {
            int u = face_nodes[i];
            int v = face_nodes[(i + 1) % face_nodes.size()];
            if (uv_to_id.count({std::min(u, v), std::max(u, v)})) {
                boundary_ids.push_back(uv_to_id[{std::min(u, v), std::max(u, v)}]);
            }
        }
        system.addEquation(boundary_ids, face_nodes.size() % 2 == 1);
    }

    auto [consistent, particular, nullspace] = system.solve();
    if (!consistent) return {};

    return {true, particular, nullspace};
}

std::map<const Edge*, EdgeMarking> GraphMarker::mark_graph(
    const Graph& graph, const std::vector<std::vector<size_t>>& faces)
{
    const QList<Edge*>& edges_list = graph.edges();
    std::map<const Edge*, EdgeMarking> result;
    if (edges_list.empty()) return result;

    const LinearSystemSolution solution = solve_marking_system(graph, faces);
    if (!solution.consistent) return result;

    const std::vector<bool>& particular = solution.particular;
    const std::vector<std::vector<bool>>& nullspace = solution.nullspace;
    int k = nullspace.size();
    if (k == 0) {
        for (int i = 0; i < edges_list.size(); ++i) {
            result[edges_list[i]] = {particular[i] ? "a" : "0", true, particular[i]};
        }
    } else if (k <= 10) {  // Limit number of solutions
        int num_solutions = 1 << k;
        for (int i = 0; i < edges_list.size(); ++i) {
            std::vector<int> values;
            for (int mask = 0; mask < num_solutions; mask++) {
                bool val = particular[i];
                for (int j = 0; j < k; j++) {
                    if (mask & (1 << j)) val = val ^ nullspace[j][i];
                }
                values.push_back(val);
            }
            std::vector<std::string> symbols = encodeSymbols(values);
            std::string marking;
            for (size_t s = 0; s < symbols.size(); ++s) {
                if (s > 0) marking += ",";
                marking += symbols[s];
            }
            result[edges_list[i]] = {marking, false, false};
        }
    }
    return result;
}

std::vector<EdgeDistributionTable> GraphMarker::edge_distribution_tables(
    const Graph& graph, const std::vector<std::vector<size_t>>& faces)
{
    return edge_distribution_tables_from_linear_solutions(graph, faces);
}

std::vector<EdgeDistributionTable> GraphMarker::edge_distribution_tables_from_linear_solutions(
    const Graph& graph, const std::vector<std::vector<size_t>>& faces)
{
    const QList<Edge*>& edges_list = graph.edges();
    std::vector<EdgeDistributionTable> tables;
    if (edges_list.empty()) return tables;

    const LinearSystemSolution solution = solve_marking_system(graph, faces);
    if (!solution.consistent) return tables;

    const std::vector<bool>& particular = solution.particular;
    const std::vector<std::vector<bool>>& nullspace = solution.nullspace;
    const int k = nullspace.size();

    auto build_table = [&](const std::vector<bool>& assignment) {
        EdgeDistributionTable table;
        for (int i = 0; i < edges_list.size(); ++i) {
            const Edge* edge = edges_list[i];
            const int weight = edge_weight(edge);
            const bool inverted = edge_is_inverted(edge);
            const bool is_a = assignment[i];

            if (is_a) {
                if (inverted)
                    table.pa += weight;
                else
                    table.wa += weight;
            } else {
                if (inverted)
                    table.p0 += weight;
                else
                    table.w0 += weight;
            }
        }
        return table;
    };

    if (k == 0) {
        tables.push_back(build_table(particular));
    } else {
        const int num_solutions = 1 << k;
        for (int mask = 0; mask < num_solutions; ++mask) {
            std::vector<bool> assignment = particular;
            for (int j = 0; j < k; ++j) {
                if (!(mask & (1 << j))) continue;
                for (std::size_t i = 0; i < assignment.size(); ++i) {
                    assignment[i] = assignment[i] ^ nullspace[j][i];
                }
            }
            tables.push_back(build_table(assignment));
        }
    }

    std::vector<int> label_ids;
    label_ids.reserve(tables.size());
    for (std::size_t i = 0; i < tables.size(); ++i) {
        label_ids.push_back(static_cast<int>(i));
    }
    const std::vector<std::string> labels = encodeSymbols(label_ids);
    for (std::size_t i = 0; i < tables.size() && i < labels.size(); ++i) {
        tables[i].label = labels[i];
    }

    return tables;
}

std::vector<std::vector<int>> GraphMarker::edge_assignments_from_linear_solutions(
    const Graph& graph, const std::vector<std::vector<size_t>>& faces)
{
    const QList<Edge*>& edges_list = graph.edges();
    std::vector<std::vector<int>> assignments;
    if (edges_list.empty()) return assignments;

    const LinearSystemSolution solution = solve_marking_system(graph, faces);
    if (!solution.consistent) return assignments;

    const std::vector<bool>& particular = solution.particular;
    const std::vector<std::vector<bool>>& nullspace = solution.nullspace;
    const int k = nullspace.size();

    if (k == 0) {
        std::vector<int> assignment;
        assignment.reserve(particular.size());
        for (bool value : particular) assignment.push_back(value ? 1 : 0);
        assignments.push_back(assignment);
        return assignments;
    }

    const int num_solutions = 1 << k;
    for (int mask = 0; mask < num_solutions; ++mask) {
        std::vector<bool> current = particular;
        for (int j = 0; j < k; ++j) {
            if (!(mask & (1 << j))) continue;
            for (std::size_t i = 0; i < current.size(); ++i) {
                current[i] = current[i] ^ nullspace[j][i];
            }
        }

        std::vector<int> assignment;
        assignment.reserve(current.size());
        for (bool value : current) assignment.push_back(value ? 1 : 0);
        assignments.push_back(assignment);
    }

    return assignments;
}

std::vector<std::string> GraphMarker::encodeSymbols(const std::vector<int>& values)
{
    std::vector<std::string> symbols;
    if (values.size() == 1) {
        symbols.push_back(values[0] ? "a" : "0");
        return symbols;
    }
    char next_letter = 'a';
    char next_digit = '0';
    std::set<int> unique_values(values.begin(), values.end());
    for (int val : unique_values) {
        if (val)
            symbols.push_back(std::string(1, next_letter++));
        else
            symbols.push_back(std::string(1, next_digit++));
    }
    return symbols;
}
