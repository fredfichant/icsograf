#ifndef EDGE_HANDLE_CALCULATOR_HPP
#define EDGE_HANDLE_CALCULATOR_HPP

#include <QLineF>
#include "edge_handle.hpp"

class Edge;

/**
 * \brief Strategy class for calculating control handle geometry.
 *
 * This class encapsulates the logic for determining the position and angle
 * of Bezier control points for edge strands.
 */
class Edge_Handle_Calculator
{
public:
    virtual ~Edge_Handle_Calculator() = default;

    /**
     * \brief Calculates the control handle for a given edge and handle identifier.
     * \param edge The edge instance.
     * \param handle The handle (location and strand).
     * eturn A QLineF from anchor to control point.
     */
    virtual QLineF calculate(const Edge* edge, Edge_Handle handle) const = 0;
};

/**
 * \brief Standard calculator for diagonal control handles.
 *
 * This is the default implementation used by most standard edge types.
 */
class Standard_Edge_Handle_Calculator : public Edge_Handle_Calculator
{
public:
    QLineF calculate(const Edge* edge, Edge_Handle handle) const override;

    /**
     * \brief Static access to the standard calculation logic.
     */
    static QLineF standard_handle(const Edge* edge, Edge_Handle handle);
};

#endif // EDGE_HANDLE_CALCULATOR_HPP
