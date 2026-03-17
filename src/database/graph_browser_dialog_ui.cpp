/**
 * \file src/database/graph_browser_dialog_ui.cpp
 * \brief UI construction and filter wiring for the graph browser dialog.
 */

#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "graph_browser_dialog.hpp"

void Graph_Browser_Dialog::build_ui()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(14);

    auto* filters_card = new QFrame(this);
    filters_card->setObjectName(QStringLiteral("filtersCard"));
    auto* filters_layout = new QHBoxLayout(filters_card);
    filters_layout->setContentsMargins(14, 14, 14, 14);
    filters_layout->setSpacing(12);

    m_title_edit = new QLineEdit(this);
    m_title_edit->setPlaceholderText(QStringLiteral("Recherche dans le titre"));

    m_node_count_spin = new QSpinBox(this);
    m_node_count_spin->setRange(-1, 1000);
    m_node_count_spin->setValue(-1);
    m_node_count_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_edge_count_spin = new QSpinBox(this);
    m_edge_count_spin->setRange(-1, 1000);
    m_edge_count_spin->setValue(-1);
    m_edge_count_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_group_count_spin = new QSpinBox(this);
    m_group_count_spin->setRange(-1, 1000);
    m_group_count_spin->setValue(-1);
    m_group_count_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_face_count_spin = new QSpinBox(this);
    m_face_count_spin->setRange(-1, 1000);
    m_face_count_spin->setValue(-1);
    m_face_count_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_delta_t_spin = new QSpinBox(this);
    m_delta_t_spin->setRange(-1, 1000);
    m_delta_t_spin->setValue(-1);
    m_delta_t_spin->setSpecialValueText(QStringLiteral("Tous"));

    m_non_reducible_combo = new QComboBox(this);
    m_non_reducible_combo->addItem(QStringLiteral("Tous"), -1);
    m_non_reducible_combo->addItem(QStringLiteral("Oui"), 1);
    m_non_reducible_combo->addItem(QStringLiteral("Non"), 0);

    auto create_filter_block = [filters_card](const QString& label_text, QWidget* field) {
        auto* layout = new QVBoxLayout();
        layout->setSpacing(6);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* label = new QLabel(label_text, filters_card);
        label->setObjectName(QStringLiteral("filterLabel"));
        layout->addWidget(label);
        layout->addWidget(field);

        return layout;
    };

    auto* fields_layout = new QHBoxLayout();
    fields_layout->setSpacing(12);
    fields_layout->addLayout(create_filter_block(QStringLiteral("Titre"), m_title_edit), 2);
    fields_layout->addLayout(create_filter_block(QStringLiteral("Sommets (S)"), m_node_count_spin));
    fields_layout->addLayout(create_filter_block(QStringLiteral("Arêtes (C)"), m_edge_count_spin));
    fields_layout->addLayout(create_filter_block(QStringLiteral("Ronds (R)"), m_group_count_spin));
    fields_layout->addLayout(create_filter_block(QStringLiteral("Faces (F)"), m_face_count_spin));
    fields_layout->addLayout(create_filter_block(QStringLiteral("∆T"), m_delta_t_spin));
    fields_layout->addLayout(
        create_filter_block(QStringLiteral("Non réductible"), m_non_reducible_combo));

    filters_layout->addLayout(fields_layout, 1);

    auto* buttons_layout = new QVBoxLayout();
    m_search_button = new QPushButton(QStringLiteral("Rechercher"), this);
    m_reset_button = new QPushButton(QStringLiteral("Réinitialiser"), this);
    buttons_layout->addWidget(m_search_button);
    buttons_layout->addWidget(m_reset_button);
    buttons_layout->addStretch(1);

    filters_layout->addLayout(buttons_layout);
    root->addWidget(filters_card);

    auto* splitter = new QSplitter(this);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(10);
    m_table->setHorizontalHeaderLabels(
        QStringList() << QStringLiteral("ID") << QStringLiteral("nom") << QStringLiteral("C")
                      << QStringLiteral("R") << QStringLiteral("F") << QStringLiteral("wp")
                      << QStringLiteral("wp'") << QStringLiteral("w'p'") << QStringLiteral("w'p")
                      << QStringLiteral("∆T"));
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);

    m_details_scroll = new QScrollArea(this);
    m_details_scroll->setWidgetResizable(true);
    m_details_scroll->setFrameShape(QFrame::NoFrame);

    auto* details_container = new QWidget(m_details_scroll);
    auto* details_layout = new QVBoxLayout(details_container);
    details_layout->setContentsMargins(16, 16, 16, 16);
    details_layout->setSpacing(12);

    m_details_title = new QLabel(QStringLiteral("Aucun graphe sélectionné"), details_container);
    m_details_title->setObjectName(QStringLiteral("detailsTitle"));
    m_details_title->setWordWrap(true);
    details_layout->addWidget(m_details_title);

    auto* metadata_card = new QFrame(details_container);
    metadata_card->setObjectName(QStringLiteral("detailsCard"));
    auto* metadata_layout = new QFormLayout(metadata_card);
    metadata_layout->setContentsMargins(12, 12, 12, 12);
    metadata_layout->setSpacing(8);

    auto create_value_label = [details_container](const QString& object_name) {
        auto* label = new QLabel(details_container);
        label->setObjectName(object_name);
        label->setWordWrap(true);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        return label;
    };

    m_details_id = create_value_label(QStringLiteral("detailsIdValue"));
    m_details_created = create_value_label(QStringLiteral("detailsCreatedValue"));
    m_details_short_signature = create_value_label(QStringLiteral("detailsShortSignatureValue"));
    m_details_json_length = create_value_label(QStringLiteral("detailsJsonLengthValue"));
    m_details_node_count = create_value_label(QStringLiteral("detailsNodeCountValue"));
    m_details_edge_count = create_value_label(QStringLiteral("detailsEdgeCountValue"));
    m_details_group_count = create_value_label(QStringLiteral("detailsGroupCountValue"));
    m_details_face_count = create_value_label(QStringLiteral("detailsFaceCountValue"));
    m_details_delta_t = create_value_label(QStringLiteral("detailsDeltaTValue"));
    m_details_span = create_value_label(QStringLiteral("detailsSpanValue"));
    m_details_non_reducible = create_value_label(QStringLiteral("detailsNonReducibleValue"));
    m_details_wa = create_value_label(QStringLiteral("detailsWaValue"));
    m_details_w0 = create_value_label(QStringLiteral("detailsW0Value"));
    m_details_p0 = create_value_label(QStringLiteral("detailsP0Value"));
    m_details_pa = create_value_label(QStringLiteral("detailsPaValue"));

    metadata_layout->addRow(QStringLiteral("ID :"), m_details_id);
    metadata_layout->addRow(QStringLiteral("Créé le :"), m_details_created);
    metadata_layout->addRow(QStringLiteral("Signature courte :"), m_details_short_signature);
    metadata_layout->addRow(QStringLiteral("Longueur JSON :"), m_details_json_length);
    metadata_layout->addRow(QStringLiteral("Sommets (S) :"), m_details_node_count);
    metadata_layout->addRow(QStringLiteral("Arêtes (C) :"), m_details_edge_count);
    metadata_layout->addRow(QStringLiteral("Ronds (R) :"), m_details_group_count);
    metadata_layout->addRow(QStringLiteral("Faces (F) :"), m_details_face_count);
    metadata_layout->addRow(QStringLiteral("∆T :"), m_details_delta_t);
    metadata_layout->addRow(QStringLiteral("Portance :"), m_details_span);
    metadata_layout->addRow(QStringLiteral("Non réductible :"), m_details_non_reducible);
    details_layout->addWidget(metadata_card);

    auto* invariants_label = new QLabel(QStringLiteral("Invariants"), details_container);
    invariants_label->setObjectName(QStringLiteral("detailsSectionTitle"));
    details_layout->addWidget(invariants_label);

    m_invariants_warning_label = new QLabel(details_container);
    m_invariants_warning_label->setObjectName(QStringLiteral("detailsInvariantsWarning"));
    m_invariants_warning_label->setWordWrap(true);
    m_invariants_warning_label->hide();
    details_layout->addWidget(m_invariants_warning_label);

    m_invariants_tables_widget = new QWidget(details_container);
    m_invariants_tables_widget->setObjectName(QStringLiteral("detailsInvariantsTablesWidget"));
    m_invariants_tables_layout = new QVBoxLayout(m_invariants_tables_widget);
    m_invariants_tables_layout->setContentsMargins(0, 0, 0, 0);
    m_invariants_tables_layout->setSpacing(8);
    details_layout->addWidget(m_invariants_tables_widget);

    auto* dist_label = new QLabel(QStringLiteral("Distributions"), details_container);
    dist_label->setObjectName(QStringLiteral("detailsSectionTitle"));
    details_layout->addWidget(dist_label);

    m_distribution_table = new QTableWidget(details_container);
    m_distribution_table->setObjectName(QStringLiteral("detailsDistributionTable"));
    m_distribution_table->setRowCount(2);
    m_distribution_table->setVerticalHeaderLabels(
        QStringList() << QStringLiteral("Sommets") << QStringLiteral("Faces"));
    m_distribution_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_distribution_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_distribution_table->setAlternatingRowColors(true);
    m_distribution_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_distribution_table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_distribution_table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    m_distribution_table->horizontalHeader()->setDefaultSectionSize(56);
    m_distribution_table->horizontalHeader()->setMinimumSectionSize(56);
    m_distribution_table->verticalHeader()->setDefaultSectionSize(56);
    m_distribution_table->verticalHeader()->setMinimumSectionSize(56);
    m_distribution_table->setCornerButtonEnabled(false);
    m_distribution_table->setShowGrid(true);
    m_distribution_table->setMinimumHeight(210);
    details_layout->addWidget(m_distribution_table);

    auto* preview_label = new QLabel(QStringLiteral("Aperçu"), details_container);
    preview_label->setObjectName(QStringLiteral("detailsSectionTitle"));
    details_layout->addWidget(preview_label);

    m_preview_scene = new QGraphicsScene(this);
    m_preview_view = new QGraphicsView(m_preview_scene, details_container);
    m_preview_view->setObjectName(QStringLiteral("detailsPreviewView"));
    m_preview_view->setMinimumHeight(320);
    m_preview_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_preview_view->setDragMode(QGraphicsView::ScrollHandDrag);
    m_preview_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    details_layout->addWidget(m_preview_view);

    auto* detail_buttons_layout = new QHBoxLayout();
    m_copy_details_button = new QPushButton(QStringLiteral("Copier les détails"), details_container);
    m_export_svg_button = new QPushButton(QStringLiteral("Exporter SVG"), details_container);
    m_enlarge_preview_button =
        new QPushButton(QStringLiteral("Agrandir l’aperçu"), details_container);
    m_copy_details_button->setObjectName(QStringLiteral("copyDetailsButton"));
    m_export_svg_button->setObjectName(QStringLiteral("exportSvgButton"));
    m_enlarge_preview_button->setObjectName(QStringLiteral("enlargePreviewButton"));
    detail_buttons_layout->addWidget(m_copy_details_button);
    detail_buttons_layout->addWidget(m_export_svg_button);
    detail_buttons_layout->addWidget(m_enlarge_preview_button);
    detail_buttons_layout->addStretch(1);
    details_layout->addLayout(detail_buttons_layout);
    details_layout->addStretch(1);

    m_details_scroll->setWidget(details_container);

    splitter->addWidget(m_table);
    splitter->addWidget(m_details_scroll);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 3);

    root->addWidget(splitter, 1);

    auto* bottom_layout = new QHBoxLayout();
    m_status_label = new QLabel(QStringLiteral("prêt"), this);

    m_open_button = new QPushButton(QStringLiteral("ouvrir"), this);
    m_open_button->setEnabled(false);

    m_delete_button = new QPushButton(QStringLiteral("supprimer"), this);
    m_delete_button->setEnabled(false);

    m_cancel_button = new QPushButton(QStringLiteral("fermer"), this);

    bottom_layout->addWidget(m_status_label, 1);
    bottom_layout->addWidget(m_delete_button);
    bottom_layout->addWidget(m_open_button);
    bottom_layout->addWidget(m_cancel_button);

    root->addLayout(bottom_layout);

    this->setStyleSheet(R"(
        QDialog {
            background-color: #d8e4f2;
            color: #0f172a;
        }
        QLabel {
            font-size: 13px;
            color: #0f172a;
        }
        QLineEdit, QSpinBox, QComboBox, QTableWidget, QGraphicsView, QScrollArea {
            background: #f8fbff;
            color: #10233b;
            border: 1px solid #8aa9c7;
            border-radius: 6px;
            padding: 4px;
        }
        QHeaderView::section {
            background: #c3d6ea;
            color: #10233b;
            border: none;
            border-bottom: 1px solid #8aa9c7;
            padding: 6px;
            font-weight: 700;
        }
        QFrame#detailsCard {
            background: #f8fbff;
            border: 1px solid #8aa9c7;
            border-radius: 8px;
        }
        QFrame#filtersCard {
            background: #eef5fb;
            border: 1px solid #a8bfd8;
            border-radius: 10px;
        }
        QLabel#filterLabel {
            font-size: 12px;
            font-weight: 700;
            color: #33597d;
        }
        QLabel#detailsTitle {
            font-size: 18px;
            font-weight: 700;
            color: #0b2742;
        }
        QLabel#detailsSectionTitle {
            font-size: 14px;
            font-weight: 700;
            color: #1d4f7a;
        }
        QPushButton {
            background-color: #1c5d99;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 6px 12px;
            min-height: 28px;
        }
        QPushButton:hover {
            background-color: #164a7b;
        }
        QPushButton:disabled {
            background-color: #9ca3af;
            color: #e5e7eb;
        }
        QTableWidget#detailsInvariantsTable::item {
            font-size: 14px;
            font-weight: 600;
            text-align: center;
        }
        QTableWidget#detailsInvariantsTable {
            gridline-color: #8aa9c7;
        }
        QTableWidget#detailsDistributionTable {
            gridline-color: #8aa9c7;
        }
        QTableWidget#detailsInvariantsTable::item, QTableWidget#detailsDistributionTable::item {
            padding: 10px 6px;
        }
    )");
}

void Graph_Browser_Dialog::connect_signals()
{
    connect(m_search_button, &QPushButton::clicked, this, &Graph_Browser_Dialog::on_search_clicked);
    connect(m_reset_button, &QPushButton::clicked, this, &Graph_Browser_Dialog::on_reset_clicked);
    connect(m_open_button, &QPushButton::clicked, this, &Graph_Browser_Dialog::on_open_clicked);
    connect(m_delete_button, &QPushButton::clicked, this, &Graph_Browser_Dialog::on_delete_clicked);
    connect(m_cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_copy_details_button, &QPushButton::clicked, this,
            &Graph_Browser_Dialog::on_copy_details_clicked);
    connect(m_export_svg_button, &QPushButton::clicked, this,
            &Graph_Browser_Dialog::on_export_svg_clicked);
    connect(m_enlarge_preview_button, &QPushButton::clicked, this,
            &Graph_Browser_Dialog::on_enlarge_preview_clicked);

    connect(m_table, &QTableWidget::itemSelectionChanged, this,
            &Graph_Browser_Dialog::on_selection_changed);
    connect(m_table, &QTableWidget::cellDoubleClicked, this,
            &Graph_Browser_Dialog::on_table_double_clicked);
    // recherche automatique quand une valeur change
    connect(m_title_edit, &QLineEdit::returnPressed, this,
            &Graph_Browser_Dialog::on_search_clicked);
    connect(m_group_count_spin, qOverload<int>(&QSpinBox::valueChanged), this,
            &Graph_Browser_Dialog::on_search_clicked);
    connect(m_node_count_spin, qOverload<int>(&QSpinBox::valueChanged), this,
            &Graph_Browser_Dialog::on_search_clicked);
    connect(m_face_count_spin, qOverload<int>(&QSpinBox::valueChanged), this,
            &Graph_Browser_Dialog::on_search_clicked);
    connect(m_delta_t_spin, qOverload<int>(&QSpinBox::valueChanged), this,
            &Graph_Browser_Dialog::on_search_clicked);
    connect(m_edge_count_spin, qOverload<int>(&QSpinBox::valueChanged), this,
            &Graph_Browser_Dialog::on_search_clicked);

    connect(m_non_reducible_combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &Graph_Browser_Dialog::on_search_clicked);
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

void Graph_Browser_Dialog::on_search_clicked() { refresh_results(); }

void Graph_Browser_Dialog::on_reset_clicked()
{
    m_title_edit->clear();
    m_node_count_spin->setValue(-1);
    m_edge_count_spin->setValue(-1);
    m_group_count_spin->setValue(-1);
    m_face_count_spin->setValue(-1);
    m_delta_t_spin->setValue(-1);
    m_non_reducible_combo->setCurrentIndex(0);
    m_status_label->setText(QStringLiteral("Filtres réinitialisés"));
    refresh_results();
}

void Graph_Browser_Dialog::on_table_double_clicked(int, int)
{
    if (selected_graph_id() >= 0) accept();
}
