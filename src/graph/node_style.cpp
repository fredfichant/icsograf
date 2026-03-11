/**
 * \file src/graph/node_style.cpp
 * \brief Implementation of node style graph logic.
 */

#include "node_style.hpp"

Node_Style Node_Style::default_to(const Node_Style& other) const
{
    return Node_Style((enabled_style & CUSP_ANGLE) ? cusp_angle : other.cusp_angle,
                      (enabled_style & HANDLE_LENGTH) ? handle_length : other.handle_length,
                      (enabled_style & CUSP_DISTANCE) ? cusp_distance : other.cusp_distance,
                      (enabled_style & CUSP_SHAPE) && cusp_shape ? cusp_shape : other.cusp_shape,
                      EVERYTHING);
}
