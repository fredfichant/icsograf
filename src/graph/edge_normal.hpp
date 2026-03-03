#ifndef EDGE_NORMAL_HPP
#define EDGE_NORMAL_HPP

#include "edge_type.hpp"

/**
 * \brief Represents a standard edge type.
 *
 * This class handles the geometry and traversal logic for a standard crossing.
 */
class Edge_Normal : public Edge_Type
{
   public:
    /// Computes the path through the edge and returns the exit handle
    Edge::Handle traverse(Edge* edge, Edge::Handle handle, Path_Builder& path) const override;
    /// Human-readable name of the edge type
    QString name() const override;
    /// Machine-readable identifier
    QString machine_name() const override;
    /// Calculates the control point geometry for the given handle
    QLineF handle(const Edge* edge, Edge::Handle handle) const override;
    /// Returns the icon associated with this edge type
    QIcon icon() const override { return QIcon::fromTheme("edge-crossing"); }
};

/**
 * \brief Represents an inverted edge type.
 *
 * In this variant, the crossing logic is inverted relative to the standard Edge_Normal.
 */
class Edge_Inverted : public Edge_Normal
{
   public:
    /// Custom paint method (draws a dashed line to indicate inversion)
    void paint(QPainter* painter, const Edge& edge) override;
    /// Computes the path through the edge (inverted logic)
    Edge::Handle traverse(Edge* edge, Edge::Handle handle, Path_Builder& path) const override;
    /// Human-readable name
    QString name() const override;
    /// Machine-readable identifier
    QString machine_name() const override;
    /// Returns the icon associated with this edge type
    QIcon icon() const override { return QIcon::fromTheme("edge-inverted"); }
};

#endif  // EDGE_NORMAL_HPP
