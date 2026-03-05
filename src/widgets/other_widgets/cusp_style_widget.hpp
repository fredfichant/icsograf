#ifndef CUSP_STYLE_WIDGET_HPP
#define CUSP_STYLE_WIDGET_HPP

#include <QSignalMapper>

#include "node_style.hpp"
#include "ui_cusp_style_widget.h"

class Cusp_Style_Widget : public QWidget, private Ui::Cusp_Style_Widget
{
    Q_OBJECT

    QSignalMapper mapper;

   public:
    explicit Cusp_Style_Widget(QWidget* parent = 0);

    void set_style(const Node_Style& st);
    Node_Style get_style() const;

    Node_Style::Enabled_Styles enabled_styles() const;

   protected:
    void changeEvent(QEvent* e);

   signals:
    void cusp_angle_changed(double);
    void handle_length_changed(double);
    void cusp_distance_changed(double);
    void enabled_styles_changed(Node_Style::Enabled_Styles);

   private slots:
    void checkbox_toggled(int style);

   private:
    /// Copy tooltip from buddy to label
    void label_tooltip();

    Cusp_Shape* m_cusp_shape;
};

#endif  // CUSP_STYLE_WIDGET_HPP
