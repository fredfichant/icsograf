/**
 * \file xml_loader_private.hpp
 * \brief Internal XML parsing for Knotter graphs
 */

#ifndef XML_LOADER_PRIVATE_HPP
#define XML_LOADER_PRIVATE_HPP

#include <QMap>
#include <QString>
#include <QXmlStreamReader>

#include "graph.hpp"

/**
 * \brief Internal class for reading Knot XML files (Versions 3 and 4)
 *
 * This class uses QXmlStreamReader for high-performance, stream-based XML parsing.
 * It is not part of the public API and should be accessed through import_xml().
 */
class XML_Loader
{
    QMap<QString, Node*> nodes;  ///< Mapping of IDs to nodes for edge linking
    QXmlStreamReader xml;        ///< The XML stream reader
    int m_version;               ///< Version of the XML file currently being loaded

    static const int min_version = 3;  ///< Minimum version supported by this loader
    static const int max_version = 4;  ///< Maximum version supported by this loader

    /**
     * \brief Dispatches parsing of top-level knot elements
     * \param graph The graph to populate
     */
    void read_knot(Graph* graph);

    /**
     * \brief Parses the <style> element
     * \param graph The graph to populate
     */
    void read_style(Graph* graph);

    /**
     * \brief Parses the <colors> element
     * \param graph The graph to populate
     */
    void read_colors(Graph* graph);

    /**
     * \brief Parses the <cusp> element (default node style)
     * \param graph The graph to populate
     * \param default_style If true, the style is set as default for the graph
     */
    void read_cusp(Graph* graph, bool default_style);

    /**
     * \brief Parses the <crossing> element (default edge style)
     * \param graph The graph to populate
     * \param default_style If true, the style is set as default for the graph
     */
    void read_crossing(Graph* graph, bool default_style);

    /**
     * \brief Parses the <stroke> element for graph appearance
     * \param graph The graph to populate
     */
    void read_stroke(Graph* graph);

    /**
     * \brief Parses the <graph> element containing nodes and edges
     * \param graph The graph to populate
     */
    void read_graph(Graph* graph);

    /**
     * \brief Parses the <nodes> list element
     * \param graph The graph to populate
     */
    void read_nodes(Graph* graph);

    /**
     * \brief Parses a single <node> element
     * \param graph The graph to populate
     */
    void read_node(Graph* graph);

    /**
     * \brief Parses the <edges> list element
     * \param graph The graph to populate
     */
    void read_edges(Graph* graph);

    /**
     * \brief Parses a single <edge> element
     * \param graph The graph to populate
     */
    void read_edge(Graph* graph);

    /**
     * \brief Helper for parsing a style description into a Node_Style struct
     * \return The populated Node_Style struct
     */
    Node_Style read_node_style_element();

    /**
     * \brief Helper for parsing a style description into an Edge_Style struct
     * \return The populated Edge_Style struct
     */
    Edge_Style read_edge_style_element();

    /**
     * \brief Helper for parsing a <color> element
     * \return The parsed QColor object
     */
    QColor read_color();

   public:
    /**
     * \brief Constructs an XML_Loader
     */
    XML_Loader() : m_version(0) {}

    /**
     * \brief Loads graph data from the input device
     * \param input The device to read from
     * \param graph The graph to populate
     * \return true if loading was successful, false otherwise
     */
    bool load(QIODevice* input, Graph* graph);

    /**
     * \brief Loads only the <style> part from the input device
     * \param input The device to read from
     * \param graph The graph to apply the style to
     */
    void load_style(QIODevice* input, Graph* graph);

    /**
     * \brief Returns the version of the Knot file loaded
     */
    int version() const { return m_version; }

    /**
     * \brief Checks if the given version is supported by this loader
     * \param v The version to check
     * \return true if the version is supported, false otherwise
     */
    static bool supports_version(int v) { return v >= min_version && v <= max_version; }
};

#endif  // XML_LOADER_PRIVATE_HPP
