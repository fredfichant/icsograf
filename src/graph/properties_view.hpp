#ifndef PROPERTIES_VIEW_HPP
#define PROPERTIES_VIEW_HPP

#include <QWidget>

class Graph_Properties;
class QLabel;
class QFormLayout;
class QVBoxLayout;
class QTableWidget;

class Properties_View : public QWidget
{
    Q_OBJECT
   public:
    explicit Properties_View(const Graph_Properties& properties, QWidget* parent = nullptr);

   public slots:
    void update_view();

   private:
    const Graph_Properties& m_properties;
    QLabel* m_node_count_label;
    QLabel* m_edge_count_label;
    QLabel* m_group_count_label;
    QLabel* m_face_count_label;
    QTableWidget* m_degree_table;
    QTableWidget* m_edge_distribution_table;
    QLabel* m_delta_t_label;
    QVBoxLayout* m_main_layout;
    QFormLayout* m_form_layout;
};

#endif  // PROPERTIES_VIEW_HPP
