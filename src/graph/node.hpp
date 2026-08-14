/**
 * \file node.hpp
 * \brief Node representation in the knot graph
 * \author Mattia Basaglia
 */

#ifndef NODE_HPP
#define NODE_HPP

#include <QPainter>
#include <QPointF>

#include "graph_item.hpp"
#include "node_style.hpp"

class Edge;

/**
 * \brief Represents a vertex (intersection or cusp) in the knot graph
 *
 * Nodes are connected by Edges. A Node can have multiple edges connected to it,
 * depending on its type (degree).
 */
class Node : public Graph_Item
{
    Q_OBJECT

   public:
    static int radius;                ///< Radius used to display the node
    static QColor color_resting;      ///< Color used to display the node (resting)
    static QColor color_highlighted;  ///< Color used to display the node (highlighted)
    static QColor color_selected;     ///< Color used to display the node (selected outline)

   private:
    QList<Edge*> m_edges;  ///< List of edges connected to this node
    Node_Style m_style;    ///< Style overrides for this specific node

   public:
    /**
     * \brief Constructs a Node at a given position
     * \param pos The initial position of the node
     */
    Node(QPointF pos);

    /**
     * \brief Access the node's style overrides
     * \return Reference to the Node_Style object
     */
    Node_Style& style() { return m_style; }
    const Node_Style& style() const { return m_style; }

    /**
     * \brief Sets style overrides for this node
     * \param st The new node style
     */
    void set_style(Node_Style st) { m_style = st; }

    /**
     * \brief Adds an edge to the node's internal list
     * \param e Edge to be added
     */
    void add_edge(Edge* e);

    /**
     * \brief Removes an edge from the node's internal list
     * \param e Edge to be removed
     * \pre e is in the edge list
     */
    void remove_edge(Edge* e);

    /**
     * \brief Checks if there is an edge connecting this node to another
     * \param n The other node
     * \return true if an edge exists, false otherwise
     */
    bool has_edge_to(const Node* n) const;

    /**
     * \brief Gets the edge connecting this node to another
     * \param n The other node
     * \return The Edge object if it exists, nullptr otherwise
     */
    Edge* edge_to(const Node* n) const;

    /**
     * \brief Returns the list of all edges connected to this node
     */
    QList<Edge*> edges() const { return m_edges; }

    /**
     * \brief Returns the type ID for QGraphicsItem casting
     */
    int type() const override { return UserType + 0x01; }

    /**
     * \brief Paints the node on the given painter
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* = 0, QWidget* = 0) override;

    /**
     * \brief Returns the bounding rectangle of the node
     */
    QRectF boundingRect() const override;

    /**
     * \brief Moves the node to a new position
     * \param p The new position
     * \note Use this instead of setPos to ensure proper signaling
     */
    void move(QPointF p);

    /**
     * \brief Returns the radius used for collision detection and display
     */
    static int external_radius() { return radius + 1; }

   signals:
    /**
     * \brief Emitted when the node's position changes
     * \param p The new position
     */
    void moved(QPointF p);
};

#endif  // NODE_HPP
