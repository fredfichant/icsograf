#include "xml_exporter.hpp"

#include <QBuffer>

#include "../graph/edge.hpp"
#include "../graph/edge_style.hpp"
#include "../graph/graph.hpp"
#include "../graph/node.hpp"
#include "../graph/node_style.hpp"
#include "image_exporter.hpp"
#include "resource_manager.hpp"

XML_Exporter::XML_Exporter(QIODevice* output, bool pretty_xml) : xml(output)
{
    if (pretty_xml) {
        xml.setAutoFormatting(true);
        xml.setAutoFormattingIndent(4);
    }
}

void XML_Exporter::export_graph(const Graph* graph)
{
    begin();

    // Extract style components from the graph object
    const QList<QColor>& colors = graph->colors();
    const Node_Style& node_style = graph->default_node_style();
    const Edge_Style& edge_style = graph->default_edge_style();

    // Call save_style with the extracted components
    save_style(colors, node_style, edge_style);

    start_element("graph");

    start_element("nodes");
    for (Node* node : graph->nodes()) save_node(node);
    end_element();  // nodes

    start_element("edges");
    for (Edge* edge : graph->edges()) save_edge(edge);
    end_element();  // edges

    end();
}

void XML_Exporter::begin()
{
    xml.writeStartDocument("1.0");
    xml.writeStartElement("knot");
    xml.writeAttribute("version", QString::number(version));
    xml.writeAttribute("generator", QString("%1 %2")
                                        .arg(resource_manager().program.name())
                                        .arg(resource_manager().program.version()));
}

void XML_Exporter::end() { xml.writeEndDocument(); }

void XML_Exporter::start_element(QString name) { xml.writeStartElement(name); }

void XML_Exporter::end_element() { xml.writeEndElement(); }

void XML_Exporter::save_style(const QList<QColor>& colors, const Node_Style& node_style,
                              const Edge_Style& edge_style)
{
    start_element("style");

    start_element("colors");
    for (QColor c : colors) save_color("color", c);
    end_element();  // colors

    save_cusp("cusp", node_style);
    save_crossing("crossing", edge_style);

    end_element();  // style
}

void XML_Exporter::save_cusp(QString name, const Node_Style& style)
{
    start_element(name);
    if (style.enabled_style & Node_Style::CUSP_SHAPE)
        xml.writeTextElement("shape", style.cusp_shape->machine_name());

    if (style.enabled_style & Node_Style::CUSP_ANGLE)
        xml.writeTextElement("angle", QString::number(style.cusp_angle));

    if (style.enabled_style & Node_Style::CUSP_DISTANCE)
        xml.writeTextElement("distance", QString::number(style.cusp_distance));

    if (style.enabled_style & Node_Style::HANDLE_LENGTH)
        xml.writeTextElement("curve", QString::number(style.handle_length));

    end_element();
}

void XML_Exporter::save_crossing(QString name, const Edge_Style& style)
{
    start_element(name);

    if (style.enabled_style & Edge_Style::CROSSING_DISTANCE)
        xml.writeTextElement("gap", QString::number(style.crossing_distance));

    if (style.enabled_style & Edge_Style::EDGE_SLIDE)
        xml.writeTextElement("slide", QString::number(style.edge_slide));

    if (style.enabled_style & Edge_Style::HANDLE_LENGTH)
        xml.writeTextElement("curve", QString::number(style.handle_length));

    end_element();
}

void XML_Exporter::save_node(Node* node)
{
    int id = node_id(node);

    start_element("node");
    xml.writeAttribute("id", QString("node_%1").arg(id));
    xml.writeAttribute("x", QString::number(node->pos().x()));
    xml.writeAttribute("y", QString::number(node->pos().y()));
    if (node->style().enabled_style != Node_Style::NOTHING) save_cusp("style", node->style());
    end_element();  // node
}

void XML_Exporter::save_edge(Edge* edge)
{
    start_element("edge");
    xml.writeAttribute("type", edge->style().edge_type->machine_name());
    xml.writeAttribute("v1", QString("node_%1").arg(node_id(edge->vertex1())));
    xml.writeAttribute("v2", QString("node_%1").arg(node_id(edge->vertex2())));
    if (edge->style().enabled_style & (Edge_Style::EVERYTHING ^ Edge_Style::EDGE_TYPE))
        save_crossing("style", edge->style());
    end_element();  // edge
}

void XML_Exporter::save_color(QString name, QColor col)
{
    start_element(name);
    xml.writeAttribute("alpha", QString::number(col.alpha()));
    xml.writeCharacters(col.name());
    end_element();
}

int XML_Exporter::node_id(Node* node)
{
    if (node_ids.contains(node))
        return node_ids[node];
    else
        return node_ids[node] = node_ids.size();
}

bool export_xml(const Graph& graph, QIODevice& file)
{
    if (!file.isWritable() && !file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    XML_Exporter(&file).export_graph(&graph);

    return true;
}

void export_xml_mime_data(QMimeData* data, const Graph& graph)
{
    QByteArray knot_xml;
    QBuffer xml_stream(&knot_xml);
    export_xml(graph, xml_stream);

    data->setData("application/x-knotter", knot_xml);

    if (resource_manager().settings.clipboard_feature(Settings::XML))
        data->setData("text/xml", knot_xml);

    if (resource_manager().settings.clipboard_feature(Settings::SVG)) {
        QByteArray knot_svg;
        QBuffer svg_stream(&knot_svg);
        export_svg(svg_stream, graph, false);
        data->setData("image/svg+xml", knot_svg);
    }
}

QByteArray export_xml_style(const QList<QColor>& colors, const Node_Style& node_style,
                            const Edge_Style& edge_style)
{
    QByteArray output;
    QBuffer buffer(&output);
    buffer.open(QIODevice::WriteOnly);
    XML_Exporter(&buffer, false).save_style(colors, node_style, edge_style);
    return output;
}
