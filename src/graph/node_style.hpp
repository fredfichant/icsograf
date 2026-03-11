/**
 * \file src/graph/node_style.hpp
 * \brief API declarations for node style graph data structures and operations.
 */

#ifndef NODE_STYLE_HPP
#define NODE_STYLE_HPP

#include <QObject>

#include "node_cusp_shape.hpp"
#include "path_builder.hpp"

class Node_Style
{
   public:
    enum Enabled_Styles_Enum
    {
        NOTHING = 0x00,
        CUSP_SHAPE = 0x01,
        CUSP_ANGLE = 0x02,
        HANDLE_LENGTH = 0x04,
        CUSP_DISTANCE = 0x08,
        EVERYTHING = 0xFF
    };
    Q_DECLARE_FLAGS(Enabled_Styles, Enabled_Styles_Enum)

   public:
    /// Which styles to override
    Enabled_Styles enabled_style;
    /// Minimum angle required to enable the cusp
    double cusp_angle;
    /// Length of the handles (line from start point to control point in SVG curves )
    double handle_length;
    /// Distance from the node to the cusp point
    double cusp_distance;
    /// Polymorphic object that renders the cusp
    Cusp_Shape* cusp_shape;

   public:
    Node_Style(double cusp_angle = 225, double handle_length = 24, double cusp_distance = 32,
               Cusp_Shape* cusp_shape = nullptr, Enabled_Styles enabled_style = NOTHING)
        : enabled_style(enabled_style),
          cusp_angle(cusp_angle),
          handle_length(handle_length),
          cusp_distance(cusp_distance),
          cusp_shape(cusp_shape)
    {
    }

    /// Set disabled style to the values in other
    Node_Style default_to(const Node_Style& other) const;

    void build(const Traversal_Info& ti, Path_Builder& path, const Node_Style& default_style) const
    {
        Node_Style def = default_to(default_style);
        if (def.cusp_shape) def.cusp_shape->draw_joint(path, ti, def);
    }
};

Q_DECLARE_METATYPE(Node_Style::Enabled_Styles)

#endif  // NODE_STYLE_HPP
