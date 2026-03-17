/**
 * \file graph_appearance.hpp
 * \brief Appearance settings grouped outside of Graph container state.
 */

#ifndef GRAPH_APPEARANCE_HPP
#define GRAPH_APPEARANCE_HPP

#include <QColor>
#include <QList>
#include <QPen>

#include "edge_style.hpp"
#include "node_style.hpp"

class Graph_Appearance
{
   public:
    Graph_Appearance(const Node_Style& default_node_style = Node_Style(),
                     const Edge_Style& default_edge_style = Edge_Style(),
                     const QList<QColor>& colors = QList<QColor>(), bool auto_color = true,
                     const QPen& pen = QPen())
        : default_node_style(default_node_style),
          default_edge_style(default_edge_style),
          colors(colors),
          auto_color(auto_color),
          pen(pen)
    {
    }

    Node_Style default_node_style;
    Edge_Style default_edge_style;
    QList<QColor> colors;
    bool auto_color;
    QPen pen;
};

#endif  // GRAPH_APPEARANCE_HPP
