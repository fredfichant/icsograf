#ifndef DIALOG_EDGE_PROPERTIES_HPP
#define DIALOG_EDGE_PROPERTIES_HPP

#include "ui_dialog_edge_properties.h"

class Dialog_Edge_Properties : public QDialog, private Ui::Dialog_Edge_Properties
{
    Q_OBJECT

   public:
    explicit Dialog_Edge_Properties(QWidget* parent = 0);

    void set_style(const Edge_Style& style);
    Edge_Style edge_style() const;

   protected:
    void changeEvent(QEvent* e);
};

#endif  // DIALOG_EDGE_PROPERTIES_HPP
