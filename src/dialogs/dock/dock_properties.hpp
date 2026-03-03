#ifndef DOCK_PROPERTIES_HPP
#define DOCK_PROPERTIES_HPP
#include <QDockWidget>

class Properties_View;
class Graph_Properties;
class Dock_Properties : public QDockWidget

{
    Q_OBJECT

   public:
    explicit Dock_Properties(const Graph_Properties* properties = nullptr,
                             QWidget* parent = nullptr);

    void set_properties(const Graph_Properties& properties);

   private:
    Properties_View* m_view;
};

#endif  // DOCK_PROPERTIES_HPP
