#ifndef POLYGON_DIALOG_HPP
#define POLYGON_DIALOG_HPP

#include <QDialog>
#include <memory>

class QUiLoader;
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
    std::unique_ptr<QUiLoader> m_uiLoader;
    std::unique_ptr<QDialog> m_dialog;
};

#endif  // POLYGON_DIALOG_HPP
