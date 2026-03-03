#ifndef EDGE_TYPE_UTILS_HPP
#define EDGE_TYPE_UTILS_HPP

#include <QPointF>
#include <QString>

#include "edge_handle.hpp"

class Edge;

QString handleToString(Edge_Handle handle);
QPointF get_handle_pos(const Edge* edge, Edge_Handle handle);

#endif  // EDGE_TYPE_UTILS_HPP
