
#include "cusp_style_dialog.hpp"

Cusp_Style_Dialog::Cusp_Style_Dialog(QWidget* parent) : QDialog(parent) { setupUi(this); }

void Cusp_Style_Dialog::set_style(const Node_Style& style) { cusp_style_widget->set_style(style); }

Node_Style Cusp_Style_Dialog::node_style() const { return cusp_style_widget->get_style(); }

void Cusp_Style_Dialog::changeEvent(QEvent* e)
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
