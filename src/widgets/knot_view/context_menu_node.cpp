/**
 * \file src/widgets/knot_view/context_menu_node.cpp
 * \brief Implementation of context menu node interactions in the knot view.
 */

#include "context_menu_node.hpp"

#include "commands.hpp"
#include "cusp_style_dialog.hpp"

Context_Menu_Node::Context_Menu_Node(Knot_View* parent) : QMenu(parent), view(parent)
{
    action_snap =
        addAction(QIcon::fromTheme("snap-orthogonal"), tr("Snap to grid"), this, SLOT(snap()));
    addAction(QIcon::fromTheme("format-remove-node"), tr("Remove"), this, SLOT(remove()));
    addAction(QIcon::fromTheme("edit-node"), tr("Properties..."), this, SLOT(properties()));
    action_reset_style = addAction(tr("Reset custom style"), this, SLOT(reset_custom_style()));

    setTitle(tr("Node Context Menu"));
}

void Context_Menu_Node::popup(Node* n, QPoint pos)
{
    node = n;
    action_snap->setEnabled(view->grid().is_enabled());
    action_reset_style->setEnabled(node->style().enabled_style != Node_Style::NOTHING);

    QMenu::popup(pos);
}

void Context_Menu_Node::reset_custom_style()
{
    view->push_command(new Node_Style_Enable(node, node->style().enabled_style, Node_Style::NOTHING,
                                             tr("Reset Node Style"), view));
}

void Context_Menu_Node::properties()
{
    Cusp_Style_Dialog dialog;
    dialog.set_style(node->style());
    if (dialog.exec()) {
        view->push_command(new Node_Style_All(node, node->style(), dialog.node_style(), view));
    }
}

void Context_Menu_Node::remove() { view->remove_node(node); }

void Context_Menu_Node::snap()
{
    view->begin_macro(tr("Snap to Grid"));
    view->push_command(new Move_Node(node, node->pos(), view->grid().nearest(node->pos()), view));
    view->end_macro();
}
