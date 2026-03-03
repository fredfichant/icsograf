#include "properties_view.hpp"

#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <set>

#include "graph_properties.hpp"

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
    m_form_layout->addRow(tr("Edges:"), m_edge_count_label);
    m_form_layout->addRow(tr("Groups:"), m_group_count_label);
    m_form_layout->addRow(tr("Faces:"), m_face_count_label);
    m_main_layout->addLayout(m_form_layout);

    // Degree distribution table
    m_degree_table = new QTableWidget(this);
    m_degree_table->setRowCount(2);
    m_degree_table->setVerticalHeaderLabels({tr("Vertices"), tr("Faces")});
    m_degree_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_main_layout->addWidget(m_degree_table);

    // Edge distribution table (Step 6)
    m_edge_distribution_table = new QTableWidget(this);
    m_edge_distribution_table->setRowCount(2);
    m_edge_distribution_table->setColumnCount(2);
    m_edge_distribution_table->setVerticalHeaderLabels({"a", "0"});
    m_edge_distribution_table->setHorizontalHeaderLabels({"w", "p"});
    m_edge_distribution_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_main_layout->addWidget(new QLabel(tr("Edge Distribution:"), this));
    m_main_layout->addWidget(m_edge_distribution_table);

    m_delta_t_label = new QLabel(this);
    m_form_layout->addRow(tr("∆T:"), m_delta_t_label);

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

    // Update edge distribution table (Step 6)
    m_edge_distribution_table->setItem(0, 0,
                                       new QTableWidgetItem(QString::number(m_properties.wa())));
    m_edge_distribution_table->setItem(1, 0,
                                       new QTableWidgetItem(QString::number(m_properties.w0())));
    m_edge_distribution_table->setItem(1, 1,
                                       new QTableWidgetItem(QString::number(m_properties.p0())));
    m_edge_distribution_table->setItem(0, 1,
                                       new QTableWidgetItem(QString::number(m_properties.pa())));

    for (int r = 0; r < 2; ++r)
        for (int c = 0; c < 2; ++c)
            m_edge_distribution_table->item(r, c)->setTextAlignment(Qt::AlignCenter);

    m_delta_t_label->setText(QString::number(m_properties.delta_t()));
}
