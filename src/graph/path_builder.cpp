/**
 * \file src/graph/path_builder.cpp
 * \brief Implementation of path builder graph logic.
 */

#include "path_builder.hpp"

Path_Builder::Path_Builder() {}

Path_Builder::~Path_Builder()
{
    foreach (const container::value_type& ll, strokes)
        foreach (group::value_type stroke, ll) delete stroke;
    strokes.clear();
}

void Path_Builder::add_line(path_item::Line* current)
{
    // qDebug() << "Path_Builder::add_line: Ajout segment" << current->begin << "->" << current->end;

    if (strokes.empty()) {
        new_group();
    }

    group& last_group = strokes.back();

    // check adjacent strokes to merge them
    for (iterator i = last_group.begin(); i != last_group.end(); ++i) {
        if (path_item::adjacent(*i, current)) {
            // qDebug() << "  -> Fusion avec segment existant (adjacent)";
            path_item::Line* merged = path_item::merge(*i, current);
            last_group.erase(i);
            // check adjacent on the other end
            for (iterator j = last_group.begin(); j != last_group.end();) {
                if (*j != merged && path_item::adjacent(merged, *j)) {
                    // qDebug() << "  -> Fusion secondaire (pont entre deux segments)";
                    merged = path_item::merge(merged, *j);
                    strokes.back().erase(j);
                    // j = strokes.back().begin();
                    break;
                } else
                    ++j;
            }
            strokes.back().push_back(merged);
            return;
        }
    }
    // qDebug() << "  -> Pas d'adjacence, ajout comme nouveau segment dans le groupe";
    strokes.back().push_back(current);
}

void Path_Builder::add_line(QPointF begin, QPointF end)
{
    add_line(new path_item::Line(begin, end));
}

void Path_Builder::add_cubic(QPointF begin, QPointF control1, QPointF control2, QPointF end)
{
    add_line(new path_item::Cubic_Curve(begin, control1, control2, end));
}

void Path_Builder::add_quad(QPointF begin, QPointF control, QPointF end)
{
    add_line(new path_item::Quad_Curve(begin, control, end));
}

void Path_Builder::new_group() { 
    // qDebug() << "Path_Builder::new_group: Nouveau groupe de traits";
    strokes.push_back(group()); 
}

QList<QPainterPath> Path_Builder::build()
{
    if (strokes.empty()) return QList<QPainterPath>();

    QList<QPainterPath> paths;

    foreach (const container::value_type& ll, strokes) {
        if (ll.empty()) continue;

        paths.push_back(QPainterPath());

        foreach (path_item::Line* stroke, ll) {
            // bool need_move = paths.back().isEmpty() || (paths.back().currentPoint() !=
            // stroke->begin);
            bool need_move =
                paths.back().isEmpty() || (paths.back().currentPosition() != stroke->begin);
            stroke->add_to(need_move, paths.back());
        }
    }

    return paths;
}
