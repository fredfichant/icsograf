/**
 * \file graph_renderer.hpp
 * \brief Internal graph rendering/traversal helper used by Graph.
 */

#ifndef GRAPH_RENDERER_HPP
#define GRAPH_RENDERER_HPP

#include <QRectF>

#include "edge.hpp"
#include "traversal_info.hpp"

class Graph;
class Path_Builder;

class Graph_Renderer
{
   public:
    static void render(Graph& graph);
    static void traverse(Graph& graph, Path_Builder& path);
    static Traversal_Info traverse(Graph& graph, Edge* edge, Edge::Handle handle, Path_Builder& path);
    static void update_bounding_box(Graph& graph);
    static QRectF full_image_bounding_rect(const Graph& graph);
};

#endif  // GRAPH_RENDERER_HPP
