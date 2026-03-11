/**
 * \file src/dialogs/cusp_style_dialog.hpp
 * \brief API declarations for the cusp style dialog dialog or window.
 */

#ifndef CUSP_STYLE_DIALOG_HPP
#define CUSP_STYLE_DIALOG_HPP

#include <QDialog>

QT_BEGIN_NAMESPACE
class QEvent;
QT_END_NAMESPACE

#include "ui_cusp_style_dialog.h"

class Node_Style;  // ou #include "node_style.hpp" si c'est un type concret (pas une classe)

class Cusp_Style_Dialog : public QDialog, private Ui::Cusp_Style_Dialog
{
    Q_OBJECT

   public:
    explicit Cusp_Style_Dialog(QWidget* parent = nullptr);

    void set_style(const Node_Style& style);
    Node_Style node_style() const;

   protected:
    void changeEvent(QEvent* e) override;
};

#endif  // CUSP_STYLE_DIALOG_HPP
