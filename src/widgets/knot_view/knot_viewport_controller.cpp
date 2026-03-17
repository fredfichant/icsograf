/**
 * \file src/widgets/knot_view/knot_viewport_controller.cpp
 * \brief Internal viewport/navigation helper for Knot_View.
 */

#include "knot_viewport_controller.hpp"

#include <QCursor>
#include <QPainter>
#include <QPrinter>
#include <QtGlobal>

#include "knot_view.hpp"

void Knot_Viewport_Controller::translate_view(QPointF delta)
{
    m_view.translate(delta);
    expand_scene_rect(10);
}

void Knot_Viewport_Controller::translate_view_to(QPointF destination)
{
    translate_view(m_view.mapToScene(0, 0) - destination);
}

void Knot_Viewport_Controller::zoom_view(double factor)
{
    if (m_view.get_zoom_factor() * factor < 0.01) return;

    QPoint mouse_pos = m_view.mapFromGlobal(QCursor::pos());
    QPointF scene_before = m_view.mapToScene(mouse_pos);

    QRectF rect(m_view.mapToScene(0, 0),
                m_view.mapToScene(m_view.width() / factor, m_view.height() / factor));
    rect.translate(-rect.bottomRight() / 2);
    m_view.setSceneRect(m_view.sceneRect().united(rect));

    m_view.scale(factor, factor);
    if (m_view.rect().contains(mouse_pos)) {
        QPointF scene_after = m_view.mapToScene(mouse_pos);
        m_view.translate(scene_after - scene_before);
    }
    expand_scene_rect();

    emit m_view.zoomed(100 * m_view.get_zoom_factor());
}

void Knot_Viewport_Controller::set_zoom(double factor)
{
    if (factor < 0.01) return;
    QTransform transform(factor, m_view.transform().m12(), m_view.transform().m13(),
                         m_view.transform().m21(), factor, m_view.transform().m23(),
                         m_view.transform().m31(), m_view.transform().m32(),
                         m_view.transform().m33());
    m_view.setTransform(transform);
    emit m_view.zoomed(100 * factor);
}

void Knot_Viewport_Controller::expand_scene_rect(int margin)
{
    QRectF viewport_rect(m_view.mapToScene(-margin, -margin),
                         m_view.mapToScene(m_view.width() + 2 * margin,
                                           m_view.height() + 2 * margin));
    QRectF scene_rect = m_view.sceneRect();
    if (!scene_rect.contains(viewport_rect)) {
        m_view.setSceneRect(scene_rect.united(viewport_rect));
    }

    emit m_view.scene_rect_changed(
        QRectF(m_view.mapToScene(0, 0), m_view.mapToScene(m_view.width(), m_view.height())));
}

void Knot_Viewport_Controller::view_fit()
{
    if (m_view.m_graph.nodes().empty()) {
        reset_view();
    } else {
        set_zoom(1);
        QRectF area = m_view.m_graph.boundingRect();
        QRectF self_area(m_view.mapToScene(0, 0), m_view.mapToScene(m_view.width(), m_view.height()));

        double zoom_w = self_area.width() / area.width();
        double zoom_h = self_area.height() / area.height();
        double zoom_wh = qMin(zoom_w, zoom_h);

        area.setLeft(area.center().x() - self_area.width() / zoom_wh / 2);
        area.setWidth(self_area.width() / zoom_wh);

        area.setTop(area.center().y() - self_area.height() / zoom_wh / 2);
        area.setHeight(self_area.height() / zoom_wh);

        m_view.setSceneRect(area.united(m_view.sceneRect()));
        set_zoom(zoom_wh);
        m_view.centerOn(area.center());
        expand_scene_rect(10);
    }
}

void Knot_Viewport_Controller::reset_view()
{
    m_view.resetTransform();
    m_view.setSceneRect(-m_view.width() / 2, -m_view.height() / 2, m_view.width(), m_view.height());
    m_view.centerOn(0, 0);
    expand_scene_rect(10);
}

void Knot_Viewport_Controller::print(QPrinter* printer)
{
    QPainter painter(printer);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
    painter.scale(m_view.get_zoom_factor(), m_view.get_zoom_factor());
    painter.translate(-m_view.mapToScene(0, 0));
    m_view.m_graph.const_paint(&painter);
}
