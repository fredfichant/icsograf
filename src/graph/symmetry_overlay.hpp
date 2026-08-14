#ifndef SYMMETRY_OVERLAY_HPP
#define SYMMETRY_OVERLAY_HPP

#include <QGraphicsItem>
#include <QLineF>
#include <QVector>

class Graph;

class Symmetry_Overlay : public QGraphicsItem
{
   public:
    explicit Symmetry_Overlay(const Graph& graph);
    void set_result(const QVector<QLineF>& axes, const QVector<QVector<int>>& orbits);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

   private:
    const Graph& m_graph;
    QVector<QLineF> m_axes;
    QVector<QVector<int>> m_orbits;
};

#endif  // SYMMETRY_OVERLAY_HPP
