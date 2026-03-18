/**
 * \file edge_type_utils.hpp
 * \brief Utility functions for calculating edge geometry and debugging handles.
 */

#ifndef EDGE_TYPE_UTILS_HPP
#define EDGE_TYPE_UTILS_HPP

#include <QPointF>
#include <QPainter>
#include <QString>

#include "edge_handle.hpp"

class Edge;

/**
 * \brief Converts an edge handle identifier to a string representation.
 * \details Useful for debugging purposes to identify which handle is being processed.
 * \param handle The handle to convert.
 * \return A string describing the handle (e.g., "TOP_LEFT (S0)").
 */
QString handleToString(Edge_Handle handle);

/**
 * \brief Calculates the physical position of a handle on an edge.
 *
 * \details This function computes the exact coordinates of a handle (connection point)
 * based on the edge's geometry and style settings. It accounts for:
 * - The specific location of the handle (e.g., TOP_LEFT, CENTER).
 * - The edge's rotation angle.
 * - The spacing between strands.
 * - The crossing distance (size of the crossing).
 * - The edge slide (position of the crossing along the edge).
 *
 * \param edge Pointer to the edge.
 * \param handle The specific handle identifier (location and strand).
 * \return The calculated QPointF in the scene coordinates.
 */
QPointF get_handle_pos(const Edge* edge, Edge_Handle handle);

/**
 * \brief Draws the editor representation of an edge as one or more parallel lines.
 *
 * For 2-strand edges, this renders 2 parallel lines spaced by 8 scene units.
 * For 3-strand edges, this renders 3 parallel lines spaced by 6 scene units.
 * Other edge types are rendered as a single line.
 */
void draw_editor_edge_lines(QPainter* painter, const Edge& edge, int strands);

#endif  // EDGE_TYPE_UTILS_HPP
