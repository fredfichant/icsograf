/**
 * \file src/graph/edge_inverted_logic.hpp
 * \brief API declarations for edge inverted logic graph data structures and operations.
 */

#ifndef EDGE_INVERTED_LOGIC_HPP
#define EDGE_INVERTED_LOGIC_HPP

#include <QPainter>
#include "edge.hpp"

/**
 * \brief Utility class for sharing common inverted edge logic.
 *
 * This class provides standard implementations for visual and behavioral
 * aspects of inverted edges (e.g., dashed line painting).
 */
class Edge_Inverted_Logic
{
public:
    /**
     * \brief Standard painting for inverted edges (dashed line).
     */
    static void paint_inverted(QPainter* painter, const Edge& edge);
};

#endif // EDGE_INVERTED_LOGIC_HPP
