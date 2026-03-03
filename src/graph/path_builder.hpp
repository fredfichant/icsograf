#ifndef PATH_BUILDER_HPP
#define PATH_BUILDER_HPP

#include <QPainterPath>

#include "path_item.hpp"

/**
 *  Class that builds a SVG path incrementally
 */
class Path_Builder
{
   protected:
    typedef QList<path_item::Line*> group;
    typedef QList<group> container;
    typedef group::iterator iterator;

    /**
     *  \brief Contains the path data
     *
     *  The data is stored in groups, hence the nested list
     */
    container strokes;

    Path_Builder(const Path_Builder&);
    Path_Builder& operator=(const Path_Builder&);

   public:
    Path_Builder();
    ~Path_Builder();

    void add_line(path_item::Line* l);
    void add_line(QPointF begin, QPointF end);
    void add_cubic(QPointF begin, QPointF control1, QPointF control2, QPointF end);
    void add_quad(QPointF begin, QPointF control, QPointF end);

    /// next calls to add_* will be placed in a different group
    void new_group();

    QList<QPainterPath> build();
};

#endif  // PATH_BUILDER_HPP
