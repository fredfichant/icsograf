/**
 * \file src/widgets/knot_view/knot_selection_controller.hpp
 * \brief Internal selection and selection-editing helper for Knot_View.
 */

#ifndef KNOT_SELECTION_CONTROLLER_HPP
#define KNOT_SELECTION_CONTROLLER_HPP

#include <QList>

class Knot_View;
class Node;
class Edge;

class Knot_Selection_Controller
{
   public:
    explicit Knot_Selection_Controller(Knot_View& view) : m_view(view) {}

    QList<Node*> selected_nodes() const;
    QList<Edge*> selected_edges() const;
    void update_selection(bool select_edges = true);
    void rubberband_select(QList<Node*> nodes, bool modifier);
    QList<Node*> nodes_in_rubberband() const;
    void select_all();
    void select_connected();
    void snap_selection_to_grid();
    void erase_selection();
    void connect_selection();
    void disconnect_selection();
    void merge_selection();
    void flip_horiz_selection();
    void flip_vert_selection();

   private:
    Knot_View& m_view;
};

#endif  // KNOT_SELECTION_CONTROLLER_HPP
