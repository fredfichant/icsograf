#ifndef CROSSING_STYLE_WIDGET_HPP
#define CROSSING_STYLE_WIDGET_HPP

#include <QSignalMapper>

#include "edge_style.hpp"
#include "edge_type.hpp"
#include "ui_crossing_style_widget.h"

class Crossing_Style_Widget : public QWidget, private Ui::Crossing_Style_Widget
{
    Q_OBJECT

    QSignalMapper mapper;

   public:
    explicit Crossing_Style_Widget(QWidget* parent = 0);

    void set_style(const Edge_Style& st);
    Edge_Style get_style() const;

    Edge_Style::Enabled_Styles enabled_styles() const;

   protected:
    void changeEvent(QEvent* e);

   signals:
    void handle_length_changed(double);
    void crossing_distance_changed(double);
    void enabled_styles_changed(Edge_Style::Enabled_Styles);
    void edge_slide_changed(double);

   private slots:
    void checkbox_toggled(int style);
    void emit_edge_slide(int percent);

   private:
    /// Copy tooltip from buddy to label
    void label_tooltip();

    Edge_Type* m_edge_type;
    double m_spacing;
    int m_strand_count;
};

#endif  // CROSSING_STYLE_WIDGET_HPP
