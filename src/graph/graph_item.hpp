/**
 * \file src/graph/graph_item.hpp
 * \brief API declarations for graph item graph data structures and operations.
 */

#ifndef GRAPH_ITEM_HPP
#define GRAPH_ITEM_HPP
#include <QGraphicsObject>
#include <QObject>
#include <QPainter>
class Graph;

class Graph_Item : public QGraphicsObject
{
    Q_OBJECT

   protected:
    bool highlighted;
    bool visible;

   public:
    Graph_Item();

    // virtual double distance_squared(QPointF to) const = 0;

    void set_highlighted(bool h) { highlighted = h; }
    void set_visible(bool h) { visible = h; }
};

#endif  // GRAPH_ITEM_HPP
