#include "edge_type_widget.hpp"

#include "resource_manager.hpp"

Edge_Type_Widget::Edge_Type_Widget(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
    label_tooltip();

    reload_edge_types();

    connect(Resource_Manager::pointer(), SIGNAL(edge_types_changed()), SLOT(reload_edge_types()));

    connect(spin_strand_count, SIGNAL(valueChanged(int)), SIGNAL(strand_count_changed(int)));
    connect(spin_strand_offset, SIGNAL(valueChanged(double)),
            SIGNAL(strand_offset_changed(double)));
}

void Edge_Type_Widget::set_style(const Edge_Style& st)
{
    spin_strand_count->setValue(st.strand_count);
    spin_strand_offset->setValue(st.strand_offset);

    if (st.enabled_style & Edge_Style::EDGE_TYPE) {
        set_edge_type(st.edge_type);
    }
}

Edge_Style Edge_Type_Widget::get_style() const
{
    return Edge_Style(24, 10, 0.5,  // Defaults for other fields
                      edge_type(), enabled_styles(), spin_strand_count->value(),
                      spin_strand_offset->value());
}

Edge_Style::Enabled_Styles Edge_Type_Widget::enabled_styles() const
{
    Edge_Style::Enabled_Styles es = Edge_Style::NOTHING;
    es |= Edge_Style::STRAND_COUNT;
    es |= Edge_Style::STRAND_OFFSET;
    es |= Edge_Style::EDGE_TYPE;
    return es;
}

void Edge_Type_Widget::set_edge_type(Edge_Type* type)
{
    for (int i = 0; i < combo_edge_type->count(); i++) {
        Edge_Type* cs = edge_type(i);
        if (cs == type) {
            combo_edge_type->setCurrentIndex(i);
            break;
        }
    }
}

Edge_Type* Edge_Type_Widget::edge_type() const
{
    return edge_type(combo_edge_type->currentIndex());
}

Edge_Type* Edge_Type_Widget::edge_type(int index) const
{
    return combo_edge_type->itemData(index).value<Edge_Type*>();
}

void Edge_Type_Widget::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange: {
            retranslateUi(this);
            label_tooltip();
        } break;
        default:
            break;
    }
}

void Edge_Type_Widget::on_combo_edge_type_activated(int index)
{
    emit edge_type_changed(edge_type(index));
}

void Edge_Type_Widget::label_tooltip()
{
    for (QLabel* l : findChildren<QLabel*>()) {
        QWidget* bud = l->buddy();
        if (bud) l->setToolTip(bud->toolTip());
    }
}

void Edge_Type_Widget::reload_edge_types()
{
    int current_index = combo_edge_type->currentIndex();
    Edge_Type* current_type = edge_type();

    blockSignals(true);
    combo_edge_type->clear();

    for (int i = 0; i < resource_manager().edge_types().size(); i++) {
        Edge_Type* et = resource_manager().edge_types()[i];
        if (et == current_type) current_index = i;
        combo_edge_type->addItem(et->icon(), et->name(), QVariant::fromValue(et));
    }

    if (current_index >= combo_edge_type->count()) current_index = 0;

    combo_edge_type->setCurrentIndex(current_index);
    blockSignals(false);

    Edge_Type* new_type = edge_type(current_index);
    if (new_type != current_type) emit edge_type_changed(new_type);
}
