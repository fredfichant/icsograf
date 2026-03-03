#ifndef EDGE_TYPE_WIDGET_HPP
#define EDGE_TYPE_WIDGET_HPP

#include <QSignalMapper>

#include "edge_style.hpp"
#include "edge_type.hpp"
#include "ui_edge_type_widget.h"

class Edge_Type_Widget : public QWidget, private Ui::Edge_Type_Widget
{
    Q_OBJECT

   public:
    explicit Edge_Type_Widget(QWidget* parent = 0);

    void set_style(const Edge_Style& st);
    Edge_Style get_style() const;

    Edge_Style::Enabled_Styles enabled_styles() const;

   protected:
    void changeEvent(QEvent* e);

   signals:
    void edge_type_changed(Edge_Type*);
    void enabled_styles_changed(Edge_Style::Enabled_Styles);
    void strand_count_changed(int);
    void strand_offset_changed(double);

   private slots:
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

#endif  // EDGE_TYPE_WIDGET_HPP
