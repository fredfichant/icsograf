#include "dialog_edge_properties.hpp"

Dialog_Edge_Properties::Dialog_Edge_Properties(QWidget* parent) : QDialog(parent) { setupUi(this); }

void Dialog_Edge_Properties::set_style(const Edge_Style& style)
{
    edge_type_widget->set_style(style);
    crossing_style_widget->set_style(style);
}

Edge_Style Dialog_Edge_Properties::edge_style() const
{
    Edge_Style cs = crossing_style_widget->get_style();
    Edge_Style et = edge_type_widget->get_style();

    // Merge them
    Edge_Style merged = cs;
    merged.edge_type = et.edge_type;
    merged.spacing = et.spacing;
    merged.strand_count = et.strand_count;
    merged.enabled_style |= et.enabled_style;

    return merged;
}

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
