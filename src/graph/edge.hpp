/**
 * \file edge.hpp
 * \brief Edge representation in the knot graph
 * \author fred fichant
 */

#ifndef EDGE_HPP
#define EDGE_HPP

#include <QLineF>

#include "edge_handle.hpp"
#include "edge_style.hpp"
#include "graph_item.hpp"
#include "node.hpp"
#include "point_math.hpp"

/**
 * \brief Represents a connection between two Nodes in the knot graph
 *
 * Edges represent the strands of the knot. Each edge connects exactly two nodes.
 * The way strands cross each other is determined by the Edge_Style.
 */
class Edge : public Graph_Item
{
    Q_OBJECT

   public:
    static QColor color_resting;      ///< Color used to display the edge (resting)
    static QColor color_highlighted;  ///< Color used to display the edge (highlighted)
    static QColor color_selected;     ///< Color used to display the edge (selected outline)

    typedef Edge_Handle Handle;  ///< Typedef for edge traversal handle
    typedef Edge_Handle_Namespace::Handle_Flags Handle_Flags;  ///< Typedef for set of handles

   private:
    Node* v1;                        ///< First vertex connected to this edge
    Node* v2;                        ///< Second vertex connected to this edge
    Edge_Style m_style;              ///< Style overrides for this specific edge
    Handle_Flags available_handles;  ///< Set of untraversed handles for path generation
    const Graph* m_graph;            ///< The graph this edge belongs to
    QColor m_marking_color;          ///< Optional color for marking edges in the editor

    static const int shapew = 8;  ///< Width used for shape() calculation
   public:
    /**
     * \brief Constructs an Edge between two nodes
     * \param v1 The first node
     * \param v2 The second node
     * \param type The initial edge type (optional)
     */
    explicit Edge(Node* v1, Node* v2, Edge_Type* type = nullptr);

    /**
     * \brief Sets the parent graph for this edge
     * \param g The graph object
     */
    void set_graph(const Graph* g) { m_graph = g; }

    /**
     * \brief Returns the parent graph
     */
    const Graph* graph() const { return m_graph; }

    /**
     * \brief Checks if the given node is one of this edge's endpoints
     * \param node The node to check
     * \return true if it is v1 or v2, false otherwise
     */
    bool is_vertex(const Node* node) const { return node == v1 or node == v2; }

    /**
     * \brief Returns the first node (v1)
     */
    Node* vertex1() const { return v1; }

    /**
     * \brief Returns the second node (v2)
     */
    Node* vertex2() const { return v2; }

    /**
     * \brief Gets the node at the other end of the edge
     * \param n One of the endpoint nodes
     * \return The opposite node, or nullptr if n is not part of this edge
     */
    Node* other(const Node* n) const { return n == v1 ? v2 : (n == v2 ? v1 : nullptr); }

    /**
     * \brief Sets style overrides for this edge
     * \param st The new edge style
     */
    void set_style(Edge_Style st);

    /**
     * \brief Returns the overridden style features for this edge
     */
    Edge_Style style() const;

    /**
     * \brief Returns a reference to the style overrides
     */
    Edge_Style& style();

    /**
     * \brief Returns the effective style, combining overrides with graph defaults
     */
    Edge_Style defaulted_style() const;

    /**
     * \brief Returns the number of strands for this edge
     */
    int strand_count() const;

    /**
     * \brief Sets the number of strands
     * \param count New strand count
     */
    void set_strand_count(int count);

    /**
     * \brief Returns the spacing between strands
     */
    double spacing() const;

    /**
     * \brief Sets the spacing between strands
     * \param s New spacing
     */
    void set_spacing(double s);

    /**
     * \brief Returns a QLineF representing the edge
     */
    QLineF to_line() const { return QLineF(v1->pos(), v2->pos()); }

    /**
     * \brief Returns the center point of the edge
     */
    QPointF midpoint() const { return (v1->pos() + v2->pos()) / 2; }

    /**
     * \brief Returns the closest point on the edge line to a given point
     * \param p Target point
     * \return The snapped point
     */
    QPointF snap(QPointF p) const { return project(p, to_line()); }

    /**
     * \brief Returns the editor marking color
     */
    QColor marking_color() const { return m_marking_color; }

    /**
     * \brief Sets the editor marking color
     * \param c New color
     */
    void set_marking_color(const QColor& c) { m_marking_color = c; }

    /**
     * \brief Returns the bounding rectangle for graphics rendering
     */
    QRectF boundingRect() const override;

    /**
     * \brief Returns the selection/collision shape for this edge
     */
    QPainterPath shape() const override;

    /**
     * \brief Paints the edge on the given painter
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* = 0, QWidget* = 0) override;

    /**
     * \brief Returns the type ID for QGraphicsItem casting
     */
    int type() const override { return UserType + 0x02; }

    /**
     * \brief Marks all handles as untraversed (for knot rendering)
     */
    void reset()
    {
        int strands = defaulted_style().strand_count;
        available_handles = Handle_Flags();
        for (int s = 0; s < strands; ++s) {
            Edge_Handle strand_bit = (Edge_Handle) (s << 12);
            // Include Level 0, 1, and 2 handles
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::TOP_LEFT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::TOP_RIGHT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::BOTTOM_LEFT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::BOTTOM_RIGHT | strand_bit);

            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::MID_TOP_LEFT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::MID_TOP_RIGHT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::MID_BOTTOM_LEFT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::MID_BOTTOM_RIGHT | strand_bit);

            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::CENTER_TOP_LEFT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::CENTER_TOP_RIGHT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::CENTER_BOTTOM_LEFT | strand_bit);
            available_handles |= (Edge_Handle) (Edge_Handle_Namespace::CENTER_BOTTOM_RIGHT | strand_bit);
        }
    }

    /**
     * \brief Marks a specific handle as traversed
     * \param h The handle to mark
     */
    void mark_traversed(Handle h) { available_handles &= ~h; }

    /**
     * \brief Checks if a handle has already been traversed
     * \param handle The handle to check
     * \return true if traversed, false if available
     */
    bool traversed(Handle handle) const { return !(available_handles & handle); }

    /**
     * \brief Returns an arbitrary untraversed handle
     * \return An available handle, or NO_HANDLE if all are traversed
     */
    Handle not_traversed() const
    {
        int strands = defaulted_style().strand_count;
        for (int s = 0; s < strands; ++s) {
            Edge_Handle strand_bit = (Edge_Handle) (s << 12);
            if (available_handles & (Edge_Handle) (Edge_Handle_Namespace::TOP_LEFT | strand_bit))
                return (Edge_Handle) (Edge_Handle_Namespace::TOP_LEFT | strand_bit);
            if (available_handles & (Edge_Handle) (Edge_Handle_Namespace::TOP_RIGHT | strand_bit))
                return (Edge_Handle) (Edge_Handle_Namespace::TOP_RIGHT | strand_bit);
            if (available_handles & (Edge_Handle) (Edge_Handle_Namespace::BOTTOM_LEFT | strand_bit))
                return (Edge_Handle) (Edge_Handle_Namespace::BOTTOM_LEFT | strand_bit);
            if (available_handles & (Edge_Handle) (Edge_Handle_Namespace::BOTTOM_RIGHT | strand_bit))
                return (Edge_Handle) (Edge_Handle_Namespace::BOTTOM_RIGHT | strand_bit);
        }
        return Edge_Handle_Namespace::NO_HANDLE;
    }

    /**
     * \brief Returns the endpoint node associated with a specific handle
     * \param handle The traversal handle
     * \return v1 or v2 depending on the handle direction
     */
    Node* vertex_for(Handle handle) const
    {
        Handle pure_handle = (Handle) (handle & Edge_Handle_Namespace::HANDLE_MASK);
        return (pure_handle == Edge_Handle_Namespace::TOP_RIGHT ||
                pure_handle == Edge_Handle_Namespace::BOTTOM_RIGHT)
                   ? v2
                   : v1;
    }

   public slots:
    /**
     * \brief Detaches this edge from its endpoint nodes
     * \note Removes this edge from the nodes' internal edge lists
     */
    void detach();

    /**
     * \brief Attaches this edge to its endpoint nodes
     * \note Adds this edge to the nodes' internal edge lists
     */
    void attach();
};

Q_DECLARE_METATYPE(Edge::Handle_Flags)

#endif  // EDGE_HPP
