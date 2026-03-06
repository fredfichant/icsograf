#ifndef POINT_MATH_HPP
#define POINT_MATH_HPP

#include <qmath.h>

#include <QLineF>
#include <QPointF>
#include <cmath>

inline bool qFuzzyCompare(QPointF p1, QPointF p2)
{
    const double epsilon = 0.0001;
    return std::abs(p1.x() - p2.x()) <= epsilon && std::abs(p1.y() - p2.y()) <= epsilon;
}

inline double point_distance_squared(const QPointF& p1, const QPointF& p2)
{
    QPointF d = p1 - p2;
    return d.x() * d.x() + d.y() * d.y();
}

inline double point_distance(const QPointF& p1, const QPointF& p2)
{
    return qSqrt(point_distance_squared(p1, p2));
}

/**
 * Project point to line
 */
inline QPointF project(QPointF point, QLineF line)
{
    QPointF intersection;
    QLineF normalLine = line.normalVector().translated(point - line.p1());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    line.intersects(normalLine, &intersection);
#else
    line.intersect(normalLine, &intersection);
#endif
    return intersection;
}

constexpr inline long double pi() { return 3.1415926535897932384626433832L; }
constexpr inline long double deg2rad(long double deg) { return deg / 180 * pi(); }
constexpr inline long double rad2deg(long double rad) { return rad * 180 / pi(); }

#endif  // POINT_MATH_HPP
