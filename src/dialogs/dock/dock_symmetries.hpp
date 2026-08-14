#ifndef DOCK_SYMMETRIES_HPP
#define DOCK_SYMMETRIES_HPP

#include <QDockWidget>

class Graph;
class QListWidget;
class QLabel;
class Symmetry_Overlay;

class Dock_Symmetries : public QDockWidget
{
    Q_OBJECT

   public:
    explicit Dock_Symmetries(QWidget* parent = nullptr);
    void set_graph(const Graph& graph);
    void clear_graph();

   public slots:
    void refresh();

   private:
    const Graph* m_graph = nullptr;
    QLabel* m_summary = nullptr;
    QListWidget* m_orbits = nullptr;
    Symmetry_Overlay* m_overlay = nullptr;
};

#endif  // DOCK_SYMMETRIES_HPP
