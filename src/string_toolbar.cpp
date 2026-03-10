#include "string_toolbar.hpp"

#include <QMainWindow>
#include <QMenu>

String_Toolbar::String_Toolbar(QToolBar* tb) : name(tb->objectName()), title(tb->windowTitle())
{
    for (QAction* a : tb->actions()) actions.push_back(a->objectName());
}

QToolBar* String_Toolbar::create(QMainWindow* w) const
{
    QToolBar* toolbar = new QToolBar(title, w);
    toolbar->setObjectName(name);

    w->addToolBar(Qt::TopToolBarArea, toolbar);

    for (const QString& item : actions) {
        if (item.isEmpty())
            toolbar->addSeparator();
        else {
            QAction* act = w->findChild<QAction*>(item);

            if (!act) {
                QMenu* menu = w->findChild<QMenu*>(item);
                if (menu) act = menu->menuAction();
            }

            if (act)
                toolbar->addAction(act);
            else
                qWarning() << QObject::tr("Warning:") << QObject::tr("Unknown action %1").arg(item);
        }
    }

    return toolbar;
}
