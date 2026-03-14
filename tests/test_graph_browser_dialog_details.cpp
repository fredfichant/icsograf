/**
 * \file tests/test_graph_browser_dialog_details.cpp
 * \brief Unit tests for graph browser dialog details.
 */

#include <cassert>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>

#include "database/graph_browser_dialog.hpp"
#include "database/graph_repository.hpp"
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
        return 0;
    }
    if (!repo.migrate(&error)) {
        return 0;
    }

    Edge_Normal normal;
    Node a(QPointF(0, 0));
    Node b(QPointF(100, 0));
    Node c(QPointF(50, 80));
    Edge e1(&a, &b, &normal);
    Edge e2(&b, &c, &normal);
    Edge e3(&c, &a, &normal);

    Graph g;
    g.add_node(&a);
    g.add_node(&b);
    g.add_node(&c);
    g.add_edge(&e1);
    g.add_edge(&e2);
    g.add_edge(&e3);
    g.render_knot();

    const Graph_Save_Result save = repo.save_graph(g, "details-panel");
    assert(save.ok);

    Graph_Record rec;
    assert(repo.load_graph_record(save.diagram_id, &rec, &error));

    QString graph_svg_data_uri;
    assert(Graph_Browser_Dialog::build_svg_data_uris(repo, rec, &graph_svg_data_uri, &error));
    assert(graph_svg_data_uri.startsWith(QStringLiteral("data:image/svg+xml;base64,")));

    const QString html = Graph_Browser_Dialog::build_details_html(rec, graph_svg_data_uri);
    assert(!html.contains(QStringLiteral("Hash topologique")));
    assert(html.contains(QStringLiteral("<table")));
    assert(html.contains(QStringLiteral("wa")));
    assert(html.contains(QStringLiteral("w0")));
    assert(html.contains(QStringLiteral("p0")));
    assert(html.contains(QStringLiteral("pa")));
    assert(html.contains(QStringLiteral("Sommets")));
    assert(html.contains(QStringLiteral("Faces")));
    assert(html.contains(QStringLiteral("Représentations SVG")));
    assert(!html.contains(QStringLiteral("Nœud")));
    assert(html.contains(QStringLiteral("data:image/svg+xml;base64,")));

    return 0;
}
