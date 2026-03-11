/**
 * \file src/graph/edges_mark.cpp
 * \brief Implementation of edges mark graph logic.
 */

#include "edges_mark.hpp"

#include <algorithm>
#include <set>

#include "edge.hpp"
#include "graph.hpp"
#include "node.hpp"

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

}  // namespace

std::map<const Edge*, EdgeMarking> GraphMarker::mark_graph(
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
    std::map<const Edge*, EdgeMarking> result;

    if (!consistent) return result;

    int k = nullspace.size();
    if (k == 0) {
        for (int i = 0; i < m; ++i) {
            result[edges_list[i]] = {particular[i] ? "a" : "0", true, particular[i]};
        }
    } else if (k <= 10) {  // Limit number of solutions
        int num_solutions = 1 << k;
        for (int i = 0; i < m; ++i) {
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
