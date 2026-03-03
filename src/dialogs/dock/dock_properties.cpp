#include "dock_properties.hpp"

#include "graph_properties.hpp"
#include "properties_view.hpp"

Dock_Properties::Dock_Properties(const Graph_Properties* properties, QWidget* parent)
    : QDockWidget(parent)
{
    setObjectName("dock_properties");
    setWindowTitle(tr("Graph Properties"));
    if (properties)
        m_view = new Properties_View(*properties, this);
    else
        m_view = new Properties_View(*new Graph_Properties(this), this);
    setWidget(m_view);
}

void Dock_Properties::set_properties(const Graph_Properties& properties)
{
    if (m_view) {
        delete m_view;
    }
    m_view = new Properties_View(properties, this);
    setWidget(m_view);
}
