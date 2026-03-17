/**
 * \file graph.hpp
 * \brief Graph representation and rendering for knots
 * \author fred fichant
 */

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <QGraphicsItem>
#include <QList>
#include <QPainterPath>
#include <QPen>
#include <QRectF>

#include "edge.hpp"
#include "graph_appearance.hpp"
#include "graph_properties.hpp"

class QColor;
class Graph_Analyzer;
class Graph_Renderer;
class Node;
class Path_Builder;
class QPainter;
class QStyleOptionGraphicsItem;
struct Traversal_Info;
class QWidget;

/**
 * \brief Class that represents the knot (as a graph) and renders it
 *
 * This class serves as the container for all Nodes and Edges of the knot.
 * It provides methods for building the knot, rendering it into painter paths,
 * and performing traversals.
 *
 * \note The Graph never takes ownership of its nodes and edges; they must be
 *       managed externally (usually by a Scene object).
 */
class Graph : public QGraphicsItem
{
   private:
    QList<Node*> m_nodes;             ///< List of nodes in the graph
    QList<Edge*> m_edges;             ///< List of edges in the graph
    friend class Graph_Analyzer;
    friend class Graph_Renderer;

    QRectF bounding_box;              ///< Calculated bounding rectangle of the entire knot
    QList<QPainterPath> paths;        ///< Rendered knot (one per loop)
    Graph_Appearance m_appearance;    ///< Aggregated style and rendering settings
    Graph_Properties* m_properties;   ///< Object holding graph statistics and properties

   public:
    /**
     * \brief Constructs an empty Graph
     */
    explicit Graph();

    /**
     * \brief Copy constructor for Graph
     */
    Graph(const Graph& other);

    /**
     * \brief Assignment operator for Graph
     */
    Graph& operator=(const Graph& other);

    /**
     * \brief Destructor for Graph
     */
    ~Graph();

    /**
     * \brief Copies style settings from another graph
     * \param other The source graph
     */
    void copy_style(const Graph& other);

    /**
     * \brief Adds a node to the graph
     * \param n Node to be added
     * \pre n is not already in the graph
     * \post n is in the graph
     * \note Signals from n are connected to this graph
     */
    void add_node(Node* n);

    /**
     * \brief Adds an edge to the graph
     * \param e Edge to be added
     * \pre e is not already in the graph
     * \post e is in the graph
     */
    void add_edge(Edge* e);

    /**
     * \brief Removes a node from the graph
     * \param n Node to be removed
     * \pre n is in the graph and has no connected edges
     * \post n is not in the graph
     */
    void remove_node(Node* n);

    /**
     * \brief Removes an edge from the graph
     * \param e Edge to be removed
     * \pre e is in the graph
     * \post e is not in the graph
     */
    void remove_edge(Edge* e);

    /**
     * \brief Returns the list of nodes
     */
    QList<Node*> nodes() const { return m_nodes; }

    /**
     * \brief Returns the list of edges
     */
    QList<Edge*> edges() const { return m_edges; }

    /**
     * \brief Returns the list of strand colors
     */
    const QList<QColor>& colors() const { return m_appearance.colors; }

    /**
     * \brief Sets the list of strand colors
     * \param l New color list
     */
    void set_colors(const QList<QColor>& l);

    /**
     * \brief Returns the stroke width for the strands
     */
    double width() const { return m_appearance.pen.widthF(); }

    /**
     * \brief Sets the stroke width for the strands
     * \param w New width
     */
    void set_width(double w);

    /**
     * \brief Returns the pen join style for strands
     */
    Qt::PenJoinStyle join_style() const { return m_appearance.pen.joinStyle(); }

    /**
     * \brief Sets the pen join style for strands
     * \param style New join style
     */
    void set_join_style(Qt::PenJoinStyle style);

    /**
     * \brief Returns the brush style for strand stroking
     */
    Qt::BrushStyle brush_style() const;

    /**
     * \brief Sets the brush style for strand stroking
     * \param style New brush style
     */
    void set_brush_style(Qt::BrushStyle style);

    /**
     * \brief Returns true if custom colors are enabled
     */
    bool custom_colors() const { return !m_appearance.auto_color; }

    /**
     * \brief Toggles between automatic and custom strand colors
     * \param b true to enable custom colors
     */
    void set_custom_colors(bool b) { m_appearance.auto_color = !b; }

    /**
     * \brief Returns the graph's default node style
     */
    Node_Style default_node_style() const { return m_appearance.default_node_style; }

    /**
     * \brief Accesses the graph's default node style as a reference
     */
    Node_Style& default_node_style_reference() { return m_appearance.default_node_style; }

    /**
     * \brief Sets the graph's default node style
     * \param style New style
     */
    void set_default_node_style(Node_Style style);

    /**
     * \brief Returns the graph's default edge style
     */
    Edge_Style default_edge_style() const { return m_appearance.default_edge_style; }

    /**
     * \brief Accesses the graph's default edge style as a reference
     */
    Edge_Style& default_edge_style_reference() { return m_appearance.default_edge_style; }

    /**
     * \brief Sets the graph's default edge style
     * \param style New style
     */
    void set_default_edge_style(Edge_Style style);

    /**
     * \brief Returns the graph's properties object
     */
    Graph_Properties* properties() const { return m_properties; }

    /**
     * \brief Recalculates graph statistics and properties
     */
    void update_properties() const;

    /**
     * \brief Renders the knot strands
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option = nullptr,
               QWidget* widget = nullptr) override;

    /**
     * \brief Constant version of the paint method
     */
    void const_paint(QPainter* painter, const QStyleOptionGraphicsItem* option = nullptr,
                     QWidget* widget = nullptr) const;

    /**
     * \brief Paints the underlying graph nodes and edges
     */
    void paint_graph(QPainter* painter, const QStyleOptionGraphicsItem* option = nullptr,
                     QWidget* widget = nullptr) const;

    /**
     * \brief Returns the bounding rectangle of the Graph
     */
    QRectF boundingRect() const override { return bounding_box; }

    /**
     * \brief Returns the full bounding rect, including large miter joints
     */
    QRectF full_image_bounding_rect() const;

    /**
     * \brief Returns the type ID for QGraphicsItem casting
     */
    int type() const override { return UserType + 0x03; }

    /**
     * \brief Traverses the graph and builds the strands' painter paths
     */
    void render_knot();

    /**
     * \brief Creates a subgraph from a set of nodes
     * \param nodes List of nodes to include
     * \return A copy of the graph including only specified elements
     */
    Graph sub_graph(QList<Node*> nodes) const;

    /**
     * \brief Toggles the DeviceCoordinateCache
     */
    void enable_cache(bool enable);

    /**
     * \brief Returns true if graphics cache is enabled
     */
    bool cache_enabled() const;

   private:
    /**
     * \brief Draws a single strand segment during traversal
     * \param path Path builder object
     * \param ti Traversal information for the segment
     */
    void draw_segment(Path_Builder& path, const Traversal_Info& ti) const;

    /**
     * \brief Traverses the entire graph to generate all paths
     * \param path Path builder object
     */
    void traverse(Path_Builder& path);

    /**
     * \brief Helper for traversing a single edge from a given handle
     */
    Traversal_Info traverse(Edge* edge, Edge::Handle handle, Path_Builder& path);

    /**
     * \brief Recalculates the bounding box of the entire Graph
     */
    void update_bounding_box();
};

#endif  // GRAPH_HPP
