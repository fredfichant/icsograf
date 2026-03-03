/**
 * \file image_exporter.hpp
 * \brief SVG image generation for Knotter graphs
 */

#ifndef IMAGE_EXPORTER_HPP
#define IMAGE_EXPORTER_HPP

#include "graph.hpp"

/**
 * \brief Exports a graph as an SVG image
 *
 * This function generates an SVG representation of the knot graph,
 * optionally including the underlying graph structure.
 *
 * \param[out] file The device to write the SVG data to
 * \param graph The graph to be rendered (must have already built the knot)
 * \param draw_graph If true, also renders the graph nodes and edges
 */
void export_svg(QIODevice& file, const Graph& graph, bool draw_graph);

#endif  // IMAGE_EXPORTER_HPP
