/**
 * \file src/widgets/knot_view/transform_handle.cpp
 * \brief Implementation of transform handle interactions in the knot view.
 */

#include "transform_handle.hpp"

QSvgRenderer Transform_Handle::scale_rest;
QSvgRenderer Transform_Handle::scale_active;
QSvgRenderer Transform_Handle::rotate_rest;
QSvgRenderer Transform_Handle::rotate_active;
double Transform_Handle::m_image_size = 24;
bool Transform_Handle::images_initialized = false;

Transform_Handle::Transform_Handle(Mode mode, int image_angle)
    : m_mode(mode), m_image_angle(image_angle), m_angle(0)
{
    if (!images_initialized) {
        images_initialized = true;

        scale_rest.load(QStringLiteral(":img/handle_scale_rest.svg"));
        scale_active.load(QStringLiteral(":img/handle_scale_active.svg"));
        rotate_rest.load(QStringLiteral(":img/handle_rotate_rest.svg"));
        rotate_active.load(QStringLiteral(":img/handle_rotate_active.svg"));
    }
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

QRectF Transform_Handle::boundingRect() const
{
    const double sz = m_image_size;
    return QRectF(QPointF(-sz, -sz), QSizeF(sz * 2, sz * 2));
}

void Transform_Handle::set_mode(Transform_Handle::Mode mode) { m_mode = mode; }

void Transform_Handle::set_image_angle(int angle) { m_image_angle = angle; }

void Transform_Handle::set_angle(double angle) { m_angle = angle; }

void Transform_Handle::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QSvgRenderer* rend;
    if (m_mode == ROTATE)
        rend = highlighted ? &rotate_active : &rotate_rest;
    else
        rend = highlighted ? &scale_active : &scale_rest;

    painter->rotate(-m_image_angle - m_angle);
    painter->translate(-m_image_size / 2, -m_image_size / 2);
    rend->render(painter, QRectF(-m_image_size / 2, -m_image_size / 2, m_image_size, m_image_size));
}
