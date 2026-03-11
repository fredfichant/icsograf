/**
 * \file src/dialogs/export_image_dialog.hpp
 * \brief API declarations for the export image dialog dialog or window.
 */

#ifndef EXPORT_IMAGE_DIALOG_HPP
#define EXPORT_IMAGE_DIALOG_HPP

#include <QFile>

#include "knot_view.hpp"
#include "ui_export_image_dialog.h"

class Export_Image_Dialog : public QDialog, private Ui::Export_Image_Dialog
{
    Q_OBJECT
   private:
    const Knot_View* view;
    QString file_name;
    QColor m_backgroundColor;

   public:
    explicit Export_Image_Dialog(QWidget* parent = 0);

    void set_view(const Knot_View* v);

   protected:
    void changeEvent(QEvent* e) override;
    void showEvent(QShowEvent* event) override;

   private slots:
    void reset_size();
    void on_button_color_background_clicked();

    void on_button_svg_clicked();

   private:
    /**
     *  \brief Check whether file is open for writing
     */
    bool file_ok(QFile& file);
    void updateColorButton(QPushButton* button, const QColor& color);
};

#endif  // EXPORT_IMAGE_DIALOG_HPP
