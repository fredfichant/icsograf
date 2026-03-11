/**
 * \file src/widgets/knot_view/transform_handle.hpp
 * \brief API declarations for transform handle interactions in the knot editor view.
 */

#ifndef TRANSFORM_HANDLE_HPP
#define TRANSFORM_HANDLE_HPP

#include <QSvgRenderer>

#include "graph_item.hpp"

class Transform_Handle : public Graph_Item
{
   public:
    /// What kind of transformation this handle applies
    enum Mode
    {
        SCALE,
        ROTATE
    };

   protected:
    static QSvgRenderer scale_rest;     ///< SVG Image to display while at rest
    static QSvgRenderer scale_active;   ///< SVG Image to display when highlight == true
    static QSvgRenderer rotate_rest;    ///< SVG Image to display while at rest
    static QSvgRenderer rotate_active;  ///< SVG Image to display when highlight == true
    static double m_image_size;         ///< Size of the image
    static bool images_initialized;     ///< whether static images have been initialized

    Mode m_mode;
    int m_image_angle;  ///< Angle for which the image has to be rotated
    double m_angle;     ///< Dynamic rotation

   public:
    explicit Transform_Handle(Mode mode = SCALE, int image_angle = 0);

    QRectF boundingRect() const override;

    void set_mode(Mode mode);
    Mode mode() const { return m_mode; }

    int image_angle() const { return m_image_angle; }
    void set_image_angle(int angle);

    static double image_size() { return m_image_size; }

    void set_angle(double angle);

   protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
};

#endif  // TRANSFORM_HANDLE_HPP
