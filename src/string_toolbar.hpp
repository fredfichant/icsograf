#ifndef STRING_TOOLBAR_HPP
#define STRING_TOOLBAR_HPP
#include <QString>
#include <QToolBar>

#include "c++.hpp"

/**
 *  This class holds a string definition of toolbars and actions
 *
 *  Separators are stored as empty strings
 */
struct String_Toolbar
{
    QString name;
    QString title;
    QStringList actions;

    String_Toolbar() {}
    String_Toolbar(QString name, QString title, QStringList actions)
        : name(name), title(title), actions(actions)
    {
    }
    String_Toolbar(QToolBar* tb);
    /// Create toolbar and add to widndow
    QToolBar* create(QMainWindow* w) const;
};

#endif  // STRING_TOOLBAR_HPP
