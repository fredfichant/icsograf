/**
 * \file properties_view.hpp
 * \brief A widget for displaying graph properties.
 */

#ifndef PROPERTIES_VIEW_HPP
#define PROPERTIES_VIEW_HPP

#include <QWidget>

class Graph_Properties;
class QLabel;
class QFormLayout;
class QScrollArea;
class QVBoxLayout;
class QTableWidget;

/**
 * \brief A widget that displays the properties of a Graph.
 *
 * This view listens to a Graph_Properties object and updates its display
 * whenever the properties change. It shows information like node/edge counts,
 * degree distributions, and edge distribution statistics.
 */
class Properties_View : public QWidget
{
    Q_OBJECT
   public:
    /**
     * \brief Constructs a Properties_View.
     * \param properties The Graph_Properties object to display.
     * \param parent The parent widget.
     */
    explicit Properties_View(const Graph_Properties& properties, QWidget* parent = nullptr);

   public slots:
    /**
     * \brief Updates the view with the latest data from the Graph_Properties object.
     *
     * This slot is connected to the `properties_changed` signal of the
     * Graph_Properties object.
     */
    void update_view();

   private:
    const Graph_Properties& m_properties;      ///< Reference to the properties data model.
    QLabel* m_node_count_label;                ///< Label to display the number of nodes.
    QLabel* m_edge_count_label;                ///< Label to display the number of edges.
    QLabel* m_group_count_label;               ///< Label to display the number of disconnected groups (strands).
    QLabel* m_face_count_label;                ///< Label to display the number of faces.
    QTableWidget* m_degree_table;              ///< Table to display vertex and face degree distributions.
    QLabel* m_edge_distribution_warning_label; ///< Warning shown when too many tables exist.
    QScrollArea* m_edge_distribution_scroll_area;  ///< Scroll area dedicated to the 2x2 tables.
    QWidget* m_edge_distribution_tables_widget;  ///< Container for the 2x2 distribution tables.
    QVBoxLayout* m_edge_distribution_tables_layout;  ///< Layout for the 2x2 distribution tables.
    QLabel* m_delta_t_label;                   ///< Label to display the Delta T value.
    QLabel* m_span_formula_label;              ///< Label to display the span formula (portance P).
    QLabel* m_non_reducible_label;             ///< Label to display reducibility status.
    QVBoxLayout* m_main_layout;                ///< Main vertical layout for the widget.
    QFormLayout* m_form_layout;                ///< Form layout for simple key-value properties.
};

#endif  // PROPERTIES_VIEW_HPP
