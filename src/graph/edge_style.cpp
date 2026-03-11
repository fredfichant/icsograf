/**
 * \file src/graph/edge_style.cpp
 * \brief Implementation of edge style graph logic.
 */

#include "edge_style.hpp"

Edge_Style Edge_Style::default_to(const Edge_Style& other) const
{
    return Edge_Style(
        (enabled_style & HANDLE_LENGTH) ? handle_length : other.handle_length,
        (enabled_style & CROSSING_DISTANCE) ? crossing_distance : other.crossing_distance,
        (enabled_style & EDGE_SLIDE) ? edge_slide : other.edge_slide,
        (enabled_style & EDGE_TYPE) ? edge_type : other.edge_type, EVERYTHING,
        (enabled_style & SPACING) ? spacing : other.spacing,
        (enabled_style & STRAND_COUNT) ? strand_count : other.strand_count);
}
