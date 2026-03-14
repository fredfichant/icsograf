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
 * This function generates an SVG representation of the knot graph.
 * When `draw_graph` is true, the underlying graph structure is rendered
 * alongside the knot instead of being superimposed on top of it.
 * Graph properties can also be included or omitted from the exported SVG.
 *
 * \param[out] file The device to write the SVG data to
 * \param graph The graph to be rendered (must have already built the knot)
 * \param draw_graph If true, also renders the graph nodes and edges in a
 *        separate panel beside the knot
 * \param include_properties If true, appends graph properties text under the
 *        rendered panels
 */
void export_svg(QIODevice& file, const Graph& graph, bool draw_graph,
                bool include_properties = true);

#endif  // IMAGE_EXPORTER_HPP
