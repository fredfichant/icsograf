#include "crossing_style_widget.hpp"

#include "resource_manager.hpp"

Crossing_Style_Widget::Crossing_Style_Widget(QWidget* parent)
    : QWidget(parent), m_edge_type(nullptr), m_spacing(10.0), m_strand_count(1)
{
    setupUi(this);
    label_tooltip();

    connect(spin_handle_length, SIGNAL(valueChanged(double)),
            SIGNAL(handle_length_changed(double)));
    connect(spin_crossing_gap, SIGNAL(valueChanged(double)),
            SIGNAL(crossing_distance_changed(double)));
    connect(slide_edge_slide, SIGNAL(valueChanged(int)), SLOT(emit_edge_slide(int)));

    mapper.setMapping(check_crossing_gap, Edge_Style::CROSSING_DISTANCE);
    mapper.setMapping(check_handle_length, Edge_Style::HANDLE_LENGTH);
    mapper.setMapping(check_edge_slide, Edge_Style::EDGE_SLIDE);
    connect(&mapper, SIGNAL(mapped(int)), SLOT(checkbox_toggled(int)));
    for (QCheckBox* cb : findChildren<QCheckBox*>()) {
        connect(cb, SIGNAL(clicked()), &mapper, SLOT(map()));
    }
}

void Crossing_Style_Widget::set_style(const Edge_Style& st)
{
    m_edge_type = st.edge_type;
    m_spacing = st.spacing;
    m_strand_count = st.strand_count;

    if (st.enabled_style & Edge_Style::CROSSING_DISTANCE) {
        spin_crossing_gap->setValue(st.crossing_distance);
        check_crossing_gap->setChecked(true);
    } else
        check_crossing_gap->setChecked(false);

    if (st.enabled_style & Edge_Style::EDGE_SLIDE) {
        slide_edge_slide->setValue(st.edge_slide * 100);
        check_edge_slide->setChecked(true);
    } else
        check_edge_slide->setChecked(false);

    if (st.enabled_style & Edge_Style::HANDLE_LENGTH) {
        spin_handle_length->setValue(st.handle_length);
        check_handle_length->setChecked(true);
    } else
        check_handle_length->setChecked(false);
}

Edge_Style Crossing_Style_Widget::get_style() const
{
    return Edge_Style(spin_handle_length->value(), spin_crossing_gap->value(),
                      slide_edge_slide->value() / 100.0, m_edge_type, enabled_styles(),
                      m_spacing, m_strand_count);
}

Edge_Style::Enabled_Styles Crossing_Style_Widget::enabled_styles() const
{
    Edge_Style::Enabled_Styles es = Edge_Style::NOTHING;
    if (check_crossing_gap->isChecked()) es |= Edge_Style::CROSSING_DISTANCE;
    if (check_edge_slide->isChecked()) es |= Edge_Style::EDGE_SLIDE;
    if (check_handle_length->isChecked()) es |= Edge_Style::HANDLE_LENGTH;
    return es;
}

void Crossing_Style_Widget::changeEvent(QEvent* e)
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

void Crossing_Style_Widget::checkbox_toggled(int style)
{
    emit enabled_styles_changed(enabled_styles());

    if (qobject_cast<QCheckBox*>(mapper.mapping(style))->isChecked()) {
        switch (Edge_Style::Enabled_Styles_Enum(style)) {
            case Edge_Style::CROSSING_DISTANCE:
                emit crossing_distance_changed(spin_crossing_gap->value());
                break;
            case Edge_Style::EDGE_SLIDE:
                emit crossing_distance_changed(slide_edge_slide->value());
                break;
            case Edge_Style::HANDLE_LENGTH:
                emit handle_length_changed(spin_handle_length->value());
                break;
            case Edge_Style::SPACING:
            case Edge_Style::STRAND_COUNT:
            case Edge_Style::EDGE_TYPE:
            case Edge_Style::NOTHING:
            case Edge_Style::EVERYTHING:
                break;
        }
    }
}

void Crossing_Style_Widget::label_tooltip()
{
    for (QLabel* l : findChildren<QLabel*>()) {
        QWidget* bud = l->buddy();
        if (bud) l->setToolTip(bud->toolTip());
    }
}

void Crossing_Style_Widget::emit_edge_slide(int percent)
{
    emit edge_slide_changed(percent / 100.0);
}
