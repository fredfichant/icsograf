/**
 * \file src/widgets/knot_view/context_menu_edge.hpp
 * \brief API declarations for context menu edge interactions in the knot editor view.
 */

#ifndef CONTEXT_MENU_EDGE_HPP
#define CONTEXT_MENU_EDGE_HPP

#include <QMenu>
#include <QSignalMapper>

#include "knot_view.hpp"

class Context_Menu_Edge : public QMenu
{
    Q_OBJECT
   private:
    Knot_View* view;
    Edge* edge;
    QAction* action_snap;
    QMenu* menu_edge_types;
    QSignalMapper mapper;
    QActionGroup* edge_type_actions;
    QAction* action_reset_style;

   public:
    explicit Context_Menu_Edge(Knot_View* parent = 0);

    void popup(Edge* e, QPoint pos);

   public slots:
    void snap();
    void change_edge_type(QString type_name);
    void remove();
    void break_intersections();
    void subdivide();
    void reset_custom_style();
};

#endif  // CONTEXT_MENU_EDGE_HPP
