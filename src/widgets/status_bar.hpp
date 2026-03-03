#ifndef STATUS_BAR_HPP
#define STATUS_BAR_HPP

#include <QStatusBar>
#include <memory>

class QDoubleSpinBox;
class QLabel;
class Knot_View;

class StatusBar : public QStatusBar
{
    Q_OBJECT

   public:
    explicit StatusBar(QWidget* parent = nullptr);
    ~StatusBar();

    void connectView(Knot_View* view);
    void disconnectView(Knot_View* view);

   public slots:
    void updateMousePos(QPointF pos);
    void viewportChanged(QRectF rect);
    void applyZoom();

   private slots:
    void viewportXy();

   private:
    Knot_View* m_view;
    QDoubleSpinBox* m_zoomer;
    QDoubleSpinBox* m_sceneWidgets[4];
};

#endif  // STATUS_BAR_HPP
