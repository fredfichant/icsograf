#include "graph_properties.hpp"

Graph_Properties::Graph_Properties(QObject* parent)
    : QObject(parent),
      m_node_count(0),
      m_edge_count(0),
      m_group_count(0),
      m_face_count(0),
      m_wa(0),
      m_w0(0),
      m_p0(0),
      m_pa(0)
{
    m_vertex_degree_distribution = QMap<int, int>();
    m_face_degree_distribution = QMap<int, int>();
}

void Graph_Properties::set_node_count(int count)
{
    if (m_node_count != count) {
        m_node_count = count;
        emit properties_changed();
    }
}

void Graph_Properties::set_edge_count(int count)
{
    if (m_edge_count != count) {
        m_edge_count = count;
        emit properties_changed();
    }
}

void Graph_Properties::set_group_count(int count)
{
    if (m_group_count != count) {
        m_group_count = count;
        emit properties_changed();
    }
}

void Graph_Properties::set_face_count(int count)
{
    if (m_face_count != count) {
        m_face_count = count;
        emit properties_changed();
    }
}

void Graph_Properties::set_vertex_degree_distribution(const QMap<int, int>& dist)
{
    if (m_vertex_degree_distribution != dist) {
        m_vertex_degree_distribution = dist;
        emit properties_changed();
    }
}

void Graph_Properties::set_face_degree_distribution(const QMap<int, int>& dist)
{
    if (m_face_degree_distribution != dist) {
        m_face_degree_distribution = dist;
        emit properties_changed();
    }
}

void Graph_Properties::set_edge_distribution(int wa, int w0, int p0, int pa)
{
    if (m_wa != wa || m_w0 != w0 || m_p0 != p0 || m_pa != pa) {
        m_wa = wa;
        m_w0 = w0;
        m_p0 = p0;
        m_pa = pa;
        emit properties_changed();
    }
}

QString Graph_Properties::summary_text() const
{
    return QString("sommets: %1, arêtes: %2, faces: %3, Delta T: %4")
        .arg(node_count())
        .arg(edge_count())
        .arg(face_count())
        .arg(delta_t());
}

QString Graph_Properties::vertex_distribution_text() const
{
    QString dist = "Vertex Degree Distribution: ";
    for (int deg : m_vertex_degree_distribution.keys())
        dist += QString("%1:%2 ").arg(deg).arg(m_vertex_degree_distribution.value(deg));
    return dist;
}

QString Graph_Properties::face_distribution_text() const
{
    QString dist = "Face Degree Distribution: ";
    for (int deg : m_face_degree_distribution.keys())
        dist += QString("%1:%2 ").arg(deg).arg(m_face_degree_distribution.value(deg));
    return dist;
}
