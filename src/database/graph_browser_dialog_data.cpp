/**
 * \file src/database/graph_browser_dialog_data.cpp
 * \brief Data loading, detail rendering, and actions for the graph browser dialog.
 */

#include <QBuffer>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSet>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <algorithm>

#include "edge.hpp"
#include "graph.hpp"
#include "graph_browser_dialog.hpp"
#include "image_exporter.hpp"
#include "node.hpp"

namespace {
QString svg_data_uri(const QByteArray& svg_bytes)
{
    return QStringLiteral("data:image/svg+xml;base64,%1")
        .arg(QString::fromLatin1(svg_bytes.toBase64()));
}

bool export_graph_svg_data_uri(const Graph& graph, bool draw_graph, QString* out_data_uri,
                               QString* error_message)
{
    if (!out_data_uri) {
        if (error_message) *error_message = QStringLiteral("out_data_uri is nullptr");
        return false;
    }

    QByteArray bytes;
    QBuffer buffer(&bytes);
    if (!buffer.open(QIODevice::WriteOnly)) {
        if (error_message) *error_message = QStringLiteral("failed to open SVG buffer");
        return false;
    }

    export_svg(buffer, graph, draw_graph, false);

    if (bytes.isEmpty()) {
        if (error_message) *error_message = QStringLiteral("empty SVG output");
        return false;
    }

    *out_data_uri = svg_data_uri(bytes);
    return true;
}

QList<int> sorted_distribution_keys(const Graph_Record& rec)
{
    QSet<int> keys;
    for (auto it = rec.vertex_degree_distribution.constBegin();
         it != rec.vertex_degree_distribution.constEnd(); ++it) {
        keys.insert(it.key());
    }
    for (auto it = rec.face_degree_distribution.constBegin();
         it != rec.face_degree_distribution.constEnd(); ++it) {
        keys.insert(it.key());
    }

    QList<int> sorted = keys.values();
    std::sort(sorted.begin(), sorted.end());
    return sorted;
}
}  // namespace

void Graph_Browser_Dialog::refresh_results()
{
    QString error;
    m_rows = m_repo.find_graphs(current_filter(), &error);

    if (!error.isEmpty()) {
        m_status_label->setText(QStringLiteral("Erreur : %1").arg(error));
        m_table->setRowCount(0);
        m_details_text->clear();
        m_open_button->setEnabled(false);
        m_delete_button->setEnabled(false);
        m_selected_id = -1;
        return;
    }

    populate_table(m_rows);
    m_status_label->setText(QStringLiteral("%1 résultat(s)").arg(m_rows.size()));
}

void Graph_Browser_Dialog::populate_table(const QList<Graph_Record>& rows)
{
    m_table->setRowCount(0);

    for (int i = 0; i < rows.size(); ++i) {
        const Graph_Record& rec = rows[i];
        m_table->insertRow(i);

        auto* id_item = new QTableWidgetItem(QString::number(rec.id));
        id_item->setData(Qt::UserRole, rec.id);

        m_table->setItem(i, 0, id_item);
        m_table->setItem(i, 1, new QTableWidgetItem(rec.title));
        m_table->setItem(i, 2, new QTableWidgetItem(rec.created_at));
        m_table->setItem(i, 3, new QTableWidgetItem(QString::number(rec.edge_count)));
        m_table->setItem(i, 4, new QTableWidgetItem(QString::number(rec.group_count)));
        m_table->setItem(i, 5, new QTableWidgetItem(QString::number(rec.face_count)));
        m_table->setItem(i, 6, new QTableWidgetItem(QString::number(rec.wa)));
        m_table->setItem(i, 7, new QTableWidgetItem(QString::number(rec.w0)));
        m_table->setItem(i, 8, new QTableWidgetItem(QString::number(rec.p0)));
        m_table->setItem(i, 9, new QTableWidgetItem(QString::number(rec.pa)));
        m_table->setItem(i, 10, new QTableWidgetItem(QString::number(rec.delta_t)));
    }

    if (!rows.isEmpty()) {
        m_table->selectRow(0);
    } else {
        m_selected_id = -1;
        m_open_button->setEnabled(false);
        m_delete_button->setEnabled(false);
        m_details_text->clear();
    }
}

qint64 Graph_Browser_Dialog::selected_graph_id() const
{
    const QList<QTableWidgetItem*> items = m_table->selectedItems();
    if (items.isEmpty()) return -1;

    QTableWidgetItem* first = m_table->item(items.first()->row(), 0);
    if (!first) return -1;

    return first->data(Qt::UserRole).toLongLong();
}

void Graph_Browser_Dialog::update_details_panel()
{
    const qint64 id = selected_graph_id();
    if (id < 0) {
        m_details_text->clear();
        return;
    }

    for (const Graph_Record& rec : m_rows) {
        if (rec.id != id) continue;

        QString graph_svg_data_uri;
        QString svg_error;
        if (!build_svg_data_uris(m_repo, rec, &graph_svg_data_uri, &svg_error)) {
            graph_svg_data_uri.clear();
            m_status_label->setText(QStringLiteral("%1 résultat(s) - SVG indisponible (%2)")
                                        .arg(m_rows.size())
                                        .arg(svg_error));
        }

        m_details_text->setHtml(build_details_html(rec, graph_svg_data_uri));
        return;
    }

    m_details_text->clear();
}

void Graph_Browser_Dialog::on_selection_changed()
{
    m_selected_id = selected_graph_id();
    const bool has_selection = (m_selected_id >= 0);
    m_open_button->setEnabled(has_selection);
    m_delete_button->setEnabled(has_selection);
    update_details_panel();
}

void Graph_Browser_Dialog::on_open_clicked()
{
    if (selected_graph_id() < 0) {
        QMessageBox::warning(this, QStringLiteral("Sélection"),
                             QStringLiteral("Aucun graphe sélectionné."));
        return;
    }

    accept();
}

void Graph_Browser_Dialog::on_delete_clicked()
{
    const qint64 id = selected_graph_id();
    if (id < 0) return;

    const auto answer =
        QMessageBox::question(this, QStringLiteral("Suppression"),
                              QStringLiteral("Supprimer définitivement le graphe sélectionné ?"));

    if (answer != QMessageBox::Yes) return;

    QString error;
    if (!m_repo.delete_graph(id, &error)) {
        QMessageBox::warning(this, QStringLiteral("Suppression"),
                             QStringLiteral("Échec de la suppression :\n%1").arg(error));
        return;
    }

    refresh_results();
}

bool Graph_Browser_Dialog::load_selected_graph(Graph& graph, QList<Node*>& out_nodes,
                                               QList<Edge*>& out_edges, QString* error_message)
{
    const qint64 id = selected_graph_id();
    if (id < 0) {
        if (error_message) *error_message = QStringLiteral("aucun graphe sélectionné");
        return false;
    }

    Graph_Record rec;
    QString error;
    if (!m_repo.load_graph_record(id, &rec, &error)) {
        if (error_message) *error_message = error;
        return false;
    }

    if (!m_repo.deserialize_graph_into(rec.graph_json, graph, out_nodes, out_edges, &error)) {
        if (error_message) *error_message = error;
        return false;
    }

    return true;
}

QString Graph_Browser_Dialog::build_details_html(const Graph_Record& rec,
                                                 const QString& graph_svg_data_uri)
{
    const QList<int> keys = sorted_distribution_keys(rec);

    QString html;
    html += QStringLiteral("<h3>Détails</h3>");
    html += QStringLiteral("<p><b>ID :</b> %1<br/>").arg(rec.id);
    html += QStringLiteral("<b>Titre :</b> %1<br/>").arg(rec.title.toHtmlEscaped());
    html += QStringLiteral("<b>Créé le :</b> %1</p>").arg(rec.created_at.toHtmlEscaped());

    html += QStringLiteral("<table border='1' cellspacing='0' cellpadding='4'>");
    html += QStringLiteral("<tr><th>C</th><th>R</th><th>S</th><th>F</th></tr>");
    html += QStringLiteral("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
                .arg(rec.edge_count)
                .arg(rec.group_count)
                .arg(rec.node_count)
                .arg(rec.face_count);
    html += QStringLiteral("</table><br/>");
    html += QStringLiteral("<h3>table d'état</h3>");
    html += QStringLiteral("<table border='1' cellspacing='0' cellpadding='6'>");
    html +=
        QStringLiteral("<tr><td>wp<br/>%1</td><td>w'p<br/>%2</td></tr>").arg(rec.wa).arg(rec.pa);
    html +=
        QStringLiteral("<tr><td>wp'<br/>%1</td><td>w'p'<br/>%2</td></tr>").arg(rec.w0).arg(rec.p0);
    html += QStringLiteral("</table><br/>");

    html += QStringLiteral("<p><b>&#x0394; T :</b> %1<br/>").arg(rec.delta_t);
    html += QStringLiteral("<b>portance :</b> %1<br/>").arg(rec.span_formula.toHtmlEscaped());
    html += QStringLiteral("<b>Non réductible :</b> %1</p>")
                .arg(rec.is_non_reducible ? QStringLiteral("oui") : QStringLiteral("non"));

    html += QStringLiteral("<h3>Distribution des degrés</h3>");
    html += QStringLiteral("<table border='1' cellspacing='0' cellpadding='4'>");
    html += QStringLiteral("<tr><th>Type</th>");
    for (int degree : keys) {
        html += QStringLiteral("<th>%1</th>").arg(degree);
    }
    html += QStringLiteral("</tr>");

    html += QStringLiteral("<tr><td><b>Sommets</b></td>");
    for (int degree : keys) {
        html += QStringLiteral("<td>%1</td>").arg(rec.vertex_degree_distribution.value(degree, 0));
    }
    html += QStringLiteral("</tr>");

    html += QStringLiteral("<tr><td><b>Faces</b></td>");
    for (int degree : keys) {
        html += QStringLiteral("<td>%1</td>").arg(rec.face_degree_distribution.value(degree, 0));
    }
    html += QStringLiteral("</tr>");
    html += QStringLiteral("</table><br/>");

    html += QStringLiteral("<h3>Représentation</h3>");
    if (!graph_svg_data_uri.isEmpty()) {
        html += QStringLiteral("<img src='%1' width='360' style='background-color: white;'/>")
                    .arg(graph_svg_data_uri);
    } else {
        html += QStringLiteral("<p><b>Graphe</b> : indisponible</p>");
    }

    return html;
}

bool Graph_Browser_Dialog::build_svg_data_uris(const Graph_Repository& repo,
                                               const Graph_Record& rec,
                                               QString* out_graph_svg_data_uri,
                                               QString* error_message)
{
    if (!out_graph_svg_data_uri) {
        if (error_message) *error_message = QStringLiteral("output SVG pointer is null");
        return false;
    }

    Graph graph;
    QList<Node*> nodes;
    QList<Edge*> edges;
    QString error;
    const bool ok = repo.deserialize_graph_into(rec.graph_json, graph, nodes, edges, &error);
    if (!ok) {
        if (error_message) *error_message = error;
        return false;
    }

    const bool graph_ok = export_graph_svg_data_uri(graph, true, out_graph_svg_data_uri, &error);

    for (Edge* edge : edges) {
        delete edge;
    }
    for (Node* node : nodes) {
        delete node;
    }

    if (!graph_ok) {
        if (error_message) *error_message = error;
        return false;
    }

    return true;
}
