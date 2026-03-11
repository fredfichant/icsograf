/**
 * \file src/database/graph_repository_codec.cpp
 * \brief Serialization, hashing, and deserialization routines for graph repository records.
 */

#include "graph_repository.hpp"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "edge.hpp"
#include "edge_2strand.hpp"
#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "edge_type.hpp"
#include "graph.hpp"
#include "graph_properties.hpp"
#include "node.hpp"

namespace
{
const Edge_Type* edge_type_from_machine_name(const QString& machine_name)
{
    static Edge_Normal regular_type;
    static Edge_Inverted inverted_type;
    static Edge_2Strand two_strand_type;
    static Edge_2Strand_Inverted two_strand_inverted_type;
    static Edge_3Strand three_strand_type;
    static Edge_3Strand_Inverted three_strand_inverted_type;

    if (machine_name == "inverted")
        return &inverted_type;
    if (machine_name == "2strand")
        return &two_strand_type;
    if (machine_name == "2strand_inverted")
        return &two_strand_inverted_type;
    if (machine_name == "3strand")
        return &three_strand_type;
    if (machine_name == "3strand_inverted")
        return &three_strand_inverted_type;

    return &regular_type;
}
}  // namespace

QByteArray Graph_Repository::serialize_graph(const Graph& graph)
{
    return QJsonDocument(graph_to_json_object(graph)).toJson(QJsonDocument::Compact);
}

QByteArray Graph_Repository::serialize_properties_fingerprint(const Graph_Properties& properties)
{
    return QJsonDocument(properties_to_json_object(properties)).toJson(QJsonDocument::Compact);
}

QString Graph_Repository::topology_hash(const Graph_Properties& properties)
{
    return sha256_hex(serialize_properties_fingerprint(properties));
}

QString Graph_Repository::sha256_hex(const QByteArray& bytes)
{
    return QString::fromLatin1(QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex());
}

QJsonObject Graph_Repository::graph_to_json_object(const Graph& graph)
{
    QJsonObject root;
    root["version"] = 1;

    QJsonArray nodes_json;
    QJsonArray edges_json;

    QList<Node*> nodes = graph.nodes();
    QList<Edge*> edges = graph.edges();

    QMap<const Node*, int> node_ids;
    for (int i = 0; i < nodes.size(); ++i) {
        node_ids[nodes[i]] = i;

        QJsonObject n;
        n["id"] = i;
        n["x"] = nodes[i]->pos().x();
        n["y"] = nodes[i]->pos().y();
        nodes_json.append(n);
    }

    for (Edge* e : edges) {
        QJsonObject obj;
        obj["a"] = node_ids.value(e->vertex1(), -1);
        obj["b"] = node_ids.value(e->vertex2(), -1);

        const Edge_Type* type = e->style().edge_type;
        obj["type"] = type ? type->machine_name() : QStringLiteral("regular");
        obj["strand_count"] = e->strand_count();
        obj["spacing"] = e->spacing();

        edges_json.append(obj);
    }

    root["nodes"] = nodes_json;
    root["edges"] = edges_json;
    return root;
}

QJsonObject Graph_Repository::properties_to_json_object(const Graph_Properties& properties)
{
    auto map_to_object = [](const QMap<int, int>& map) {
        QJsonObject obj;
        for (QMap<int, int>::const_iterator it = map.constBegin(); it != map.constEnd(); ++it) {
            obj[QString::number(it.key())] = it.value();
        }
        return obj;
    };

    QJsonObject root;
    root["node_count"] = properties.node_count();
    root["edge_count"] = properties.edge_count();
    root["group_count"] = properties.group_count();
    root["face_count"] = properties.face_count();

    root["wa"] = properties.wa();
    root["w0"] = properties.w0();
    root["p0"] = properties.p0();
    root["pa"] = properties.pa();
    root["delta_t"] = properties.delta_t();

    root["span_formula"] = properties.span_formula();
    root["is_non_reducible"] = properties.is_non_reducible();

    root["vertex_degree_distribution"] = map_to_object(properties.vertex_degree_distribution());
    root["face_degree_distribution"] = map_to_object(properties.face_degree_distribution());

    return root;
}

QByteArray Graph_Repository::map_to_json(const QMap<int, int>& map)
{
    QJsonObject obj;
    for (QMap<int, int>::const_iterator it = map.constBegin(); it != map.constEnd(); ++it) {
        obj[QString::number(it.key())] = it.value();
    }
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

QMap<int, int> Graph_Repository::json_to_map(const QByteArray& json)
{
    QMap<int, int> out;

    const QJsonDocument doc = QJsonDocument::fromJson(json);
    if (!doc.isObject())
        return out;

    const QJsonObject obj = doc.object();
    for (QJsonObject::const_iterator it = obj.constBegin(); it != obj.constEnd(); ++it) {
        bool ok = false;
        const int key = it.key().toInt(&ok);
        if (!ok)
            continue;
        out[key] = it.value().toInt();
    }
    return out;
}

bool Graph_Repository::deserialize_graph_into(const QByteArray& graph_json,
                                              Graph& graph,
                                              QList<Node*>& out_nodes,
                                              QList<Edge*>& out_edges,
                                              QString* error_message) const
{
    out_nodes.clear();
    out_edges.clear();

    const QJsonDocument doc = QJsonDocument::fromJson(graph_json);
    if (!doc.isObject()) {
        if (error_message)
            *error_message = QStringLiteral("graph_json is not a JSON object");
        return false;
    }

    const QJsonObject root = doc.object();
    const QJsonArray nodes_json = root.value("nodes").toArray();
    const QJsonArray edges_json = root.value("edges").toArray();

    QMap<int, Node*> by_id;

    for (const QJsonValue& value : nodes_json) {
        if (!value.isObject())
            continue;

        const QJsonObject obj = value.toObject();
        const int id = obj.value("id").toInt(-1);
        const double x = obj.value("x").toDouble();
        const double y = obj.value("y").toDouble();

        if (id < 0)
            continue;

        Node* node = new Node(QPointF(x, y));
        graph.add_node(node);

        by_id[id] = node;
        out_nodes.push_back(node);
    }

    for (const QJsonValue& value : edges_json) {
        if (!value.isObject())
            continue;

        const QJsonObject obj = value.toObject();
        const int a = obj.value("a").toInt(-1);
        const int b = obj.value("b").toInt(-1);
        const QString type_name = obj.value("type").toString("regular");
        const int strand_count = obj.value("strand_count").toInt(1);
        const double spacing = obj.value("spacing").toDouble(10.0);

        Node* v1 = by_id.value(a, nullptr);
        Node* v2 = by_id.value(b, nullptr);

        if (!v1 || !v2)
            continue;

        Edge* edge = new Edge(v1, v2,
                              const_cast<Edge_Type*>(edge_type_from_machine_name(type_name)));

        edge->set_strand_count(strand_count);
        edge->set_spacing(spacing);

        graph.add_edge(edge);
        out_edges.push_back(edge);
    }

    graph.render_knot();
    return true;
}
