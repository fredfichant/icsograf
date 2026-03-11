/**
 * \file src/graph/edge_2strand.hpp
 * \brief API declarations for edge 2-strand graph data structures and operations.
 */

#ifndef EDGE_2STRAND_HPP
#define EDGE_2STRAND_HPP

#include "edge_type.hpp"

/**
 * \brief Represents an edge composed of two parallel strands.
 *
 * This class handles the geometry and traversal logic for a standard 2-strand crossing.
 */
class Edge_2Strand : public Edge_Type
{
   public:
    /// Returns the number of strands (2)
    int strand_count() const override { return 2; }
    /// Computes the path through the edge and returns the exit handle
    Edge::Handle traverse(Edge* edge, Edge::Handle handle, Path_Builder& path) const override;
    /// Human-readable name of the edge type
    QString name() const override;
    /// Machine-readable identifier
    QString machine_name() const override;
    /// Calculates the control point geometry for the given handle
    QLineF handle(const Edge* edge, Edge::Handle handle) const override;
    /// Returns the icon associated with this edge type
    QIcon icon() const override { return QIcon::fromTheme("edge-2strand"); }
};

/**
 * \brief Represents an inverted 2-strand edge.
 *
 * In this variant, the crossing logic is inverted relative to the standard Edge_2Strand.
 */
class Edge_2Strand_Inverted : public Edge_2Strand
{
   public:
    /// Returns the number of strands (2)
    int strand_count() const override { return 2; }
    /// Custom paint method (draws a dashed line to indicate inversion)
    void paint(QPainter* painter, const Edge& edge) override;
    /// Computes the path through the edge (inverted logic)
    Edge::Handle traverse(Edge* edge, Edge::Handle handle, Path_Builder& path) const override;
    /// Human-readable name
    QString name() const override;
    /// Machine-readable identifier
    QString machine_name() const override;
    /// Returns the icon associated with this edge type
    QIcon icon() const override { return QIcon::fromTheme("edge-2strand-inverted"); }
};

#endif  // EDGE_2STRAND_HPP
