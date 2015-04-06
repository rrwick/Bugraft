#include "graphicsviewzoom.h"
#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>
#include "../program/settings.h"

GraphicsViewZoom::GraphicsViewZoom(QGraphicsView* view)
    : QObject(view), _view(view)
{
    g_zoom = 1.0;
    _view->viewport()->installEventFilter(this);
    _view->setMouseTracking(true);
    _modifiers = Qt::ControlModifier;
}

void GraphicsViewZoom::gentle_zoom(double factor, bool mouseWheel) {

    if (g_zoom * factor >= g_settings->maxZoom)
        factor = g_settings->maxZoom / g_zoom;
    if (g_zoom * factor <= g_settings->minZoom)
        factor = g_settings->minZoom / g_zoom;

    //I put this bit in so it's easier to get back to exactly 100%, which is where
    //the background pixmap will look best.
    if (g_zoom * factor < 1.03 && g_zoom * factor > 0.97)
        factor = 1.0 / g_zoom;

    g_zoom *= factor;

    _view->scale(factor, factor);

    if (!mouseWheel)
        return;

    _view->centerOn(target_scene_pos);


    QPointF delta_viewport_pos = target_viewport_pos - QPointF(_view->viewport()->width() / 2.0,
                                                               _view->viewport()->height() / 2.0);
    QPointF viewport_center = _view->mapFromScene(target_scene_pos) - delta_viewport_pos;
    _view->centerOn(_view->mapToScene(viewport_center.toPoint()));
    emit zoomed();
}

void GraphicsViewZoom::set_modifiers(Qt::KeyboardModifiers modifiers) {
    _modifiers = modifiers;

}

bool GraphicsViewZoom::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        QPointF delta = target_viewport_pos - mouse_event->pos();
        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
            target_viewport_pos = mouse_event->pos();
            target_scene_pos = _view->mapToScene(mouse_event->pos());
        }
    } else if (event->type() == QEvent::Wheel) {
        QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
        if (QApplication::keyboardModifiers() == _modifiers) {
            if (wheel_event->orientation() == Qt::Vertical) {
                double angle = wheel_event->angleDelta().y();
                double factor = qPow(g_settings->zoomFactor, angle);
                gentle_zoom(factor, true);
                return true;
            }
        }
    }
    Q_UNUSED(object)
    return false;
}
