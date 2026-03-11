/**
 * \file src/database/graph_repository.hpp
 * \brief SQLite repository API for saving, querying, loading, and deleting graph records.
 */

#ifndef GRAPH_REPOSITORY_HPP
#define GRAPH_REPOSITORY_HPP

#include <QtGlobal>
#include <QByteArray>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

class Graph;
class Graph_Properties;
class Node;
class Edge;

struct Graph_Record
{
    qint64 id = -1;
    QString title;
    QString created_at;
    QString topology_hash;
    QByteArray graph_json;

    int node_count = 0;
    int edge_count = 0;
    int group_count = 0;
    int face_count = 0;

    int wa = 0;
    int w0 = 0;
    int p0 = 0;
    int pa = 0;
    int delta_t = 0;

    QString span_formula;
    bool is_non_reducible = false;

    QMap<int, int> vertex_degree_distribution;
    QMap<int, int> face_degree_distribution;
};

struct Graph_Query_Filter
{
    int node_count = -1;
    int edge_count = -1;
    int group_count = -1;
    int face_count = -1;

    int wa = -1;
    int w0 = -1;
    int p0 = -1;
    int pa = -1;
    int delta_t = -1;

    QString span_formula;
    QString title_contains;

    // -1 = ignore, 0 = false, 1 = true
    int is_non_reducible = -1;

    int limit = 200;
};

struct Graph_Save_Result
{
    bool ok = false;
    bool inserted = false;          // false si doublon ignoré
    qint64 diagram_id = -1;
    QString topology_hash;
    QString error;
};

class Graph_Repository : public QObject
{
    Q_OBJECT

public:
    explicit Graph_Repository(QObject* parent = nullptr);
    ~Graph_Repository() override;

    bool open(QString* error_message = nullptr);
    bool migrate(QString* error_message = nullptr);

    Graph_Save_Result save_graph(const Graph& graph, const QString& title = QString());

    QList<Graph_Record> find_graphs(const Graph_Query_Filter& filter,
                                    QString* error_message = nullptr) const;
    bool delete_graph(qint64 id, QString* error_message = nullptr);

    QList<Graph_Record> find_graphs_by_title(const QString& title_substring,
                                             int limit = 200,
                                             QString* error_message = nullptr) const;

    bool load_graph_record(qint64 id, Graph_Record* out_record,
                           QString* error_message = nullptr) const;

    bool deserialize_graph_into(const QByteArray& graph_json,
                                Graph& graph,
                                QList<Node*>& out_nodes,
                                QList<Edge*>& out_edges,
                                QString* error_message = nullptr) const;

    static QByteArray serialize_graph(const Graph& graph);
    static QByteArray serialize_properties_fingerprint(const Graph_Properties& properties);
    static QString topology_hash(const Graph_Properties& properties);

    QString database_path() const { return m_database_path; }
    QString connection_name() const { return m_connection_name; }

private:
    bool ensure_open(QString* error_message) const;
    static QString sha256_hex(const QByteArray& bytes);

    static QJsonObject graph_to_json_object(const Graph& graph);
    static QJsonObject properties_to_json_object(const Graph_Properties& properties);

    static QByteArray map_to_json(const QMap<int, int>& map);
    static QMap<int, int> json_to_map(const QByteArray& json);

    QString m_connection_name;
    QString m_database_path;
};

#endif  // GRAPH_REPOSITORY_HPP
