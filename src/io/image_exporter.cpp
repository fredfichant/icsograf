/**
 * \file src/io/image_exporter.cpp
 * \brief Implementation of image exporter import and export routines.
 */

#include "image_exporter.hpp"
#include "graph_properties.hpp"

#include <QSvgGenerator>

void export_svg(QIODevice& file, const Graph& graph, bool draw_graph, bool include_properties)
{
    if (!file.isWritable() && !file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    graph.update_properties();
    Graph_Properties* props = graph.properties();

    QRectF fibr = graph.full_image_bounding_rect();
    if (fibr.width() <= 0) fibr.setWidth(1);
    if (fibr.height() <= 0) fibr.setHeight(1);

    const double panel_spacing = draw_graph ? 40.0 : 0.0;
    const double panel_width = fibr.width();
    const double content_width = draw_graph ? (panel_width * 2.0 + panel_spacing) : panel_width;

    const double text_height = include_properties ? 100.0 : 0.0;
    QRectF viewBox(0, 0, content_width, fibr.height() + text_height);

    QSvgGenerator gen;
    gen.setOutputDevice(&file);
    gen.setTitle("état nœudien");

    QString desc = props->summary_text();
    gen.setDescription(desc);
    gen.setViewBox(viewBox.toRect());

    QPainter painter;
    painter.begin(&gen);
    painter.translate(-fibr.topLeft());

    if (draw_graph) {
        graph.paint_graph(&painter);
        painter.translate(panel_width + panel_spacing, 0);
    }

    graph.const_paint(&painter);

    if (include_properties) {
        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setPointSize(10);
        painter.setFont(font);

        if (draw_graph) {
            painter.translate(-(panel_width + panel_spacing), 0);
        }

        double y = fibr.bottom() + 20;
        painter.drawText(fibr.left(), y, desc);

        y += 15;
        painter.drawText(fibr.left(), y, props->vertex_distribution_text());

        y += 15;
        painter.drawText(fibr.left(), y, props->face_distribution_text());
    }

    painter.end();
}
