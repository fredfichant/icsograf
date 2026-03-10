#ifndef GRAPH_SAVE_DIALOG_HPP
#define GRAPH_SAVE_DIALOG_HPP

#include <QDialog>

class QLineEdit;
class QLabel;
class QPushButton;

class Graph;
class Graph_Repository;

#include "graph_repository.hpp"

class Graph_Save_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Graph_Save_Dialog(const Graph& graph,
                               QWidget* parent = nullptr);

    Graph_Save_Result save_result() const { return m_result; }

private slots:
    void on_save_clicked();

private:
    void build_ui();

    const Graph& m_graph;
    Graph_Repository m_repo;
    Graph_Save_Result m_result;

    QLabel* m_info_label = nullptr;
    QLineEdit* m_title_edit = nullptr;
    QPushButton* m_save_button = nullptr;
    QPushButton* m_cancel_button = nullptr;
};

#endif  // GRAPH_SAVE_DIALOG_HPP