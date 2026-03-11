/**
 * \file src/io/image_exporter.cpp
 * \brief Implementation of image exporter import and export routines.
 */

#include "image_exporter.hpp"

#include <QSvgGenerator>

#include "graph_properties.hpp"

void export_svg(QIODevice& file, const Graph& graph, bool draw_graph)
{
    if (!file.isWritable() && !file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    graph.update_properties();
    Graph_Properties* props = graph.properties();

    QRectF fibr = graph.full_image_bounding_rect();
    if (fibr.width() <= 0) fibr.setWidth(1);
    if (fibr.height() <= 0) fibr.setHeight(1);

    // Add space for properties text
    double text_height = 100;
    QRectF viewBox(0, 0, fibr.width(), fibr.height() + text_height);

    QSvgGenerator gen;
    gen.setOutputDevice(&file);
    gen.setTitle("état nœudien");

    QString desc = props->summary_text();
    gen.setDescription(desc);
    gen.setViewBox(viewBox.toRect());

    QPainter painter;
    painter.begin(&gen);
    painter.translate(-fibr.topLeft());

    if (draw_graph) graph.paint_graph(&painter);

    graph.const_paint(&painter);

    // Render properties text at the bottom
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    double y = fibr.bottom() + 20;
    painter.drawText(fibr.left(), y, desc);

    y += 15;
    painter.drawText(fibr.left(), y, props->vertex_distribution_text());

    y += 15;
    painter.drawText(fibr.left(), y, props->face_distribution_text());

    painter.end();
}
