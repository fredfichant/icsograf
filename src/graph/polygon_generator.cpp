#include "polygon_generator.hpp"

#include <QPointF>  // For QPointF
#include <cmath>    // For M_PI, std::cos, std::sin

#include "graph/edge.hpp"  // Required for Edge
#include "graph/node.hpp"  // Required for Node

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Graph Polygon_Generator::create_polygon(int sides, bool middle_node, double radius,
                                        Snapping_Grid* grid)
{
    Graph graph;

    Node* last_node =
        nullptr;  // Renamed to avoid conflict with 'last' variable in general contexts
    Node* first_node =
        nullptr;  // Renamed to avoid conflict with 'first' variable in general contexts

    // Adjust radius if grid is enabled and provides a size
    if (grid && grid->is_enabled()) {
        // As discussed, radius parameter should already account for grid if needed.
    }

    Node* middle_graph_node = nullptr;  // Renamed to avoid conflict with 'middle' variable
    if (middle_node) {
        middle_graph_node = new Node(QPointF(0, 0));
        graph.add_node(middle_graph_node);
    }

    for (int i = 0; i < sides; ++i) {
        double angle = 2 * M_PI * i / sides;
        QPointF pos(
            radius * std::cos(angle),
            -radius * std::sin(angle));  // Qt's Y-axis is typically inverted from standard math

        Node* current_node = new Node(pos);  // Create a new Node object
        graph.add_node(current_node);        // Add the node to the graph

        if (!first_node) {
            first_node = current_node;
        }

        if (middle_graph_node) {
            Edge* edge =
                new Edge(middle_graph_node, current_node, graph.default_edge_style().edge_type);
            graph.add_edge(edge);
        }

        if (last_node) {
            Edge* edge = new Edge(last_node, current_node, graph.default_edge_style().edge_type);
            graph.add_edge(edge);
        }

        last_node = current_node;
    }
    // Connect the last node back to the first node to close the polygon
    Edge* closing_edge = new Edge(last_node, first_node, graph.default_edge_style().edge_type);
    graph.add_edge(closing_edge);

    return graph;
}
