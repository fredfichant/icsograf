#ifndef EDGE_TYPE_HPP
#define EDGE_TYPE_HPP

#include <QMetaType>
#include <QPainter>

#include "edge.hpp"
#include "path_builder.hpp"

class Edge_Type
{
   public:
    Edge_Type();
    virtual ~Edge_Type() {}
    virtual int strand_count() const { return 1; }
    void paint_regular(QPainter* painter, const Edge& edge);
    void paint_highlighted(QPainter* painter, const Edge& edge);

    virtual void paint(QPainter* painter, const Edge& edge);

    /**
     *  \brief Debug function to draw handles on the edge
     */
    void debug_draw_handles(QPainter* painter, const Edge& edge) const;

    /**
     *  \brief Perform any rendering to path and return the next handle
     */
    virtual Edge::Handle traverse(Edge* edge, Edge::Handle handle, Path_Builder& path) const;
    /**
     *  \brief Get handle geometry
     *  p1 is the point where the path line should pass, p2 the control point
     */
    virtual QLineF handle(const Edge* edge, Edge::Handle handle) const = 0;

    /// (Translated) Human-readable name, used in the UI
    virtual QString name() const = 0;

    /// Machine-readable (unique) name, used as identifier
    virtual QString machine_name() const = 0;

    /// Icon showing the style
    virtual QIcon icon() const = 0;
};

Q_DECLARE_METATYPE(Edge_Type*)

#endif  // EDGE_TYPE_HPP
