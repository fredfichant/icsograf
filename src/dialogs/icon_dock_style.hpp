#ifndef ICON_DOCK_STYLE_HPP
#define ICON_DOCK_STYLE_HPP

#include <QDockWidget>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOption>
#include <QWidget>

/**
 * @brief Style proxy pour afficher l'icône d'un QDockWidget dans sa barre de titre.
 *
 * Cette classe s'appuie sur le style de base du dock cible et surcharge le rendu du contrôle
 * `QStyle::CE_DockWidgetTitle` afin de :
 * - dessiner l'icône de fenêtre du dock à gauche du titre ;
 * - ajuster la zone de texte du titre pour éviter le chevauchement avec l'icône.
 */
class Icon_Dock_Style : public QProxyStyle
{
    Q_OBJECT

    // QDockWidget* target;
   public:
    /**
     * @brief Construit le style proxy pour un dock donné.
     * @param target Dock dont le style courant est utilisé comme style de base.
     */
    Icon_Dock_Style(QDockWidget* target) : QProxyStyle(target->style()) {}

    /**
     * @brief Redessine certains contrôles de style.
     *
     * Lorsque `element` vaut `QStyle::CE_DockWidgetTitle`, la méthode dessine l'icône du dock
     * puis décale la zone de rendu du titre avant de déléguer au style de base.
     *
     * @param element Type de contrôle à dessiner.
     * @param option Options de style fournies par Qt.
     * @param painter Peintre utilisé pour le rendu.
     * @param widget Widget concerné par le rendu.
     */
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
