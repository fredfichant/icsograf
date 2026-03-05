/**
 * \file edge_style.hpp
 * \brief Defines the styling properties for edges in the knot graph.
 */

#ifndef EDGE_STYLE_HPP
#define EDGE_STYLE_HPP

#include <QMetaType>

class Edge_Type;

/**
 * \brief Container for edge styling properties.
 *
 * This class holds various visual properties of an edge, such as handle length,
 * crossing distance, and the specific Edge_Type strategy used for rendering.
 * It supports a mechanism to override specific properties while inheriting others
 * from a default style using the `enabled_style` flags.
 */
class Edge_Style
{
   public:
    /**
     * \brief Flags indicating which style properties are explicitly set.
     *
     * These flags are used to determine if a specific property should be used
     * from this instance or if it should fall back to a default value.
     */
    enum Enabled_Styles_Enum
    {
        NOTHING = 0x00,            ///< No properties are enabled (use defaults).
        HANDLE_LENGTH = 0x01,      ///< Override handle_length.
        CROSSING_DISTANCE = 0x02,  ///< Override crossing_distance.
        EDGE_SLIDE = 0x04,         ///< Override edge_slide.
        EDGE_TYPE = 0x08,          ///< Override edge_type.
        SPACING = 0x10,            ///< Override spacing.
        STRAND_COUNT = 0x20,       ///< Override strand_count.
        EVERYTHING = 0xFF          ///< All properties are enabled.
    };
    Q_DECLARE_FLAGS(Enabled_Styles, Enabled_Styles_Enum)

   public:
    /// Bitmask of enabled styles (overrides).
    Enabled_Styles enabled_style;
    /// Length of the Bezier control handles (affects curvature).
    double handle_length;
    /// Size of the visual gap for under-passing strands.
    double crossing_distance;
    /// Position of the crossing along the edge (0.0 to 1.0).
    double edge_slide;
    /// Distance between parallel strands (for multi-strand edges).
    double spacing;
    /// Number of parallel strands (e.g., 1, 2, 3).
    int strand_count;
    /// Pointer to the strategy object defining the crossing logic and rendering.
    Edge_Type* edge_type;

   public:
    /**
     * \brief Constructs an Edge_Style with specified properties.
     *
     * \param handle_length Length of control handles.
     * \param crossing_distance Size of the crossing gap.
     * \param edge_slide Position of the crossing (0.0 to 1.0).
     * \param edge_type Pointer to the edge type definition.
     * \param enabled_style Bitmask of enabled properties.
     * \param spacing Spacing between strands.
     * \param strand_count Number of strands.
     */
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

    /**
     * \brief Creates a new style by combining this style with a default style.
     *
     * For every property not enabled in `enabled_style`, the value from `other`
     * is used. If enabled, the value from this instance is used.
     *
     * \param other The default style to fallback to.
     * \return A new Edge_Style with all properties resolved.
     */
    Edge_Style default_to(const Edge_Style& other) const;
};

Q_DECLARE_METATYPE(Edge_Style::Enabled_Styles)

#endif  // EDGE_STYLE_HPP
