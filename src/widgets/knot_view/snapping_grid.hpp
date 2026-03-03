#ifndef SNAPPING_GRID_HPP
#define SNAPPING_GRID_HPP

#include <QObject>
#include <QPainter>

class Snapping_Grid : public QObject
{
    Q_OBJECT

   public:
    enum Grid_Shape
    {
        SQUARE,
        TRIANGLE1,
        TRIANGLE2
    };

    static QColor line_color;

   protected:
    unsigned m_size;
    Grid_Shape m_shape;
    QPointF m_origin;
    bool m_enabled;

   public:
    explicit Snapping_Grid(unsigned size = 32, Grid_Shape shape = SQUARE,
                           QPointF origin = QPointF(0, 0), bool enabled = true);

    /// move p to closest grid point
    void snap(QPointF& p) const;
    /// returns closest grid point
    QPointF nearest(QPointF p) const
    {
        snap(p);
        return p;
    }
    /// returns closest grid point
    QPointF nearest(double x, double y) const { return nearest(QPointF(x, y)); }

    /// draws grid lines that cover at least rect
    void render(QPainter* painter, const QRectF& rect) const;

    bool is_enabled() const { return m_enabled; }

    double size() const { return m_size; }

    Grid_Shape shape() const { return m_shape; }

    QPointF origin() const { return m_origin; }

   public slots:

    void enable(bool enabled);

    void set_size(int size);

    void set_shape(Grid_Shape shape);

    void set_origin(QPointF origin);

   signals:
    void grid_changed();
    void moved(QPointF);
    void shape_changed(int);
    void enabled(bool);
};

#endif  // SNAPPING_GRID_HPP
