/**
 * \file src/widgets/knot_view/context_menu_node.hpp
 * \brief API declarations for context menu node interactions in the knot editor view.
 */

#ifndef CONTEXT_MENU_NODE_HPP
#define CONTEXT_MENU_NODE_HPP

#include <QMenu>

#include "knot_view.hpp"

class Context_Menu_Node : public QMenu
{
    Q_OBJECT
   private:
    Knot_View* view;
    Node* node;
    QAction* action_snap;
    QAction* action_reset_style;

   public:
    explicit Context_Menu_Node(Knot_View* parent = 0);

    void popup(Node* n, QPoint pos);

   public slots:
    void reset_custom_style();
    void properties();
    void snap();
    void remove();
};

#endif  // CONTEXT_MENU_NODE_HPP
