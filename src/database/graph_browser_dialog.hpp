/**
 * \file src/database/graph_browser_dialog.hpp
 * \brief Dialog declarations for browsing, filtering, previewing, and loading stored graphs.
 */

#ifndef GRAPH_BROWSER_DIALOG_HPP
#define GRAPH_BROWSER_DIALOG_HPP

#include <QDialog>
#include <QList>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;
class QTextEdit;

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

    bool load_selected_graph(Graph& graph,
                             QList<Node*>& out_nodes,
                             QList<Edge*>& out_edges,
                             QString* error_message = nullptr);

    static QString build_details_html(const Graph_Record& rec,
                                      const QString& graph_svg_data_uri,
                                      const QString& node_svg_data_uri);
    static bool build_svg_data_uris(const Graph_Repository& repo,
                                    const Graph_Record& rec,
                                    QString* out_graph_svg_data_uri,
                                    QString* out_node_svg_data_uri,
                                    QString* error_message = nullptr);

private slots:
    void on_search_clicked();
    void on_reset_clicked();
    void on_selection_changed();
    void on_open_clicked();
    void on_delete_clicked();
    void on_table_double_clicked(int row, int column);

private:
    void build_ui();
    void connect_signals();
    void refresh_results();
    void populate_table(const QList<Graph_Record>& rows);
    void update_details_panel();
    Graph_Query_Filter current_filter() const;
    qint64 selected_graph_id() const;

    Graph_Repository m_repo;
    QList<Graph_Record> m_rows;
    qint64 m_selected_id = -1;

    QLineEdit* m_title_edit = nullptr;
    QSpinBox* m_node_count_spin = nullptr;
    QSpinBox* m_edge_count_spin = nullptr;
    QSpinBox* m_group_count_spin = nullptr;
    QSpinBox* m_face_count_spin = nullptr;
    QSpinBox* m_delta_t_spin = nullptr;
    QComboBox* m_non_reducible_combo = nullptr;

    QPushButton* m_search_button = nullptr;
    QPushButton* m_reset_button = nullptr;
    QPushButton* m_open_button = nullptr;
    QPushButton* m_delete_button = nullptr;
    QPushButton* m_cancel_button = nullptr;

    QLabel* m_status_label = nullptr;
    QTableWidget* m_table = nullptr;
    QTextEdit* m_details_text = nullptr;
};

#endif  // GRAPH_BROWSER_DIALOG_HPP
