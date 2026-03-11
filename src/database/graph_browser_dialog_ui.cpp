/**
 * \file src/database/graph_browser_dialog_ui.cpp
 * \brief UI construction and filter wiring for the graph browser dialog.
 */

#include "graph_browser_dialog.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

void Graph_Browser_Dialog::build_ui()
{
    auto* root = new QVBoxLayout(this);

    auto* filters_layout = new QHBoxLayout();
    auto* form = new QFormLayout();

    m_title_edit = new QLineEdit(this);
    m_title_edit->setPlaceholderText(QStringLiteral("Recherche dans le titre"));

    m_node_count_spin = new QSpinBox(this);
    m_node_count_spin->setRange(-1, 100000);
    m_node_count_spin->setValue(-1);
    m_node_count_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_edge_count_spin = new QSpinBox(this);
    m_edge_count_spin->setRange(-1, 100000);
    m_edge_count_spin->setValue(-1);
    m_edge_count_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_group_count_spin = new QSpinBox(this);
    m_group_count_spin->setRange(-1, 100000);
    m_group_count_spin->setValue(-1);
    m_group_count_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_face_count_spin = new QSpinBox(this);
    m_face_count_spin->setRange(-1, 100000);
    m_face_count_spin->setValue(-1);
    m_face_count_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_delta_t_spin = new QSpinBox(this);
    m_delta_t_spin->setRange(-1, 100000);
    m_delta_t_spin->setValue(-1);
    m_delta_t_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_non_reducible_combo = new QComboBox(this);
    m_non_reducible_combo->addItem(QStringLiteral("Tous"), -1);
    m_non_reducible_combo->addItem(QStringLiteral("Oui"), 1);
    m_non_reducible_combo->addItem(QStringLiteral("Non"), 0);

    form->addRow(QStringLiteral("Titre"), m_title_edit);
    form->addRow(QStringLiteral("Nombre de sommets"), m_node_count_spin);
    form->addRow(QStringLiteral("Nombre d'arêtes"), m_edge_count_spin);
    form->addRow(QStringLiteral("Nombre de groupes"), m_group_count_spin);
    form->addRow(QStringLiteral("Nombre de faces"), m_face_count_spin);
    form->addRow(QStringLiteral("Delta t"), m_delta_t_spin);
    form->addRow(QStringLiteral("Non réductible"), m_non_reducible_combo);

    filters_layout->addLayout(form);

    auto* buttons_layout = new QVBoxLayout();
    m_search_button = new QPushButton(QStringLiteral("Rechercher"), this);
    m_reset_button = new QPushButton(QStringLiteral("Réinitialiser"), this);
    buttons_layout->addWidget(m_search_button);
    buttons_layout->addWidget(m_reset_button);
    buttons_layout->addStretch(1);

    filters_layout->addLayout(buttons_layout);
    root->addLayout(filters_layout);

    auto* splitter = new QSplitter(this);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(11);
    m_table->setHorizontalHeaderLabels(QStringList() << QStringLiteral("ID")
                                                      << QStringLiteral("Titre")
                                                      << QStringLiteral("Créé le")
                                                      << QStringLiteral("Arêtes")
                                                      << QStringLiteral("Groupes")
                                                      << QStringLiteral("Faces")
                                                      << QStringLiteral("wa")
                                                      << QStringLiteral("w0")
                                                      << QStringLiteral("p0")
                                                      << QStringLiteral("pa")
                                                      << QStringLiteral("Delta t"));
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);

    m_details_text = new QTextEdit(this);
    m_details_text->setReadOnly(true);

    splitter->addWidget(m_table);
    splitter->addWidget(m_details_text);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    root->addWidget(splitter, 1);

    auto* bottom_layout = new QHBoxLayout();
    m_status_label = new QLabel(QStringLiteral("Prêt"), this);

    m_open_button = new QPushButton(QStringLiteral("Ouvrir"), this);
    m_open_button->setEnabled(false);

    m_delete_button = new QPushButton(QStringLiteral("Supprimer"), this);
    m_delete_button->setEnabled(false);

    m_cancel_button = new QPushButton(QStringLiteral("Fermer"), this);

    bottom_layout->addWidget(m_status_label, 1);
    bottom_layout->addWidget(m_delete_button);
    bottom_layout->addWidget(m_open_button);
    bottom_layout->addWidget(m_cancel_button);

    root->addLayout(bottom_layout);
}

void Graph_Browser_Dialog::connect_signals()
{
    connect(m_search_button, &QPushButton::clicked, this, &Graph_Browser_Dialog::on_search_clicked);
    connect(m_reset_button, &QPushButton::clicked, this, &Graph_Browser_Dialog::on_reset_clicked);
    connect(m_open_button, &QPushButton::clicked, this, &Graph_Browser_Dialog::on_open_clicked);
    connect(m_delete_button, &QPushButton::clicked, this, &Graph_Browser_Dialog::on_delete_clicked);
    connect(m_cancel_button, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_table, &QTableWidget::itemSelectionChanged, this,
            &Graph_Browser_Dialog::on_selection_changed);
    connect(m_table, &QTableWidget::cellDoubleClicked, this,
            &Graph_Browser_Dialog::on_table_double_clicked);

    connect(m_title_edit, &QLineEdit::returnPressed, this, &Graph_Browser_Dialog::on_search_clicked);
}

Graph_Query_Filter Graph_Browser_Dialog::current_filter() const
{
    Graph_Query_Filter filter;
    filter.title_contains = m_title_edit->text().trimmed();
    filter.node_count = m_node_count_spin->value();
    filter.edge_count = m_edge_count_spin->value();
    filter.group_count = m_group_count_spin->value();
    filter.face_count = m_face_count_spin->value();
    filter.delta_t = m_delta_t_spin->value();
    filter.is_non_reducible = m_non_reducible_combo->currentData().toInt();
    filter.limit = 500;
    return filter;
}

void Graph_Browser_Dialog::on_search_clicked()
{
    refresh_results();
}

void Graph_Browser_Dialog::on_reset_clicked()
{
    m_title_edit->clear();
    m_node_count_spin->setValue(-1);
    m_edge_count_spin->setValue(-1);
    m_group_count_spin->setValue(-1);
    m_face_count_spin->setValue(-1);
    m_delta_t_spin->setValue(-1);
    m_non_reducible_combo->setCurrentIndex(0);

    refresh_results();
}

void Graph_Browser_Dialog::on_table_double_clicked(int, int)
{
    if (selected_graph_id() >= 0)
        accept();
}
