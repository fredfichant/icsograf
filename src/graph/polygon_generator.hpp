#ifndef POLYGON_GENERATOR_HPP
#define POLYGON_GENERATOR_HPP

#include "graph/graph.hpp"
#include "widgets/knot_view/snapping_grid.hpp"  // For Snapping_Grid

/**
 * @brief Utility class to generate Graph objects representing polygons.
 *        This class encapsulates the logic previously found in polygon.js.
 */
class Polygon_Generator
{
   public:
    /**
     * @brief Creates a Graph object representing a polygon.
     * @param sides The number of sides of the polygon (minimum 3).
     * @param middle_node If true, a central node will be added and connected to all vertices.
     * @param radius The radius of the polygon.
     * @param grid Optional: A Snapping_Grid to query for grid size if needed.
     * @return A Graph object representing the polygon.
     */
    static Graph create_polygon(int sides, bool middle_node, double radius,
                                Snapping_Grid* grid = nullptr);
};

#endif  // POLYGON_GENERATOR_HPP
