#include <cassert>

#include <QApplication>
#include <QImage>
#include <QPainter>

#include "edge_2strand.hpp"
#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "graph.hpp"

namespace {

bool image_has_non_transparent_pixels(const QImage& image)
{
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (qAlpha(image.pixel(x, y)) != 0) return true;
        }
    }
    return false;
}

QImage create_canvas()
{
    QImage image(220, 220, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    return image;
}

}  // namespace

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    {
        Node node(QPointF(0, 0));

        QImage image = create_canvas();
        QPainter painter(&image);
        painter.translate(110, 110);
        node.paint(&painter);
        painter.end();

        assert(image_has_non_transparent_pixels(image));
    }

    {
        Edge_Normal normal;
        Node a(QPointF(20, 110));
        Node b(QPointF(200, 110));
        Edge edge(&a, &b, &normal);
        Graph graph;
        graph.add_node(&a);
        graph.add_node(&b);
        graph.add_edge(&edge);

        QImage image = create_canvas();
        QPainter painter(&image);
        edge.paint(&painter);
        painter.end();

        assert(image_has_non_transparent_pixels(image));
    }

    {
        Edge_2Strand two_strand;
        Node a(QPointF(20, 110));
        Node b(QPointF(200, 110));
        Edge edge(&a, &b, &two_strand);
        Graph graph;
        graph.add_node(&a);
        graph.add_node(&b);
        graph.add_edge(&edge);

        QImage image = create_canvas();
        QPainter painter(&image);
        edge.paint(&painter);
        painter.end();

        assert(qAlpha(image.pixel(110, 106)) != 0);
        assert(qAlpha(image.pixel(110, 114)) != 0);
        assert(qAlpha(image.pixel(110, 110)) == 0);
    }

    {
        Edge_2Strand two_strand;
        Node a(QPointF(20, 110));
        Node b(QPointF(200, 110));
        Edge edge(&a, &b, &two_strand);
        Graph graph;
        graph.add_node(&a);
        graph.add_node(&b);
        graph.add_edge(&edge);
        edge.set_marking_color(Qt::blue);

        QImage image = create_canvas();
        QPainter painter(&image);
        edge.paint(&painter);
        painter.end();

        assert(image.pixelColor(110, 106).blue() > 0);
        assert(image.pixelColor(110, 114).blue() > 0);
        assert(qAlpha(image.pixel(110, 110)) == 0);
    }

    {
        Edge_3Strand three_strand;
        Node a(QPointF(20, 110));
        Node b(QPointF(200, 110));
        Edge edge(&a, &b, &three_strand);
        Graph graph;
        graph.add_node(&a);
        graph.add_node(&b);
        graph.add_edge(&edge);

        QImage image = create_canvas();
        QPainter painter(&image);
        edge.paint(&painter);
        painter.end();

        assert(qAlpha(image.pixel(110, 104)) != 0);
        assert(qAlpha(image.pixel(110, 110)) != 0);
        assert(qAlpha(image.pixel(110, 116)) != 0);
    }

    {
        Node node(QPointF(0, 0));

        QImage image = create_canvas();
        image.fill(Qt::black);

        QPainter painter(&image);
        painter.translate(110, 110);
        node.paint(&painter);
        painter.end();

        assert(image.pixelColor(116, 110) != QColor(Qt::black));
    }

    return 0;
}
