/**
 * \file src/dialogs/export_image_dialog.cpp
 * \brief Implementation of export image dialog dialog and window components.
 */

#include "export_image_dialog.hpp"

#include <QColorDialog>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
#include <QShowEvent>

#include "image_exporter.hpp"

Export_Image_Dialog::Export_Image_Dialog(QWidget* parent)
    : QDialog(parent), view(nullptr), m_backgroundColor(Qt::white)
{
    setupUi(this);
    check_properties->setChecked(true);
}

void Export_Image_Dialog::set_view(const Knot_View* v)
{
    // if ( !view || file_name.isEmpty() )
    file_name = v->file_name();

    view = v;
    reset_size();
}

void Export_Image_Dialog::changeEvent(QEvent* e)
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

void Export_Image_Dialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    if (!event->spontaneous()) reset_size();
}

void Export_Image_Dialog::on_button_svg_clicked()
{
    QString exname =
        QFileDialog::getSaveFileName(this, tr("Export Knot as SVG"), file_name,
                                     "SVG Images (*.svg);;XML files (*.xml);;All files (*)");

    if (exname.isNull()) return;

    QFile quf(exname);
    file_name = exname;

    export_svg(quf, view->graph(), check_graph->isChecked(), check_properties->isChecked());
}

bool Export_Image_Dialog::file_ok(QFile& file)
{
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("File Error"),
                             tr("Could not write to \"%1\".").arg(file.fileName()));
        return false;
    }
    return true;
}

void Export_Image_Dialog::reset_size() {}

