/**
 * \file tests/test_graph_browser_dialog_details.cpp
 * \brief Unit tests for graph browser dialog details.
 */

#include <cassert>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QSqlDatabase>
#include <QTableWidget>

#include "database/graph_browser_dialog.hpp"
#include "database/graph_repository.hpp"
#include "edge_2strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
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
    Edge_2Strand two_strand;
    Node a(QPointF(-200, 0));
    Node b(QPointF(-50, 80));
    Node c(QPointF(120, 60));
    Node d(QPointF(80, -120));
    Edge e1(&a, &b, &normal);
    Edge e2(&b, &c, &normal);
    Edge e3(&c, &d, &two_strand);

    Graph g;
    g.add_node(&a);
    g.add_node(&b);
    g.add_node(&c);
    g.add_node(&d);
    g.add_edge(&e1);
    g.add_edge(&e2);
    g.add_edge(&e3);
    g.render_knot();

    const Graph_Save_Result save = repo.save_graph(g, "details-panel");
    assert(save.ok);

    Graph_Record rec;
    assert(repo.load_graph_record(save.diagram_id, &rec, &error));

    Graph_Browser_Dialog dialog;
    dialog.show();
    app.processEvents();

    QTableWidget* results_table = nullptr;
    QTableWidget* distribution_table = nullptr;
    const QList<QTableWidget*> tables = dialog.findChildren<QTableWidget*>();
    int invariant_table_count = 0;
    for (QTableWidget* table : tables) {
        if (table->columnCount() == 10) results_table = table;
        if (table->objectName() == QStringLiteral("detailsDistributionTable")) {
            distribution_table = table;
        }
        if (table->objectName() == QStringLiteral("detailsInvariantsTable")) {
            invariant_table_count++;
        }
    }

    assert(results_table != nullptr);
    assert(distribution_table != nullptr);
    assert(results_table->rowCount() >= 1);

    results_table->selectRow(0);
    app.processEvents();

    QLabel* title = dialog.findChild<QLabel*>(QStringLiteral("detailsTitle"));
    QLabel* id_value = dialog.findChild<QLabel*>(QStringLiteral("detailsIdValue"));
    QLabel* short_signature =
        dialog.findChild<QLabel*>(QStringLiteral("detailsShortSignatureValue"));
    QLabel* json_length = dialog.findChild<QLabel*>(QStringLiteral("detailsJsonLengthValue"));
    QLabel* node_count = dialog.findChild<QLabel*>(QStringLiteral("detailsNodeCountValue"));
    QLabel* edge_count = dialog.findChild<QLabel*>(QStringLiteral("detailsEdgeCountValue"));
    QLabel* non_reducible =
        dialog.findChild<QLabel*>(QStringLiteral("detailsNonReducibleValue"));

    assert(title != nullptr);
    assert(id_value != nullptr);
    assert(short_signature != nullptr);
    assert(json_length != nullptr);
    assert(node_count != nullptr);
    assert(edge_count != nullptr);
    assert(non_reducible != nullptr);

    assert(title->text() == QStringLiteral("details-panel"));
    assert(id_value->text() == QString::number(rec.id));
    assert(short_signature->text() == rec.topology_hash.left(12));
    assert(json_length->text() == QStringLiteral("%1 octets").arg(rec.graph_json.size()));
    assert(node_count->text() == QString::number(rec.node_count));
    assert(edge_count->text() == QString::number(rec.edge_count));
    assert(non_reducible->text() == QStringLiteral("oui") ||
           non_reducible->text() == QStringLiteral("non"));

    assert(distribution_table->rowCount() >= 1);
    assert(distribution_table->columnCount() >= 1);
    assert(distribution_table->item(0, 0) != nullptr);

    const QList<QTableWidget*> updated_tables = dialog.findChildren<QTableWidget*>();
    invariant_table_count = 0;
    for (QTableWidget* table : updated_tables) {
        if (table->objectName() == QStringLiteral("detailsInvariantsTable")) {
            invariant_table_count++;
        }
    }
    assert(invariant_table_count >= 2);

    QGraphicsView* preview_view =
        dialog.findChild<QGraphicsView*>(QStringLiteral("detailsPreviewView"));
    assert(preview_view != nullptr);
    assert(preview_view->scene() != nullptr);
    assert(!preview_view->scene()->items().isEmpty());

    QPushButton* copy_button =
        dialog.findChild<QPushButton*>(QStringLiteral("copyDetailsButton"));
    QPushButton* export_button =
        dialog.findChild<QPushButton*>(QStringLiteral("exportSvgButton"));
    QPushButton* enlarge_button =
        dialog.findChild<QPushButton*>(QStringLiteral("enlargePreviewButton"));
    assert(copy_button != nullptr);
    assert(export_button != nullptr);
    assert(enlarge_button != nullptr);
    assert(copy_button->isEnabled());
    assert(export_button->isEnabled());
    assert(enlarge_button->isEnabled());

    return 0;
}
