#ifndef ICON_DOCK_STYLE_HPP
#define ICON_DOCK_STYLE_HPP

#include <QDockWidget>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOption>
#include <QWidget>

class Icon_Dock_Style : public QProxyStyle
{
    Q_OBJECT

    // QDockWidget* target;
   public:
    Icon_Dock_Style(QDockWidget* target) : QProxyStyle(target->style()) {}

    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                     const QWidget* widget = 0) const override
    {
        if (element == QStyle::CE_DockWidgetTitle
            // element == QStyle::CE_TabBarTabLabel
        ) {
            int width = pixelMetric(QStyle::PM_SmallIconSize);

            int margin = baseStyle()->pixelMetric(QStyle::PM_DockWidgetTitleMargin);

            QPoint icon_point(margin + option->rect.left(),
                              margin + option->rect.center().y() - width / 2);

            painter->drawPixmap(icon_point, widget->windowIcon().pixmap(width, width));

            const_cast<QStyleOption*>(option)->rect =
                option->rect.adjusted(width + margin, 0, 0, 0);
        }

        baseStyle()->drawControl(element, option, painter, widget);
    }
};

#endif  // ICON_DOCK_STYLE_HPP
