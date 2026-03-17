/**
 * \file src/graph/properties_view.cpp
 * \brief Implementation of properties view graph logic.
 */

#include "properties_view.hpp"

#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLayoutItem>
#include <QAbstractItemView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <set>

#include "graph_properties.hpp"

namespace {

constexpr int kMaxDisplayedEdgeDistributionTables = 8;

QTableWidget* create_edge_distribution_table(const EdgeDistributionTable& table, QWidget* parent)
{
    auto* widget = new QTableWidget(parent);
    widget->setRowCount(2);
    widget->setColumnCount(2);
    widget->setVerticalHeaderLabels({QStringLiteral("a"), QStringLiteral("0")});
    widget->setHorizontalHeaderLabels({QStringLiteral("w"), QStringLiteral("p")});
    widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    widget->setSelectionMode(QAbstractItemView::NoSelection);
    widget->setFocusPolicy(Qt::NoFocus);
    widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    widget->setMinimumHeight(130);

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

}  // namespace

Properties_View::Properties_View(const Graph_Properties& properties, QWidget* parent)
    : QWidget(parent), m_properties(properties)
{
    m_main_layout = new QVBoxLayout(this);
    setLayout(m_main_layout);

    // Simple properties
    m_form_layout = new QFormLayout();
    m_node_count_label = new QLabel(this);
    m_edge_count_label = new QLabel(this);
    m_group_count_label = new QLabel(this);
    m_face_count_label = new QLabel(this);

    m_form_layout->addRow(tr("Nodes:"), m_node_count_label);
    m_form_layout->addRow(tr("Edges (C):"), m_edge_count_label);
    m_form_layout->addRow(tr("Groups (R):"), m_group_count_label);
    m_form_layout->addRow(tr("Faces:"), m_face_count_label);
    m_main_layout->addLayout(m_form_layout);

    // Degree distribution table
    m_degree_table = new QTableWidget(this);
    m_degree_table->setRowCount(2);
    m_degree_table->setVerticalHeaderLabels({tr("Sommets"), tr("Faces")});
    m_degree_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_main_layout->addWidget(m_degree_table);

    m_main_layout->addWidget(new QLabel(tr("table(s) d'état(s):"), this));
    m_edge_distribution_warning_label = new QLabel(this);
    m_edge_distribution_warning_label->setWordWrap(true);
    m_edge_distribution_warning_label->hide();
    m_main_layout->addWidget(m_edge_distribution_warning_label);

    m_edge_distribution_tables_widget = new QWidget(this);
    m_edge_distribution_tables_layout = new QVBoxLayout(m_edge_distribution_tables_widget);
    m_edge_distribution_tables_layout->setContentsMargins(0, 0, 0, 0);
    m_edge_distribution_tables_layout->setSpacing(8);
    m_main_layout->addWidget(m_edge_distribution_tables_widget);

    m_delta_t_label = new QLabel(this);
    m_form_layout->addRow(tr("∆T:"), m_delta_t_label);
    m_span_formula_label = new QLabel(this);
    m_form_layout->addRow(tr("Portance P:"), m_span_formula_label);
    m_non_reducible_label = new QLabel(this);
    m_form_layout->addRow(tr("formule des portées :"), m_non_reducible_label);

    connect(&m_properties, &Graph_Properties::properties_changed, this,
            &Properties_View::update_view);

    update_view();
}

void Properties_View::update_view()
{
    // Update simple properties
    m_node_count_label->setText(QString::number(m_properties.node_count()));
    m_edge_count_label->setText(QString::number(m_properties.edge_count()));
    m_group_count_label->setText(QString::number(m_properties.group_count()));
    m_face_count_label->setText(QString::number(m_properties.face_count()));

    // Update degree distribution table
    const QMap<int, int>& vertex_dist = m_properties.vertex_degree_distribution();
    const QMap<int, int>& face_dist = m_properties.face_degree_distribution();

    m_degree_table->clearContents();

    if (vertex_dist.isEmpty() && face_dist.isEmpty()) {
        m_degree_table->setColumnCount(0);
        return;
    }

    std::set<int> degrees;
    for (int degree : vertex_dist.keys()) {
        degrees.insert(degree);
    }
    for (int degree : face_dist.keys()) {
        degrees.insert(degree);
    }

    if (degrees.empty()) {
        m_degree_table->setColumnCount(0);
        return;
    }

    QList<int> sorted_degrees =
        QList<int>::fromStdList(std::list<int>(degrees.begin(), degrees.end()));
    qSort(sorted_degrees);

    m_degree_table->setColumnCount(sorted_degrees.size());

    QStringList headers;
    for (int i = 0; i < sorted_degrees.size(); ++i) {
        int degree = sorted_degrees[i];
        headers << QString::number(degree);

        int vertex_count = vertex_dist.value(degree, 0);
        QTableWidgetItem* vertex_item = new QTableWidgetItem(QString::number(vertex_count));
        vertex_item->setTextAlignment(Qt::AlignCenter);
        m_degree_table->setItem(0, i, vertex_item);

        int face_count = face_dist.value(degree, 0);
        QTableWidgetItem* face_item = new QTableWidgetItem(QString::number(face_count));
        face_item->setTextAlignment(Qt::AlignCenter);
        m_degree_table->setItem(1, i, face_item);
    }

    m_degree_table->setHorizontalHeaderLabels(headers);

    clear_layout(m_edge_distribution_tables_layout);

    const std::vector<EdgeDistributionTable>& edge_tables = m_properties.edge_distribution_tables();
    if (edge_tables.empty()) {
        m_edge_distribution_warning_label->hide();
        auto* empty_label = new QLabel(tr("No edge distribution available."), this);
        m_edge_distribution_tables_layout->addWidget(empty_label);
    } else {
        const int displayed_count =
            std::min<int>(edge_tables.size(), kMaxDisplayedEdgeDistributionTables);
        if (edge_tables.size() > kMaxDisplayedEdgeDistributionTables) {
            m_edge_distribution_warning_label->setText(
                tr("Only the first %1 linear solutions are shown (%2 solutions).")
                    .arg(kMaxDisplayedEdgeDistributionTables)
                    .arg(edge_tables.size()));
            m_edge_distribution_warning_label->show();
        } else {
            m_edge_distribution_warning_label->hide();
        }

        for (int i = 0; i < displayed_count; ++i) {
            const EdgeDistributionTable& table = edge_tables[i];
            auto* title = new QLabel(
                tr("Solution %1").arg(QString::fromStdString(table.label)), this);
            title->setObjectName(QStringLiteral("edgeDistributionSolutionTitle"));
            m_edge_distribution_tables_layout->addWidget(title);
            m_edge_distribution_tables_layout->addWidget(create_edge_distribution_table(table, this));
        }
    }

    m_delta_t_label->setText(QString::number(m_properties.delta_t()));
    m_span_formula_label->setText(m_properties.span_formula());
    m_non_reducible_label->setText(m_properties.is_non_reducible() ? tr("OK")
                                                                    : tr("arc trop long"));
}
