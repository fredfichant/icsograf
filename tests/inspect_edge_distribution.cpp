#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <iostream>

#include "database/graph_repository.hpp"
#include "edge_type.hpp"
#include "faces.hpp"
#include "edges_mark.hpp"
#include "io/xml_loader.hpp"
#include "icsograf_info.hpp"

namespace {

void print_usage(const QString& program)
{
    QTextStream err(stderr);
    err << "Usage:\n";
    err << "  " << program << " --list\n";
    err << "  " << program << " --xml <file>\n";
    err << "  " << program << " --graph-json <file>\n";
    err << "  " << program << " --record-id <id>\n";
}

bool load_from_xml(const QString& path, Graph& graph, QString* error_message)
{
    QFile file(path);
    if (!file.exists()) {
        if (error_message) *error_message = QStringLiteral("XML file not found: %1").arg(path);
        return false;
    }
    if (!import_xml(file, graph)) {
        if (error_message) *error_message = QStringLiteral("Failed to import XML: %1").arg(path);
        return false;
    }
    graph.render_knot();
    return true;
}

bool load_from_graph_json(const QString& path, Graph& graph, QString* error_message)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error_message)
            *error_message = QStringLiteral("Failed to read graph JSON file: %1").arg(path);
        return false;
    }

    Graph_Repository repo;
    QList<Node*> nodes;
    QList<Edge*> edges;
    const QByteArray graph_json = file.readAll();
    const bool ok = repo.deserialize_graph_into(graph_json, graph, nodes, edges, error_message);
    if (!ok) {
        for (Edge* edge : edges) delete edge;
        for (Node* node : nodes) delete node;
        return false;
    }
    return true;
}

bool load_from_record_id(qint64 record_id, Graph& graph, QString* error_message)
{
    Graph_Repository repo;
    if (!repo.open(error_message) || !repo.migrate(error_message)) return false;

    Graph_Record record;
    if (!repo.load_graph_record(record_id, &record, error_message)) return false;

    QList<Node*> nodes;
    QList<Edge*> edges;
    const bool ok = repo.deserialize_graph_into(record.graph_json, graph, nodes, edges, error_message);
    if (!ok) {
        for (Edge* edge : edges) delete edge;
        for (Node* node : nodes) delete node;
        return false;
    }
    return true;
}

bool list_records(QString* error_message)
{
    Graph_Repository repo;
    if (!repo.open(error_message) || !repo.migrate(error_message)) return false;

    QTextStream out(stdout);
    out << "database_path: " << repo.database_path() << "\n";

    Graph_Query_Filter filter;
    filter.limit = 50;
    const QList<Graph_Record> rows = repo.find_graphs(filter, error_message);
    if (!error_message->isEmpty()) return false;

    out << "record_count: " << rows.size() << "\n";
    for (const Graph_Record& rec : rows) {
        out << "id=" << rec.id << " title=" << rec.title << " edges=" << rec.edge_count
            << " groups=" << rec.group_count << " created_at=" << rec.created_at << "\n";
    }
    return true;
}

void print_graph_report(const Graph& graph, const QString& source_label)
{
    QTextStream out(stdout);
    const Graph_Properties* props = graph.properties();
    const std::vector<EdgeDistributionTable>& tables = props->edge_distribution_tables();
    const std::vector<std::vector<std::size_t>> faces = find_faces(graph);
    GraphMarker marker;
    const std::map<const Edge*, EdgeMarking> markings = marker.mark_graph(graph, faces);
    const std::vector<EdgeDistributionTable> linear_tables =
        marker.edge_distribution_tables_from_linear_solutions(graph, faces);
    const std::vector<std::vector<int>> linear_assignments =
        marker.edge_assignments_from_linear_solutions(graph, faces);

    out << "source: " << source_label << "\n";
    out << "node_count: " << props->node_count() << "\n";
    out << "edge_count: " << props->edge_count() << "\n";
    out << "group_count: " << props->group_count() << "\n";
    out << "face_count: " << props->face_count() << "\n";
    out << "table_count: " << tables.size() << "\n";
    out << "linear_table_count: " << linear_tables.size() << "\n";
    out << "linear_assignment_count: " << linear_assignments.size() << "\n";

    int edge_index = 0;
    for (Edge* edge : graph.edges()) {
        auto it = markings.find(edge);
        out << "edge[" << edge_index++ << "]"
            << " type=" << (edge->style().edge_type ? edge->style().edge_type->machine_name()
                                                    : QStringLiteral("null"))
            << " strands=" << edge->strand_count()
            << " marking=" << (it != markings.end() ? QString::fromStdString(it->second.marking)
                                                    : QStringLiteral("?"))
            << "\n";
    }

    for (std::size_t i = 0; i < tables.size(); ++i) {
        const EdgeDistributionTable& table = tables[i];
        const int total = table.wa + table.w0 + table.p0 + table.pa;
        out << "table[" << i << "]"
            << " label=" << QString::fromStdString(table.label)
            << " wa=" << table.wa
            << " w0=" << table.w0
            << " p0=" << table.p0
            << " pa=" << table.pa
            << " total=" << total
            << " matches_edge_count=" << (total == props->edge_count() ? "yes" : "no") << "\n";
    }
    for (std::size_t i = 0; i < linear_tables.size(); ++i) {
        const EdgeDistributionTable& table = linear_tables[i];
        const int total = table.wa + table.w0 + table.p0 + table.pa;
        out << "linear_table[" << i << "]"
            << " label=" << QString::fromStdString(table.label)
            << " wa=" << table.wa
            << " w0=" << table.w0
            << " p0=" << table.p0
            << " pa=" << table.pa
            << " total=" << total
            << " matches_edge_count=" << (total == props->edge_count() ? "yes" : "no") << "\n";
    }
    for (std::size_t i = 0; i < linear_assignments.size(); ++i) {
        out << "linear_assignment[" << i << "]=";
        for (std::size_t j = 0; j < linear_assignments[i].size(); ++j) {
            if (j != 0) out << ",";
            out << linear_assignments[i][j];
        }
        out << "\n";
    }
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral(TARGET));
    QCoreApplication::setOrganizationName(QStringLiteral(TARGET));
    const QStringList args = app.arguments();
    if (args.size() != 2 && args.size() != 3) {
        print_usage(args.value(0, QStringLiteral("inspect_edge_distribution")));
        return 2;
    }

    const QString mode = args[1];
    if (mode == QStringLiteral("--list")) {
        QString error;
        if (!list_records(&error)) {
            QTextStream(stderr) << error << "\n";
            return 1;
        }
        return 0;
    }

    if (args.size() != 3) {
        print_usage(args.value(0, QStringLiteral("inspect_edge_distribution")));
        return 2;
    }

    const QString value = args[2];

    Graph graph;
    QString error;
    bool ok = false;

    if (mode == QStringLiteral("--xml")) {
        ok = load_from_xml(value, graph, &error);
    } else if (mode == QStringLiteral("--graph-json")) {
        ok = load_from_graph_json(value, graph, &error);
    } else if (mode == QStringLiteral("--record-id")) {
        bool is_number = false;
        const qint64 record_id = value.toLongLong(&is_number);
        if (!is_number) {
            QTextStream(stderr) << "Invalid record id: " << value << "\n";
            return 2;
        }
        ok = load_from_record_id(record_id, graph, &error);
    } else {
        print_usage(args[0]);
        return 2;
    }

    if (!ok) {
        if (mode == QStringLiteral("--record-id")) {
            Graph_Repository repo;
            QTextStream(stderr) << error << "\n";
            QTextStream(stderr) << "database_path: " << repo.database_path() << "\n";
            QTextStream(stderr) << "tip: run --list to see available ids\n";
        } else {
            QTextStream(stderr) << error << "\n";
        }
        return 1;
    }

    print_graph_report(graph, value);

    const Graph_Properties* props = graph.properties();
    for (const EdgeDistributionTable& table : props->edge_distribution_tables()) {
        const int total = table.wa + table.w0 + table.p0 + table.pa;
        if (total != props->edge_count()) {
            QTextStream(stderr) << "Invariant failed for table "
                                << QString::fromStdString(table.label) << ": " << total
                                << " != " << props->edge_count() << "\n";
            return 1;
        }
    }

    return 0;
}
