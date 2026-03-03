#include "xml_loader_private.hpp"

#include <QMetaEnum>
#include <QObject>

#include "resource_manager.hpp"

bool XML_Loader::load(QIODevice* input, Graph* graph)
{
    m_version = 0;
    nodes.clear();
    xml.setDevice(input);

    if (xml.readNextStartElement()) {
        if (xml.name() == "knot") {
            m_version = xml.attributes().value("version").toInt();
            if (m_version >= min_version && m_version <= max_version) {
                read_knot(graph);
                return !xml.hasError();
            }
        }
    }
    return false;
}

void XML_Loader::load_style(QIODevice* input, Graph* graph)
{
    xml.setDevice(input);
    if (xml.readNextStartElement()) {
        if (xml.name() == "style") {
            read_style(graph);
        }
    }
}

void XML_Loader::read_knot(Graph* graph)
{
    while (xml.readNextStartElement()) {
        if (xml.name() == "style") {
            read_style(graph);
        } else if (xml.name() == "graph") {
            read_graph(graph);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void XML_Loader::read_style(Graph* graph)
{
    while (xml.readNextStartElement()) {
        if (xml.name() == "colors") {
            read_colors(graph);
        } else if (xml.name() == "cusp") {
            read_cusp(graph, true);
        } else if (xml.name() == "crossing") {
            read_crossing(graph, true);
        } else if (xml.name() == "stroke") {
            read_stroke(graph);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void XML_Loader::read_colors(Graph* graph)
{
    QList<QColor> colors;
    while (xml.readNextStartElement()) {
        if (xml.name() == "color") {
            colors.push_back(read_color());
        } else {
            xml.skipCurrentElement();
        }
    }
    if (colors.isEmpty()) {
        colors.push_back(Qt::black);
    }
    graph->set_colors(colors);
}

QColor XML_Loader::read_color()
{
    QColor c = Qt::black;
    QString alpha = xml.attributes().value("alpha").toString();
    QString colorStr = xml.readElementText().trimmed();

    c.setNamedColor(colorStr);
    c.setAlpha(alpha.isEmpty() ? 255 : alpha.toInt());
    return c;
}

void XML_Loader::read_cusp(Graph* graph, bool default_style)
{
    Node_Style ns = read_node_style_element();
    if (default_style) {
        ns.enabled_style = Node_Style::EVERYTHING;
        if (!ns.cusp_shape) {
            ns.cusp_shape = resource_manager().default_cusp_shape();
        }
        graph->set_default_node_style(ns);
    }
}

void XML_Loader::read_crossing(Graph* graph, bool default_style)
{
    Edge_Style es = read_edge_style_element();
    if (default_style) {
        es.enabled_style = Edge_Style::EVERYTHING;
        if (!es.edge_type) {
            es.edge_type = resource_manager().default_edge_type();
        }
        graph->set_default_edge_style(es);
    }
}

void XML_Loader::read_stroke(Graph* graph)
{
    while (xml.readNextStartElement()) {
        if (xml.name() == "width") {
            graph->set_width(xml.readElementText().toDouble());
        } else if (xml.name() == "style") {
            QString style_name = xml.readElementText().trimmed();
            QMetaEnum bs_me = QMetaEnum::fromType<Qt::BrushStyle>();
            graph->set_brush_style(
                static_cast<Qt::BrushStyle>(bs_me.keyToValue(style_name.toStdString().c_str())));
        } else if (xml.name() == "join") {
            QString style_name = xml.readElementText().trimmed();
            QMetaEnum pjs_me = QMetaEnum::fromType<Qt::PenJoinStyle>();
            graph->set_join_style(
                static_cast<Qt::PenJoinStyle>(pjs_me.keyToValue(style_name.toStdString().c_str())));
        } else {
            xml.skipCurrentElement();
        }
    }
}

void XML_Loader::read_graph(Graph* graph)
{
    while (xml.readNextStartElement()) {
        if (xml.name() == "nodes") {
            read_nodes(graph);
        } else if (xml.name() == "edges") {
            read_edges(graph);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void XML_Loader::read_nodes(Graph* graph)
{
    while (xml.readNextStartElement()) {
        if (xml.name() == "node") {
            read_node(graph);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void XML_Loader::read_node(Graph* graph)
{
    QString id = xml.attributes().value("id").toString();
    double x = xml.attributes().value("x").toDouble();
    double y = xml.attributes().value("y").toDouble();

    Node_Style ns;
    while (xml.readNextStartElement()) {
        if (xml.name() == "style") {
            ns = read_node_style_element();
        } else {
            xml.skipCurrentElement();
        }
    }

    if (!id.isEmpty() && !nodes.contains(id)) {
        Node* n = new Node(QPointF(x, y));
        n->set_style(ns);
        nodes[id] = n;
        graph->add_node(n);
    }
}

void XML_Loader::read_edges(Graph* graph)
{
    while (xml.readNextStartElement()) {
        if (xml.name() == "edge") {
            read_edge(graph);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void XML_Loader::read_edge(Graph* graph)
{
    QString id1 = xml.attributes().value("v1").toString();
    QString id2 = xml.attributes().value("v2").toString();
    QString type = xml.attributes().value("type").toString();
    if (type.isEmpty()) {
        type = xml.attributes().value("style").toString();
    }

    Edge_Style es;
    while (xml.readNextStartElement()) {
        if (xml.name() == "style") {
            es = read_edge_style_element();
        } else {
            xml.skipCurrentElement();
        }
    }

    if (nodes.contains(id1) && nodes.contains(id2)) {
        Edge* e = new Edge(nodes[id1], nodes[id2]);
        es.enabled_style |= Edge_Style::EDGE_TYPE;
        es.edge_type = resource_manager().edge_type_from_machine_name(type);
        e->set_style(es);
        graph->add_edge(e);
    }
}

Node_Style XML_Loader::read_node_style_element()
{
    Node_Style ns;
    while (xml.readNextStartElement()) {
        if (xml.name() == "shape") {
            ns.enabled_style |= Node_Style::CUSP_SHAPE;
            ns.cusp_shape = resource_manager().cusp_shape_from_machine_name(xml.readElementText());
        } else if (xml.name() == "angle" || xml.name() == "min-angle") {
            ns.enabled_style |= Node_Style::CUSP_ANGLE;
            ns.cusp_angle = xml.readElementText().toDouble();
        } else if (xml.name() == "distance") {
            ns.enabled_style |= Node_Style::CUSP_DISTANCE;
            ns.cusp_distance = xml.readElementText().toDouble();
        } else if (xml.name() == "curve" || xml.name() == "handle-length") {
            ns.enabled_style |= Node_Style::HANDLE_LENGTH;
            ns.handle_length = xml.readElementText().toDouble();
        } else {
            xml.skipCurrentElement();
        }
    }
    return ns;
}

Edge_Style XML_Loader::read_edge_style_element()
{
    Edge_Style es;
    while (xml.readNextStartElement()) {
        if (xml.name() == "gap") {
            es.enabled_style |= Edge_Style::CROSSING_DISTANCE;
            es.crossing_distance = xml.readElementText().toDouble();
        } else if (xml.name() == "slide") {
            es.enabled_style |= Edge_Style::EDGE_SLIDE;
            es.edge_slide = xml.readElementText().toDouble();
        } else if (xml.name() == "curve" || xml.name() == "handle-length") {
            es.enabled_style |= Edge_Style::HANDLE_LENGTH;
            es.handle_length = xml.readElementText().toDouble();
        } else {
            xml.skipCurrentElement();
        }
    }
    return es;
}
