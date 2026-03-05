#include "cusp_style_widget.hpp"

#include "resource_manager.hpp"

Cusp_Style_Widget::Cusp_Style_Widget(QWidget* parent) : QWidget(parent), m_cusp_shape(nullptr)
{
    setupUi(this);
    label_tooltip();

    connect(spin_handle_length, SIGNAL(valueChanged(double)),
            SIGNAL(handle_length_changed(double)));
    connect(spin_cusp_angle, SIGNAL(valueChanged(double)), SIGNAL(cusp_angle_changed(double)));
    connect(spin_cusp_distance, SIGNAL(valueChanged(double)),
            SIGNAL(cusp_distance_changed(double)));

    mapper.setMapping(check_cusp_angle, Node_Style::CUSP_ANGLE);
    mapper.setMapping(check_cusp_distance, Node_Style::CUSP_DISTANCE);
    mapper.setMapping(check_handle_length, Node_Style::HANDLE_LENGTH);
    connect(&mapper, SIGNAL(mapped(int)), SLOT(checkbox_toggled(int)));
    for (QCheckBox* cb : findChildren<QCheckBox*>()) {
        connect(cb, SIGNAL(clicked()), &mapper, SLOT(map()));
    }
}

void Cusp_Style_Widget::set_style(const Node_Style& st)
{
    m_cusp_shape = st.cusp_shape;

    if (st.enabled_style & Node_Style::CUSP_ANGLE) {
        spin_cusp_angle->setValue(st.cusp_angle);
        check_cusp_angle->setChecked(true);
    } else
        check_cusp_angle->setChecked(false);

    if (st.enabled_style & Node_Style::CUSP_DISTANCE) {
        spin_cusp_distance->setValue(st.cusp_distance);
        check_cusp_distance->setChecked(true);
    } else
        check_cusp_distance->setChecked(false);

    if (st.enabled_style & Node_Style::HANDLE_LENGTH) {
        spin_handle_length->setValue(st.handle_length);
        check_handle_length->setChecked(true);
    } else
        check_handle_length->setChecked(false);
}

Node_Style Cusp_Style_Widget::get_style() const
{
    return Node_Style(spin_cusp_angle->value(), spin_handle_length->value(),
                      spin_cusp_distance->value(), m_cusp_shape, enabled_styles());
}

Node_Style::Enabled_Styles Cusp_Style_Widget::enabled_styles() const
{
    Node_Style::Enabled_Styles es = Node_Style::NOTHING;
    if (check_cusp_angle->isChecked()) es |= Node_Style::CUSP_ANGLE;
    if (check_cusp_distance->isChecked()) es |= Node_Style::CUSP_DISTANCE;
    if (check_handle_length->isChecked()) es |= Node_Style::HANDLE_LENGTH;
    return es;
}

void Cusp_Style_Widget::changeEvent(QEvent* e)
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

void Cusp_Style_Widget::checkbox_toggled(int style)
{
    emit enabled_styles_changed(enabled_styles());

    if (qobject_cast<QCheckBox*>(mapper.mapping(style))->isChecked()) {
        switch (Node_Style::Enabled_Styles_Enum(style)) {
            case Node_Style::CUSP_ANGLE:
                emit cusp_angle_changed(spin_cusp_angle->value());
                break;
            case Node_Style::CUSP_DISTANCE:
                emit cusp_distance_changed(spin_cusp_distance->value());
                break;
            case Node_Style::HANDLE_LENGTH:
                emit handle_length_changed(spin_handle_length->value());
                break;
            case Node_Style::NOTHING:
            case Node_Style::EVERYTHING:
            case Node_Style::CUSP_SHAPE:
                break;
        }
    }
}

void Cusp_Style_Widget::label_tooltip()
{
    for (QLabel* l : findChildren<QLabel*>()) {
        QWidget* bud = l->buddy();
        if (bud) l->setToolTip(bud->toolTip());
    }
}
