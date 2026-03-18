/**
 * \file src/dialogs/polygon_dialog.hpp
 * \brief API declarations for the polygon dialog dialog or window.
 */

#ifndef POLYGON_DIALOG_HPP
#define POLYGON_DIALOG_HPP

#include <QDialog>

class Knot_View;

class Polygon_Dialog : public QObject
{
    Q_OBJECT

   public:
    explicit Polygon_Dialog(Knot_View* view, QWidget* parent = nullptr);
    ~Polygon_Dialog();

    void exec();

   private:
    Knot_View* m_view;
    QWidget* m_parent;
};

#endif  // POLYGON_DIALOG_HPP
