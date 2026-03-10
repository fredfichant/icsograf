#include "graph_browser_dialog.hpp"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QTextEdit>

#include "edge.hpp"
#include "graph.hpp"
#include "node.hpp"

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
    if (items.isEmpty())
        return -1;

    QTableWidgetItem* first = m_table->item(items.first()->row(), 0);
    if (!first)
        return -1;

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
        if (rec.id != id)
            continue;

        QString text;
        text += QString("ID : %1\n").arg(rec.id);
        text += QString("Titre : %1\n").arg(rec.title);
        text += QString("Créé le : %1\n").arg(rec.created_at);
        text += QString("Hash topologique : %1\n\n").arg(rec.topology_hash);

        text += QString("Sommets : %1\n").arg(rec.node_count);
        text += QString("Arêtes : %1\n").arg(rec.edge_count);
        text += QString("Groupes : %1\n").arg(rec.group_count);
        text += QString("Faces : %1\n\n").arg(rec.face_count);

        text += QString("wa : %1\n").arg(rec.wa);
        text += QString("w0 : %1\n").arg(rec.w0);
        text += QString("p0 : %1\n").arg(rec.p0);
        text += QString("pa : %1\n").arg(rec.pa);
        text += QString("Delta t : %1\n").arg(rec.delta_t);
        text += QString("Formule span : %1\n").arg(rec.span_formula);
        text += QString("Non réductible : %1\n\n").arg(rec.is_non_reducible ? "oui" : "non");

        text += "Distribution degrés sommets :\n";
        for (auto it = rec.vertex_degree_distribution.constBegin();
             it != rec.vertex_degree_distribution.constEnd(); ++it) {
            text += QString("  %1 -> %2\n").arg(it.key()).arg(it.value());
        }

        text += "\nDistribution degrés faces :\n";
        for (auto it = rec.face_degree_distribution.constBegin();
             it != rec.face_degree_distribution.constEnd(); ++it) {
            text += QString("  %1 -> %2\n").arg(it.key()).arg(it.value());
        }

        m_details_text->setPlainText(text);
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
    if (id < 0)
        return;

    const auto answer = QMessageBox::question(
        this, QStringLiteral("Suppression"),
        QStringLiteral("Supprimer définitivement le graphe sélectionné ?"));

    if (answer != QMessageBox::Yes)
        return;

    QString error;
    if (!m_repo.delete_graph(id, &error)) {
        QMessageBox::warning(this, QStringLiteral("Suppression"),
                             QStringLiteral("Échec de la suppression :\n%1").arg(error));
        return;
    }

    refresh_results();
}

bool Graph_Browser_Dialog::load_selected_graph(Graph& graph,
                                               QList<Node*>& out_nodes,
                                               QList<Edge*>& out_edges,
                                               QString* error_message)
{
    const qint64 id = selected_graph_id();
    if (id < 0) {
        if (error_message)
            *error_message = QStringLiteral("aucun graphe sélectionné");
        return false;
    }

    Graph_Record rec;
    QString error;
    if (!m_repo.load_graph_record(id, &rec, &error)) {
        if (error_message)
            *error_message = error;
        return false;
    }

    if (!m_repo.deserialize_graph_into(rec.graph_json, graph, out_nodes, out_edges, &error)) {
        if (error_message)
            *error_message = error;
        return false;
    }

    return true;
}
