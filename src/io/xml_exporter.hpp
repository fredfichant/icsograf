/**
 * \file xml_exporter.hpp
 * \brief XML serialization for Knotter graphs
 */

#ifndef XML_EXPORTER_HPP
#define XML_EXPORTER_HPP

#include <QColor>
#include <QList>
#include <QMap>
#include <QMimeData>
#include <QString>
#include <QXmlStreamWriter>

class Graph;
class Node;
class Edge;
class Node_Style;
class Edge_Style;

/**
 * \brief Class responsible for exporting a Graph object to XML format
 *
 * This class uses QXmlStreamWriter for efficient, stream-based XML generation.
 * It handles the serialization of nodes, edges, styles, and colors.
 */
class XML_Exporter
{
    /// Version of the Knot file format supported by this exporter
    static const int version = 4;

    QXmlStreamWriter xml;       ///< The XML stream writer
    QMap<Node*, int> node_ids;  ///< Mapping of nodes to unique IDs for edge serialization

   public:
    /**
     * \brief Constructs an XML_Exporter
     * \param output The device to write the XML data to
     * \param pretty_xml Whether to use automatic formatting (indentation)
     */
    XML_Exporter(QIODevice* output, bool pretty_xml = true);

    /**
     * \brief Exports the entire graph to the XML output
     * \param graph The graph to export
     */
    void export_graph(const Graph* graph);

    /**
     * \brief Saves the style information (colors, node/edge defaults) to the XML output
     * \param colors List of colors used in the graph
     * \param node_style Default style for nodes
     * \param edge_style Default style for edges
     */
    void save_style(const QList<QColor>& colors, const Node_Style& node_style,
                    const Edge_Style& edge_style);

   protected:
    /**
     * \brief Writes the XML document header and root element
     */
    void begin();

    /**
     * \brief Closes the root element and finishes the document
     */
    void end();

    /**
     * \brief Starts a new XML element
     * \param name The name of the element
     */
    void start_element(QString name);

    /**
     * \brief Ends the current XML element
     */
    void end_element();

    /**
     * \brief Saves a node style (cusp)
     * \param name The element name
     * \param style The node style to save
     */
    void save_cusp(QString name, const Node_Style& style);

    /**
     * \brief Saves an edge style (crossing)
     * \param name The element name
     * \param style The edge style to save
     */
    void save_crossing(QString name, const Edge_Style& style);

    /**
     * \brief Saves a single node
     * \param node The node to save
     */
    void save_node(Node* node);

    /**
     * \brief Saves a single edge
     * \param edge The edge to save
     */
    void save_edge(Edge* edge);

    /**
     * \brief Saves a color
     * \param name The element name
     * \param col The color to save
     */
    void save_color(QString name, QColor col);

    /**
     * \brief Gets or generates a unique ID for a node
     * \param node The node to get the ID for
     * \return The unique integer ID
     */
    int node_id(Node* node);
};

/**
 * \brief High-level function to export a graph to a file or device
 * \param graph The graph to export
 * \param file The output device
 * \return true if successful, false otherwise
 */
bool export_xml(const Graph& graph, QIODevice& file);

/**
 * \brief Exports graph data to QMimeData for clipboard or drag-and-drop
 * \param data The MIME data object to populate
 * \param graph The graph to export
 */
void export_xml_mime_data(QMimeData* data, const Graph& graph);

/**
 * \brief Exports only the style information as a byte array
 * \param colors List of colors
 * \param node_style Node style
 * \param edge_style Edge style
 * \return QByteArray containing the XML representation of the style
 */
QByteArray export_xml_style(const QList<QColor>& colors, const Node_Style& node_style,
                            const Edge_Style& edge_style);

#endif  // XML_EXPORTER_HPP
