/**
 * \file src/widgets/knot_view/knot_viewport_controller.hpp
 * \brief Internal viewport/navigation helper for Knot_View.
 */

#ifndef KNOT_VIEWPORT_CONTROLLER_HPP
#define KNOT_VIEWPORT_CONTROLLER_HPP

#include <QPointF>

class Knot_View;
class QPrinter;

class Knot_Viewport_Controller
{
   public:
    explicit Knot_Viewport_Controller(Knot_View& view) : m_view(view) {}

    void translate_view(QPointF delta);
    void translate_view_to(QPointF destination);
    void zoom_view(double factor);
    void set_zoom(double factor);
    void expand_scene_rect(int margin = 0);
    void view_fit();
    void reset_view();
    void print(QPrinter* printer);

   private:
    Knot_View& m_view;
};

#endif  // KNOT_VIEWPORT_CONTROLLER_HPP
