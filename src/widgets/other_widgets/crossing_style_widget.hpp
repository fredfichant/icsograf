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
    void edge_type_changed(Edge_Type*);
    void enabled_styles_changed(Edge_Style::Enabled_Styles);
    void edge_slide_changed(double);
    void spacing_changed(double);
    void strand_count_changed(int);

   private slots:
    void checkbox_toggled(int style);
    void emit_edge_slide(int percent);
    void reload_edge_types();
    void on_combo_edge_type_activated(int index);

   private:
    /// Copy tooltip from buddy to label
    void label_tooltip();

    /// Edge type associated with combo box index
    Edge_Type* edge_type(int index) const;
    Edge_Type* edge_type() const;

    void set_edge_type(Edge_Type* type);
};

#endif  // CROSSING_STYLE_WIDGET_HPP
