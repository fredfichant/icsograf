/**
 * \file src/database/graph_browser_dialog.hpp
 * \brief Dialog declarations for browsing, filtering, previewing, and loading stored graphs.
 */

#ifndef GRAPH_BROWSER_DIALOG_HPP
#define GRAPH_BROWSER_DIALOG_HPP

#include <QDialog>
#include <QList>

class QComboBox;
class QFormLayout;
class QGraphicsScene;
class QGraphicsView;
class QLabel;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QSpinBox;
class QTableWidget;
class QVBoxLayout;

class Graph;
class Node;
class Edge;

#include "graph_repository.hpp"

class Graph_Browser_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Graph_Browser_Dialog(QWidget* parent = nullptr);
    ~Graph_Browser_Dialog() override;

    QString build_results_tsv() const;

    bool load_selected_graph(Graph& graph,
                             QList<Node*>& out_nodes,
                             QList<Edge*>& out_edges,
                             QString* error_message = nullptr);

private slots:
    void on_search_clicked();
    void on_reset_clicked();
    void on_selection_changed();
    void on_open_clicked();
    void on_delete_clicked();
    void on_table_double_clicked(int row, int column);
    void on_copy_details_clicked();
    void on_export_svg_clicked();
    void on_export_tsv_clicked();

private:
    void build_ui();
    void connect_signals();
    void refresh_results();
    void populate_table(const QList<Graph_Record>& rows);
    void update_details_panel();
    Graph_Query_Filter current_filter() const;
    qint64 selected_graph_id() const;
    void clear_details_panel();
    void set_metadata_value(QLabel* label, const QString& value);
    void set_preview_svg(const QByteArray& svg_bytes);
    QString build_details_text(const Graph_Record& rec) const;
    static bool build_svg_bytes(const Graph_Repository& repo,
                                const Graph_Record& rec,
                                QByteArray* out_graph_svg_bytes,
                                QString* error_message = nullptr);

    Graph_Repository m_repo;
    QList<Graph_Record> m_rows;
    qint64 m_selected_id = -1;
    QByteArray m_selected_svg_bytes;

    QLineEdit* m_title_edit = nullptr;
    QSpinBox* m_node_count_spin = nullptr;
    QSpinBox* m_edge_count_spin = nullptr;
    QSpinBox* m_group_count_spin = nullptr;
    QSpinBox* m_face_count_spin = nullptr;
    QSpinBox* m_delta_t_spin = nullptr;
    QComboBox* m_non_reducible_combo = nullptr;

    QPushButton* m_search_button = nullptr;
    QPushButton* m_reset_button = nullptr;
    QPushButton* m_export_tsv_button = nullptr;
    QPushButton* m_open_button = nullptr;
    QPushButton* m_delete_button = nullptr;
    QPushButton* m_cancel_button = nullptr;

    QLabel* m_status_label = nullptr;
    QTableWidget* m_table = nullptr;
    QScrollArea* m_details_scroll = nullptr;
    QLabel* m_details_title = nullptr;
    QLabel* m_details_id = nullptr;
    QLabel* m_details_created = nullptr;
    QLabel* m_details_short_signature = nullptr;
    QLabel* m_details_json_length = nullptr;
    QLabel* m_details_node_count = nullptr;
    QLabel* m_details_edge_count = nullptr;
    QLabel* m_details_group_count = nullptr;
    QLabel* m_details_face_count = nullptr;
    QLabel* m_details_delta_t = nullptr;
    QLabel* m_details_span = nullptr;
    QLabel* m_details_non_reducible = nullptr;
    QLabel* m_details_wa = nullptr;
    QLabel* m_details_w0 = nullptr;
    QLabel* m_details_p0 = nullptr;
    QLabel* m_details_pa = nullptr;
    QLabel* m_invariants_warning_label = nullptr;
    QWidget* m_invariants_tables_widget = nullptr;
    QVBoxLayout* m_invariants_tables_layout = nullptr;
    QTableWidget* m_distribution_table = nullptr;
    QGraphicsView* m_preview_view = nullptr;
    QGraphicsScene* m_preview_scene = nullptr;
    QPushButton* m_copy_details_button = nullptr;
    QPushButton* m_export_svg_button = nullptr;
};

#endif  // GRAPH_BROWSER_DIALOG_HPP
