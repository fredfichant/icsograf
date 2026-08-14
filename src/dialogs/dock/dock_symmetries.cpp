#include "dock_symmetries.hpp"

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "graph.hpp"
#include "graph_symmetry_analyzer.hpp"
#include "symmetry_overlay.hpp"

Dock_Symmetries::Dock_Symmetries(QWidget* parent) : QDockWidget(parent)
{
    setObjectName("dock_symmetries");
    setWindowTitle(tr("Symmetries"));
    auto* content = new QWidget(this);
    auto* layout = new QVBoxLayout(content);
    auto* refresh_button = new QPushButton(tr("Analyze"), content);
    m_summary = new QLabel(content);
    m_orbits = new QListWidget(content);
    layout->addWidget(refresh_button);
    layout->addWidget(m_summary);
    layout->addWidget(new QLabel(tr("Structural orbits"), content));
    layout->addWidget(m_orbits);
    setWidget(content);
    connect(refresh_button, &QPushButton::clicked, this, &Dock_Symmetries::refresh);
}

void Dock_Symmetries::set_graph(const Graph& graph)
{
    clear_graph();
    m_graph = &graph;
    m_overlay = new Symmetry_Overlay(graph);
    m_overlay->setParentItem(const_cast<Graph*>(&graph));
    refresh();
}

void Dock_Symmetries::clear_graph()
{
    delete m_overlay;
    m_overlay = nullptr;
    m_graph = nullptr;
    if (m_orbits) m_orbits->clear();
    if (m_summary) m_summary->clear();
}

void Dock_Symmetries::refresh()
{
    m_orbits->clear();
    if (!m_graph) {
        m_summary->setText(tr("Geometric reflections: 0\nStructural automorphisms: 0\nGenerators: 0"));
        return;
    }
    if (m_graph->nodes().isEmpty()) {
        m_summary->setText(tr("Geometric reflections: 0\nStructural automorphisms: 0\nGenerators: 0"));
        return;
    }
    const Graph_Symmetry_Result result = Graph_Symmetry_Analyzer::find_automorphisms(*m_graph);
    if (m_overlay) m_overlay->set_result(result.reflection_axes, result.orbits);
    m_summary->setText(tr("Geometric reflections: %1\nStructural automorphisms: %2%3\nGenerators: %4")
                           .arg(result.reflection_axes.size())
                           .arg(result.group_order)
                           .arg(result.complete ? QString() : tr(" (partial)"))
                           .arg(result.generators.size()));
    for (const QVector<int>& orbit : result.orbits) {
        QStringList labels;
        for (int vertex : orbit) labels.append(QString::number(vertex + 1));
        m_orbits->addItem(tr("{%1}").arg(labels.join(QStringLiteral(", "))));
    }
}
