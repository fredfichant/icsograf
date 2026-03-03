#include "dialog_edge_properties.hpp"

Dialog_Edge_Properties::Dialog_Edge_Properties(QWidget* parent) : QDialog(parent) { setupUi(this); }

void Dialog_Edge_Properties::set_style(const Edge_Style& style)
{
    crossing_style_widget->set_style(style);
}

Edge_Style Dialog_Edge_Properties::edge_style() const { return crossing_style_widget->get_style(); }

void Dialog_Edge_Properties::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            retranslateUi(this);
            break;
        default:
            break;
    }
}
