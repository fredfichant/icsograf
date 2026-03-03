#ifndef EDGE_STYLE_HPP
#define EDGE_STYLE_HPP

#include <QMetaType>

class Edge_Type;

class Edge_Style
{
   public:
    enum Enabled_Styles_Enum
    {
        NOTHING = 0x00,
        HANDLE_LENGTH = 0x01,
        CROSSING_DISTANCE = 0x02,
        EDGE_SLIDE = 0x04,
        EDGE_TYPE = 0x08,
        SPACING = 0x10,
        STRAND_COUNT = 0x20,
        EVERYTHING = 0xFF
    };
    Q_DECLARE_FLAGS(Enabled_Styles, Enabled_Styles_Enum)

   public:
    /// Which styles to override
    Enabled_Styles enabled_style;
    /// Length of the handles (line from start point to control point in SVG curves )
    double handle_length;
    /// Length of the gap that highlights the thread passing under another one
    double crossing_distance;
    /// Value in [0,1] that determines the offset of the crossing
    double edge_slide;
    /// Distance between parallel strands
    double spacing;
    /// Number of strands (1, 2, or 3)
    int strand_count;
    /// Polymorphic object that renders the crossing
    Edge_Type* edge_type;

   public:
    Edge_Style(double handle_length = 24, double crossing_distance = 10, double edge_slide = 0.5,
               Edge_Type* edge_type = nullptr, Enabled_Styles enabled_style = NOTHING,
               double spacing = 10, int strand_count = 1)
        : enabled_style(enabled_style),
          handle_length(handle_length),
          crossing_distance(crossing_distance),
          edge_slide(edge_slide),
          spacing(spacing),
          strand_count(strand_count),
          edge_type(edge_type)
    {
    }

    /// Set disabled style to the values in other
    Edge_Style default_to(const Edge_Style& other) const;
};

Q_DECLARE_METATYPE(Edge_Style::Enabled_Styles)

#endif  // EDGE_STYLE_HPP
