/**
 * \file src/widgets/status_bar.cpp
 * \brief Implementation of status bar widget components.
 */

#include "status_bar.hpp"

#include <QDoubleSpinBox>
#include <QLabel>
#include <limits>

#include "knot_view.hpp"

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent), m_view(nullptr)
{
    m_zoomer = new QDoubleSpinBox(this);
    m_zoomer->setMinimum(0.01);
    m_zoomer->setMaximum(800);
    m_zoomer->setSuffix("%");
    m_zoomer->setValue(100);

    QString labels[] = {tr("X"), tr("Y"), tr("W"), tr("H")};
    for (int i = 0; i < 4; i++) {
        m_sceneWidgets[i] = new QDoubleSpinBox(this);
        m_sceneWidgets[i]->setMinimum(-std::numeric_limits<double>::max());
        m_sceneWidgets[i]->setMaximum(std::numeric_limits<double>::max());
        m_sceneWidgets[i]->setSizePolicy(QSizePolicy::Preferred,
                                         m_sceneWidgets[i]->sizePolicy().verticalPolicy());
        m_sceneWidgets[i]->setMaximumWidth(80);
        addPermanentWidget(new QLabel(labels[i]));
        addPermanentWidget(m_sceneWidgets[i]);
    }
    m_sceneWidgets[2]->setReadOnly(true);
    m_sceneWidgets[3]->setReadOnly(true);
    connect(m_sceneWidgets[0], SIGNAL(valueChanged(double)), SLOT(viewportXy()));
    connect(m_sceneWidgets[1], SIGNAL(valueChanged(double)), SLOT(viewportXy()));

    addPermanentWidget(new QLabel(tr("Zoom")));
    addPermanentWidget(m_zoomer);
    connect(m_zoomer, SIGNAL(valueChanged(double)), this, SLOT(applyZoom()));
}

StatusBar::~StatusBar() = default;

void StatusBar::connectView(Knot_View* view)
{
    m_view = view;
    if (!view) return;

    m_zoomer->blockSignals(true);
    m_zoomer->setValue(view->get_zoom_factor() * 100);
    m_zoomer->blockSignals(false);

    connect(view, SIGNAL(zoomed(double)), m_zoomer, SLOT(setValue(double)));
    connect(view, SIGNAL(scene_rect_changed(QRectF)), this, SLOT(viewportChanged(QRectF)));
    connect(view, SIGNAL(mose_position_changed(QPointF)), this, SLOT(updateMousePos(QPointF)));
}

void StatusBar::disconnectView(Knot_View* view)
{
    if (m_view == view) {
        disconnect(view, nullptr, this, nullptr);
        m_view = nullptr;
    }
}

void StatusBar::updateMousePos(QPointF pos)
{
    showMessage(tr("(%1,%2)").arg(pos.x()).arg(pos.y()));
}

void StatusBar::viewportChanged(QRectF rect)
{
    for (int i = 0; i < 4; i++) m_sceneWidgets[i]->blockSignals(true);
    m_sceneWidgets[0]->setValue(rect.x());
    m_sceneWidgets[1]->setValue(rect.y());
    m_sceneWidgets[2]->setValue(rect.width());
    m_sceneWidgets[3]->setValue(rect.height());
    for (int i = 0; i < 4; i++) m_sceneWidgets[i]->blockSignals(false);
}

void StatusBar::applyZoom()
{
    if (m_view) {
        m_view->set_zoom(m_zoomer->value() / 100.0);
        viewportChanged(QRectF(m_view->mapToScene(0, 0),
                               m_view->mapToScene(m_view->width(), m_view->height())));
    }
}

void StatusBar::viewportXy()
{
    if (m_view) {
        m_view->translate_view_to(QPointF(m_sceneWidgets[0]->value(), m_sceneWidgets[1]->value()));
    }
}
