/**
 * \file src/graph/traversal_info.hpp
 * \brief API declarations for traversal info graph data structures and operations.
 */

#ifndef TRAVERSAL_INFO_HPP
#define TRAVERSAL_INFO_HPP

#include "edge.hpp"
#include "edge_handle.hpp"

/**
    \brief Information of the way an edge has be traversed
*/
struct Traversal_Info
{
    enum handside_type
    {
        LEFT,
        RIGHT
    };

    struct Source
    {
        Edge* edge;
        Edge_Handle handle;
        double angle;

        Source() : edge(nullptr), handle(Edge_Handle_Namespace::NO_HANDLE), angle(0) {}
    };

    Source in;
    Source out;
    Node* node;
    handside_type handside;  ///< Side of the input handle relative to the input edge
    double angle_delta;  ///< Difference between angle_in and angle_out (clock or counter depends on
                         ///< handside)
    int strand;          ///< Current strand index (0, 1, or 2)
    bool success;        ///< Input parameters were OK

    Traversal_Info() : node(nullptr), handside(LEFT), angle_delta(0), strand(0), success(false) {}
};
#endif  // TRAVERSAL_INFO_HPP
