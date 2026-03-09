/**
 * \file graph_validation.hpp
 * \brief Validation rules for knot graphs
 */

#ifndef GRAPH_VALIDATION_HPP
#define GRAPH_VALIDATION_HPP

#include <QString>

class Graph;

struct Graph_Validation_Result
{
    bool valid = true;
    QString reason;
};

/**
 * \brief Validates the graph according to knot constraints.
 *
 * Rules:
 * - The graph must be connected and biconnected.
 * - The graph must not contain a sequence of simple (1-strand) edges longer than 3.
 */
Graph_Validation_Result validate_graph(const Graph& graph);

#endif  // GRAPH_VALIDATION_HPP
