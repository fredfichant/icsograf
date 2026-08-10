/**
 * \file src/database/graph_browser_dialog_data.cpp
 * \brief Data loading, detail rendering, and actions for the graph browser dialog.
 */

#include <QBuffer>
#include <QClipboard>
#include <QDialog>
#include <QAbstractItemView>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGuiApplication>
#include <QHeaderView>
#include <QImage>
#include <QLabel>
#include <QLayoutItem>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QSet>
#include <QSvgRenderer>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QVBoxLayout>
#include <algorithm>

#include "edge.hpp"
#include "edges_mark.hpp"
#include "faces.hpp"
#include "graph.hpp"
#include "graph_browser_dialog.hpp"
#include "image_exporter.hpp"
#include "node.hpp"

namespace {
constexpr int kMaxDisplayedEdgeDistributionTables = 8;

/**
 * \brief Returns a shortened version of the topology hash.
 * \param topology_hash The full topology hash string.
 * \return The first 12 characters of the hash.
 */
QString short_signature(const QString& topology_hash) { return topology_hash.left(12); }

/**
 * \brief Renders SVG data into a QPixmap for previewing.
 * \param svg_bytes The SVG data as a byte array.
 * \param max_size The maximum size constraints for the output pixmap.
 * \return A QPixmap containing the rendered SVG, or a null QPixmap if rendering fails.
 */
QPixmap render_svg_preview(const QByteArray& svg_bytes, const QSize& max_size)
{
    QSvgRenderer renderer(svg_bytes);
    if (!renderer.isValid()) return QPixmap();

    QSize render_size = renderer.defaultSize();
    if (render_size.isEmpty()) {
        render_size = renderer.viewBox().size();
    }
    if (render_size.isEmpty()) render_size = QSize(640, 480);

    render_size.scale(max_size, Qt::KeepAspectRatio);
    if (render_size.width() < 1) render_size.setWidth(1);
    if (render_size.height() < 1) render_size.setHeight(1);

    QImage image(render_size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    renderer.render(&painter);
    painter.end();

    return QPixmap::fromImage(image);
}

/**
 * \brief Exports a Graph to SVG format as a byte array.
 * \param graph The graph to export.
 * \param draw_graph Whether to draw the graph structure.
 * \param out_svg_bytes Pointer to store the output SVG byte array.
 * \param error_message Optional pointer to store any error message.
 * \return True if export succeeds, false otherwise.
 */
bool export_graph_svg_bytes(const Graph& graph, bool draw_graph, QByteArray* out_svg_bytes,
                            QString* error_message)
{
    if (!out_svg_bytes) {
        if (error_message) *error_message = QStringLiteral("out_svg_bytes is nullptr");
        return false;
    }

    QBuffer buffer(out_svg_bytes);
    if (!buffer.open(QIODevice::WriteOnly)) {
        if (error_message) *error_message = QStringLiteral("failed to open SVG buffer");
        return false;
    }

    export_svg(buffer, graph, draw_graph, false);

    if (out_svg_bytes->isEmpty()) {
        if (error_message) *error_message = QStringLiteral("empty SVG output");
        return false;
    }

    return true;
}

/**
 * \brief Gathers and sorts all unique degree keys from both vertex and face distributions.
 * \param rec The graph record containing the distributions.
 * \return A sorted list of degree integers.
 */
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

/**
 * \brief Builds an expanded textual representation of a degree distribution.
 * \param distribution The degree -> count map to serialize.
 * \return A string like "2210", starting at degree 2 and filling missing degrees with 0.
 */
QString distribution_summary(const QMap<int, int>& distribution)
{
    if (distribution.isEmpty()) {
        return QString();
    }

    const int max_degree = distribution.lastKey();
    QString summary;
    for (int degree = 2; degree <= max_degree; ++degree) {
        summary += QString::number(distribution.value(degree, 0));
    }
    return summary;
}

/**
 * \brief Returns the display text for a span formula in the results table.
 * \param span_formula The stored span formula.
 * \return "métabole" when the formula contains only 1 and | characters, otherwise the formula.
 */
QString span_formula_summary(const QString& span_formula)
{
    if (!span_formula.isEmpty()) {
        bool is_metabole = true;
        for (QChar c : span_formula) {
            if (c != QChar('1') && c != QChar('|')) {
                is_metabole = false;
                break;
            }
        }
        if (is_metabole) {
            return QStringLiteral("métabole");
        }
    }

    return span_formula;
}

/**
 * \brief Creates a UI table widget displaying an EdgeDistributionTable.
 * \param table The edge distribution table data.
 * \param parent The parent widget.
 * \return A configured QTableWidget displaying the distribution table.
 */
QTableWidget* create_edge_distribution_table(const EdgeDistributionTable& table, QWidget* parent)
{
    auto* widget = new QTableWidget(parent);
    widget->setObjectName(QStringLiteral("detailsInvariantsTable"));
    widget->setRowCount(2);
    widget->setColumnCount(2);
    widget->setHorizontalHeaderLabels(QStringList() << QStringLiteral("w") << QStringLiteral("w'"));
    widget->setVerticalHeaderLabels(QStringList() << QStringLiteral("p") << QStringLiteral("p'"));
    widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    widget->setSelectionMode(QAbstractItemView::NoSelection);
    widget->setFocusPolicy(Qt::NoFocus);
    widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    widget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    widget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    widget->horizontalHeader()->setDefaultSectionSize(72);
    widget->verticalHeader()->setDefaultSectionSize(58);
    widget->horizontalHeader()->setMinimumSectionSize(72);
    widget->verticalHeader()->setMinimumSectionSize(58);
    widget->setCornerButtonEnabled(false);
    widget->setShowGrid(true);
    widget->setMinimumHeight(180);
    widget->setMaximumHeight(180);

    const struct {
        int row;
        int column;
        int value;
    } cells[] = {
        {0, 0, table.wa},
        {1, 0, table.w0},
        {1, 1, table.p0},
        {0, 1, table.pa},
    };

    for (const auto& cell : cells) {
        auto* item = new QTableWidgetItem(QString::number(cell.value));
        item->setTextAlignment(Qt::AlignCenter);
        widget->setItem(cell.row, cell.column, item);
    }

    return widget;
}

/**
 * \brief Recursively clears and deletes all widgets within a layout.
 * \param layout The QVBoxLayout to clear.
 */
void clear_layout(QVBoxLayout* layout)
{
    if (!layout) return;
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

/**
 * \brief Computes the edge distribution tables for a graph record.
 * \param repo The graph repository for deserialization.
 * \param rec The graph record containing the serialized graph JSON.
 * \return A vector of computed EdgeDistributionTable objects.
 */
std::vector<EdgeDistributionTable> compute_edge_distribution_tables(const Graph_Repository& repo,
                                                                   const Graph_Record& rec)
{
    Graph graph;
    QList<Node*> nodes;
    QList<Edge*> edges;
    QString error;
    std::vector<EdgeDistributionTable> tables;

    if (repo.deserialize_graph_into(rec.graph_json, graph, nodes, edges, &error)) {
        const std::vector<std::vector<std::size_t>> faces = find_faces(graph);
        GraphMarker marker;
        tables = marker.edge_distribution_tables(graph, faces);
    }

    for (Edge* edge : edges) {
        delete edge;
    }
    for (Node* node : nodes) {
        delete node;
    }

    return tables;
}

/**
 * \brief Computes the comma-separated delta_t string for a graph record.
 * \param repo The graph repository for deserialization.
 * \param rec The graph record.
 * \return A comma-separated string of unique delta_t values.
 */
QString compute_delta_t_string(const Graph_Repository& repo, const Graph_Record& rec)
{
    const std::vector<EdgeDistributionTable> tables = compute_edge_distribution_tables(repo, rec);
    if (tables.empty()) return QString::number(rec.delta_t);

    QSet<int> values;
    for (const auto& table : tables) {
        values.insert(qAbs((table.wa + table.p0) - (table.w0 + table.pa)));
    }

    QStringList list;
    QList<int> sorted = values.values();
    std::sort(sorted.begin(), sorted.end());
    for (int v : sorted) list << QString::number(v);
    return list.join(QStringLiteral(", "));
}

QString escape_tsv_field(QString value)
{
    value.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    value.replace(QChar('\r'), QChar('\n'));
    value.replace(QChar('\t'), QChar(' '));
    return value;
}
}  // namespace

/**
 * \brief Retrieves graphs matching the current filter from the database and updates the table.
 */
void Graph_Browser_Dialog::refresh_results()
{
    QString error;
    m_rows = m_repo.find_graphs(current_filter(), &error);

    if (!error.isEmpty()) {
        m_status_label->setText(QStringLiteral("Erreur : %1").arg(error));
        m_table->setRowCount(0);
        clear_details_panel();
        m_open_button->setEnabled(false);
        m_delete_button->setEnabled(false);
        m_selected_id = -1;
        return;
    }

    populate_table(m_rows);
    m_status_label->setText(QStringLiteral("%1 résultat(s)").arg(m_rows.size()));
}

/**
 * \brief Populates the results table with a list of graph records.
 * \param rows The list of graph records to display.
 */
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

        auto* group_item = new QTableWidgetItem();
        group_item->setData(Qt::DisplayRole, rec.group_count);
        m_table->setItem(i, 2, group_item);

        auto* edge_item = new QTableWidgetItem();
        edge_item->setData(Qt::DisplayRole, rec.edge_count);
        m_table->setItem(i, 3, edge_item);

        m_table->setItem(i, 4, new QTableWidgetItem(QString::number(rec.node_count)));
        m_table->setItem(i, 5, new QTableWidgetItem(QString::number(rec.face_count)));
        m_table->setItem(i, 6, new QTableWidgetItem(QStringLiteral("%1, %2, %3, %4").arg(rec.wa).arg(rec.w0).arg(rec.pa).arg(rec.p0)));
        m_table->setItem(i, 7, new QTableWidgetItem(compute_delta_t_string(m_repo, rec)));
        m_table->setItem(i, 8, new QTableWidgetItem(span_formula_summary(rec.span_formula)));
        m_table->setItem(i, 9,
                         new QTableWidgetItem(
                             distribution_summary(rec.vertex_degree_distribution)));
        m_table->setItem(i, 10,
                         new QTableWidgetItem(
                             distribution_summary(rec.face_degree_distribution)));
    }

    if (!rows.isEmpty()) {
        m_table->selectRow(0);
    } else {
        m_selected_id = -1;
        m_open_button->setEnabled(false);
        m_delete_button->setEnabled(false);
        clear_details_panel();
    }
}

/**
 * \brief Retrieves the database ID of the currently selected graph in the table.
 * \return The graph ID, or -1 if no graph is selected.
 */
qint64 Graph_Browser_Dialog::selected_graph_id() const
{
    const QList<QTableWidgetItem*> items = m_table->selectedItems();
    if (items.isEmpty()) return -1;

    QTableWidgetItem* first = m_table->item(items.first()->row(), 0);
    if (!first) return -1;

    return first->data(Qt::UserRole).toLongLong();
}

/**
 * \brief Updates the details panel with metadata and visuals of the selected graph.
 */
void Graph_Browser_Dialog::update_details_panel()
{
    const qint64 id = selected_graph_id();
    if (id < 0) {
        clear_details_panel();
        return;
    }

    for (const Graph_Record& rec : m_rows) {
        if (rec.id != id) continue;

        set_metadata_value(m_details_id, QString::number(rec.id));
        set_metadata_value(m_details_created, rec.created_at);
        set_metadata_value(m_details_short_signature, short_signature(rec.topology_hash));
        set_metadata_value(m_details_json_length,
                           QStringLiteral("%1 octets").arg(rec.graph_json.size()));
        set_metadata_value(m_details_node_count, QString::number(rec.node_count));
        set_metadata_value(m_details_edge_count, QString::number(rec.edge_count));
        set_metadata_value(m_details_group_count, QString::number(rec.group_count));
        set_metadata_value(m_details_face_count, QString::number(rec.face_count));
        set_metadata_value(m_details_delta_t, compute_delta_t_string(m_repo, rec));

        clear_layout(m_invariants_tables_layout);
        const std::vector<EdgeDistributionTable> edge_tables =
            compute_edge_distribution_tables(m_repo, rec);

        if (edge_tables.empty()) {
            m_invariants_warning_label->hide();
            m_invariants_tables_layout->addWidget(new QLabel(QStringLiteral("Aucune table disponible"),
                                                             m_invariants_tables_widget));
        } else {
            const int displayed_count =
                std::min<int>(edge_tables.size(), kMaxDisplayedEdgeDistributionTables);
            if (edge_tables.size() > kMaxDisplayedEdgeDistributionTables) {
                m_invariants_warning_label->setText(
                    QStringLiteral("Seules les %1 premières solutions linéaires sont affichées "
                                   "(%2 solutions au total).")
                        .arg(kMaxDisplayedEdgeDistributionTables)
                        .arg(edge_tables.size()));
                m_invariants_warning_label->show();
            } else {
                m_invariants_warning_label->hide();
            }

            for (int i = 0; i < displayed_count; ++i) {
                const EdgeDistributionTable& table = edge_tables[i];
                auto* title =
                    new QLabel(QStringLiteral("Solution %1")
                                   .arg(QString::fromStdString(table.label)),
                               m_invariants_tables_widget);
                title->setObjectName(QStringLiteral("detailsInvariantSolutionTitle"));
                m_invariants_tables_layout->addWidget(title);
                m_invariants_tables_layout->addWidget(
                    create_edge_distribution_table(table, m_invariants_tables_widget));
            }
        }

        set_metadata_value(m_details_span, rec.span_formula);
        set_metadata_value(m_details_non_reducible,
                           rec.is_non_reducible ? QStringLiteral("oui") : QStringLiteral("non"));
        set_metadata_value(m_details_wa, QString::number(rec.wa));
        set_metadata_value(m_details_w0, QString::number(rec.w0));
        set_metadata_value(m_details_p0, QString::number(rec.p0));
        set_metadata_value(m_details_pa, QString::number(rec.pa));
        m_details_title->setText(rec.title.isEmpty() ? QStringLiteral("(sans titre)") : rec.title);

        const QList<int> keys = sorted_distribution_keys(rec);
        QStringList headers;
        headers.reserve(keys.size());
        m_distribution_table->setRowCount(2);
        m_distribution_table->setColumnCount(keys.size());

        for (int column = 0; column < keys.size(); ++column) {
            const int degree = keys[column];
            headers << QString::number(degree);

            auto* vertex_item = new QTableWidgetItem(
                QString::number(rec.vertex_degree_distribution.value(degree, 0)));
            auto* face_item = new QTableWidgetItem(
                QString::number(rec.face_degree_distribution.value(degree, 0)));

            vertex_item->setTextAlignment(Qt::AlignCenter);
            face_item->setTextAlignment(Qt::AlignCenter);

            m_distribution_table->setItem(0, column, vertex_item);
            m_distribution_table->setItem(1, column, face_item);
        }
        m_distribution_table->setHorizontalHeaderLabels(headers);
        m_distribution_table->resizeColumnsToContents();
        m_distribution_table->resizeRowsToContents();

        QByteArray graph_svg_bytes;
        QString svg_error;
        if (!build_svg_bytes(m_repo, rec, &graph_svg_bytes, &svg_error)) {
            m_status_label->setText(QStringLiteral("%1 résultat(s) - SVG indisponible (%2)")
                                        .arg(m_rows.size())
                                        .arg(svg_error));
        }

        m_selected_svg_bytes = graph_svg_bytes;
        set_preview_svg(graph_svg_bytes);
        m_copy_details_button->setEnabled(true);
        m_export_svg_button->setEnabled(!graph_svg_bytes.isEmpty());
        return;
    }

    clear_details_panel();
}

/**
 * \brief Slot called when the table selection changes. Enables/disables actions and refreshes details.
 */
void Graph_Browser_Dialog::on_selection_changed()
{
    m_selected_id = selected_graph_id();
    const bool has_selection = (m_selected_id >= 0);
    m_open_button->setEnabled(has_selection);
    m_delete_button->setEnabled(has_selection);
    update_details_panel();
}

/**
 * \brief Slot to copy the textual details of the selected graph to the clipboard.
 */
void Graph_Browser_Dialog::on_copy_details_clicked()
{
    const qint64 id = selected_graph_id();
    if (id < 0) return;

    for (const Graph_Record& rec : m_rows) {
        if (rec.id != id) continue;
        QGuiApplication::clipboard()->setText(build_details_text(rec));
        m_status_label->setText(QStringLiteral("Détails copiés dans le presse-papiers"));
        return;
    }
}

QString Graph_Browser_Dialog::build_results_tsv() const
{
    if (!m_table) return QString();

    QString output;
    QTextStream stream(&output);

    QStringList header_fields;
    header_fields.reserve(m_table->columnCount());
    for (int column = 0; column < m_table->columnCount(); ++column) {
        QTableWidgetItem* header_item = m_table->horizontalHeaderItem(column);
        header_fields << escape_tsv_field(header_item ? header_item->text() : QString());
    }
    stream << header_fields.join(QChar('\t')) << QChar('\n');

    for (int row = 0; row < m_table->rowCount(); ++row) {
        QStringList row_fields;
        row_fields.reserve(m_table->columnCount());
        for (int column = 0; column < m_table->columnCount(); ++column) {
            QTableWidgetItem* item = m_table->item(row, column);
            row_fields << escape_tsv_field(item ? item->text() : QString());
        }
        stream << row_fields.join(QChar('\t')) << QChar('\n');
    }

    return output;
}

void Graph_Browser_Dialog::on_export_tsv_clicked()
{
    if (!m_table || m_table->rowCount() == 0) {
        QMessageBox::information(this, QStringLiteral("Exporter TSV"),
                                 QStringLiteral("Aucun résultat à exporter."));
        return;
    }

    const QString file_path =
        QFileDialog::getSaveFileName(this, QStringLiteral("Exporter les résultats en TSV"),
                                     QStringLiteral("graphes.tsv"),
                                     QStringLiteral("TSV (*.tsv);;Texte (*.txt)"));
    if (file_path.isEmpty()) return;

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(
            this, QStringLiteral("Exporter TSV"),
            QStringLiteral("Impossible d'écrire le fichier :\n%1").arg(file.errorString()));
        return;
    }

    const QByteArray bytes = build_results_tsv().toUtf8();
    if (file.write(bytes) != bytes.size()) {
        QMessageBox::warning(this, QStringLiteral("Exporter TSV"),
                             QStringLiteral("Écriture incomplète du fichier TSV."));
        return;
    }

    m_status_label->setText(QStringLiteral("TSV exporté : %1").arg(file_path));
}

/**
 * \brief Slot to export the currently selected graph's SVG preview to a file.
 */
void Graph_Browser_Dialog::on_export_svg_clicked()
{
    if (m_selected_svg_bytes.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Exporter SVG"),
                                 QStringLiteral("Aucun aperçu SVG disponible."));
        return;
    }

    QString suggested_name = QStringLiteral("graphe_%1.svg").arg(m_selected_id);
    const QString file_path = QFileDialog::getSaveFileName(
        this, QStringLiteral("Exporter le SVG"), suggested_name, QStringLiteral("SVG (*.svg)"));
    if (file_path.isEmpty()) return;

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(
            this, QStringLiteral("Exporter SVG"),
            QStringLiteral("Impossible d'écrire le fichier :\n%1").arg(file.errorString()));
        return;
    }

    if (file.write(m_selected_svg_bytes) != m_selected_svg_bytes.size()) {
        QMessageBox::warning(this, QStringLiteral("Exporter SVG"),
                             QStringLiteral("Écriture incomplète du fichier SVG."));
        return;
    }

    m_status_label->setText(QStringLiteral("SVG exporté : %1").arg(file_path));
}

/**
 * \brief Slot called when the 'Open' button is clicked. Accepts the dialog.
 */
void Graph_Browser_Dialog::on_open_clicked()
{
    if (selected_graph_id() < 0) {
        QMessageBox::warning(this, QStringLiteral("Sélection"),
                             QStringLiteral("Aucun graphe sélectionné."));
        return;
    }

    accept();
}

/**
 * \brief Slot to delete the currently selected graph from the database after user confirmation.
 */
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

/**
 * \brief Deserializes and loads the selected graph's data.
 * \param graph The Graph object to populate.
 * \param out_nodes The list to populate with the graph's nodes.
 * \param out_edges The list to populate with the graph's edges.
 * \param error_message Optional pointer to store any error message.
 * \return True if loading succeeds, false otherwise.
 */
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

/**
 * \brief Clears all text, widgets, and previews from the details panel.
 */
void Graph_Browser_Dialog::clear_details_panel()
{
    m_selected_svg_bytes.clear();
    m_details_title->setText(QStringLiteral("Aucun graphe sélectionné"));
    set_metadata_value(m_details_id, QString());
    set_metadata_value(m_details_created, QString());
    set_metadata_value(m_details_short_signature, QString());
    set_metadata_value(m_details_json_length, QString());
    set_metadata_value(m_details_node_count, QString());
    set_metadata_value(m_details_edge_count, QString());
    set_metadata_value(m_details_group_count, QString());
    set_metadata_value(m_details_face_count, QString());
    set_metadata_value(m_details_delta_t, QString());
    set_metadata_value(m_details_span, QString());
    set_metadata_value(m_details_non_reducible, QString());
    set_metadata_value(m_details_wa, QString());
    set_metadata_value(m_details_w0, QString());
    set_metadata_value(m_details_p0, QString());
    set_metadata_value(m_details_pa, QString());
    clear_layout(m_invariants_tables_layout);
    m_invariants_warning_label->hide();
    m_distribution_table->setRowCount(2);
    m_distribution_table->setColumnCount(0);
    set_preview_svg(QByteArray());
    m_copy_details_button->setEnabled(false);
    m_export_svg_button->setEnabled(false);
}

/**
 * \brief Sets a text value on a QLabel, defaulting to "n/a" if empty.
 * \param label The target QLabel.
 * \param value The text value to set.
 */
void Graph_Browser_Dialog::set_metadata_value(QLabel* label, const QString& value)
{
    if (!label) return;
    label->setText(value.isEmpty() ? QStringLiteral("n/a") : value);
}

/**
 * \brief Renders and displays SVG bytes in the preview graphics scene.
 * \param svg_bytes The SVG data to display.
 */
void Graph_Browser_Dialog::set_preview_svg(const QByteArray& svg_bytes)
{
    m_preview_scene->clear();
    if (svg_bytes.isEmpty()) {
        m_preview_scene->addText(QStringLiteral("Aperçu indisponible"));
        m_preview_scene->setSceneRect(m_preview_scene->itemsBoundingRect());
        return;
    }

    const QPixmap preview = render_svg_preview(svg_bytes, QSize(1200, 900));
    if (preview.isNull()) {
        m_preview_scene->addText(QStringLiteral("SVG invalide"));
        m_preview_scene->setSceneRect(m_preview_scene->itemsBoundingRect());
        return;
    }

    m_preview_scene->addPixmap(preview);
    m_preview_scene->setSceneRect(preview.rect());
    m_preview_view->fitInView(m_preview_scene->sceneRect(), Qt::KeepAspectRatio);
}

/**
 * \brief Constructs a formatted multi-line string containing the metadata and distributions of a graph record.
 * \param rec The graph record to textify.
 * \return A string representation of the graph's details.
 */
QString Graph_Browser_Dialog::build_details_text(const Graph_Record& rec) const
{
    QString text;
    text += QStringLiteral("Titre: %1\n").arg(rec.title);
    text += QStringLiteral("ID: %1\n").arg(rec.id);
    text += QStringLiteral("Créé le: %1\n").arg(rec.created_at);
    text += QStringLiteral("Signature courte: %1\n").arg(short_signature(rec.topology_hash));
    text += QStringLiteral("Longueur JSON: %1 octets\n").arg(rec.graph_json.size());
    text += QStringLiteral("Ronds (R): %1\n").arg(rec.group_count);
    text += QStringLiteral("Arêtes (C): %1\n").arg(rec.edge_count);
    text += QStringLiteral("Sommets (S): %1\n").arg(rec.node_count);
    text += QStringLiteral("Faces (F): %1\n").arg(rec.face_count);
    text += QStringLiteral("∆T: %1\n").arg(compute_delta_t_string(m_repo, rec));
    text += QStringLiteral("Portance: %1\n").arg(rec.span_formula);
    text += QStringLiteral("Non réductible: %1\n")
                .arg(rec.is_non_reducible ? QStringLiteral("oui") : QStringLiteral("non"));

    text += QStringLiteral("\ntable d'état\n");
    text += QStringLiteral("       w    p\n");
    text += QStringLiteral("a   %1   %2\n")
                .arg(QString::number(rec.wa).leftJustified(4, QChar(' ')),
                     QString::number(rec.pa));
    text += QStringLiteral("0   %1   %2\n")
                .arg(QString::number(rec.w0).leftJustified(4, QChar(' ')),
                     QString::number(rec.p0));

    text += QStringLiteral("\nDistributions\n");

    const QList<int> keys = sorted_distribution_keys(rec);
    for (int degree : keys) {
        text += QStringLiteral("degré %1: sommets=%2, faces=%3\n")
                    .arg(degree)
                    .arg(rec.vertex_degree_distribution.value(degree, 0))
                    .arg(rec.face_degree_distribution.value(degree, 0));
    }

    return text;
}

/**
 * \brief Deserializes a graph record and generates its SVG byte representation.
 * \param repo The graph repository for deserialization.
 * \param rec The graph record containing the serialized graph JSON.
 * \param out_graph_svg_bytes Pointer to store the generated SVG bytes.
 * \param error_message Optional pointer to store any error message.
 * \return True if generation succeeds, false otherwise.
 */
bool Graph_Browser_Dialog::build_svg_bytes(const Graph_Repository& repo, const Graph_Record& rec,
                                           QByteArray* out_graph_svg_bytes, QString* error_message)
{
    if (!out_graph_svg_bytes) {
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

    const bool graph_ok = export_graph_svg_bytes(graph, true, out_graph_svg_bytes, &error);

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
