/**
 * \file tests/test_graph_repository.cpp
 * \brief Unit tests for graph repository.
 */

#include <cassert>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>

#include "database/graph_repository.hpp"
#include "edge_2strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    const QStringList plugin_paths = {QStringLiteral("/usr/local/opt/qt@5/plugins"),
                                      QStringLiteral("/opt/homebrew/opt/qt@5/plugins")};
    for (const QString& path : plugin_paths) {
        if (QDir(path).exists()) {
            app.addLibraryPath(path);
        }
    }

    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
        return 0;
    }

    Graph_Repository repo;
    QFile::remove(repo.database_path());

    QString error;
    if (!repo.open(&error)) {
        qWarning() << "Skipping test_graph_repository: cannot open SQLite DB:" << error;
        return 0;
    }
    assert(repo.migrate(&error));

    Edge_Normal normal;
    Edge_2Strand two_strand;

    Node a1(QPointF(0, 0));
    Node b1(QPointF(100, 0));
    Node c1(QPointF(50, 80));
    Edge e1(&a1, &b1, &normal);
    Edge e2(&b1, &c1, &normal);
    Edge e3(&c1, &a1, &normal);
    Graph g1;
    g1.add_node(&a1);
    g1.add_node(&b1);
    g1.add_node(&c1);
    g1.add_edge(&e1);
    g1.add_edge(&e2);
    g1.add_edge(&e3);
    g1.render_knot();

    const Graph_Save_Result s1 = repo.save_graph(g1, "alpha");
    assert(s1.ok);
    assert(s1.inserted);
    assert(s1.diagram_id > 0);

    const Graph_Save_Result s1_dup = repo.save_graph(g1, "alpha-duplicate");
    assert(s1_dup.ok);
    assert(!s1_dup.inserted);
    assert(s1_dup.diagram_id == s1.diagram_id);

    Node a2(QPointF(0, 0));
    Node b2(QPointF(100, 0));
    Node c2(QPointF(50, 80));
    Edge e4(&a2, &b2, &normal);
    Edge e5(&b2, &c2, &normal);
    Edge e6(&c2, &a2, &two_strand);
    Graph g2;
    g2.add_node(&a2);
    g2.add_node(&b2);
    g2.add_node(&c2);
    g2.add_edge(&e4);
    g2.add_edge(&e5);
    g2.add_edge(&e6);
    g2.render_knot();

    const Graph_Save_Result s2 = repo.save_graph(g2, "beta");
    assert(s2.ok);
    assert(s2.inserted);
    assert(s2.diagram_id != s1.diagram_id);

    QList<Graph_Record> title_hits = repo.find_graphs_by_title("alpha", 20, &error);
    assert(error.isEmpty());
    assert(title_hits.size() == 1);
    assert(title_hits.first().id == s1.diagram_id);

    Graph_Query_Filter filter;
    filter.edge_count = 4;
    filter.limit = 20;
    QList<Graph_Record> filtered = repo.find_graphs(filter, &error);
    assert(error.isEmpty());
    assert(filtered.size() == 1);
    assert(filtered.first().id == s2.diagram_id);

    Graph_Record loaded;
    assert(repo.load_graph_record(s2.diagram_id, &loaded, &error));

    Graph restored;
    QList<Node*> restored_nodes;
    QList<Edge*> restored_edges;
    assert(repo.deserialize_graph_into(loaded.graph_json, restored, restored_nodes, restored_edges, &error));
    assert(restored_nodes.size() == 3);
    assert(restored_edges.size() == 3);

    assert(repo.delete_graph(s2.diagram_id, &error));
    Graph_Record removed;
    assert(!repo.load_graph_record(s2.diagram_id, &removed, &error));

    for (Edge* edge : restored_edges) {
        delete edge;
    }
    for (Node* node : restored_nodes) {
        delete node;
    }

    return 0;
}
