/**
 * \file graph_properties.hpp
 * \brief Statistical information and properties of a knot graph
 */

#ifndef GRAPH_PROPERTIES_HPP
#define GRAPH_PROPERTIES_HPP

#include <QMap>
#include <QObject>

/**
 * \brief Holds various statistics and calculated properties of a Graph
 *
 * This class stores node/edge counts, degree distributions, and other
 * topological properties of the knot graph.
 */
class Graph_Properties : public QObject
{
    Q_OBJECT

    friend class Graph;

   public:
    /**
     * \brief Constructs a Graph_Properties object
     * \param parent Parent QObject
     */
    explicit Graph_Properties(QObject* parent = nullptr);

    /** \brief Returns the number of nodes in the graph */
    int node_count() const { return m_node_count; }

    /** \brief Returns the number of edges in the graph */
    int edge_count() const { return m_edge_count; }

    /** \brief Returns the number of disconnected groups in the graph */
    int group_count() const { return m_group_count; }

    /** \brief Returns the number of faces in the graph */
    int face_count() const { return m_face_count; }

    /** \brief Returns the distribution of vertex degrees */
    const QMap<int, int>& vertex_degree_distribution() const
    {
        return m_vertex_degree_distribution;
    }

    /** \brief Returns the distribution of face degrees */
    const QMap<int, int>& face_degree_distribution() const { return m_face_degree_distribution; }

    // Edge distribution (Step 6)
    /** \brief Weighted average of edge distribution parameter wa */
    int wa() const { return m_wa; }
    /** \brief Weighted average of edge distribution parameter w0 */
    int w0() const { return m_w0; }
    /** \brief Weighted average of edge distribution parameter p0 */
    int p0() const { return m_p0; }
    /** \brief Weighted average of edge distribution parameter pa */
    int pa() const { return m_pa; }

    /**
     * \brief Calculates the Delta T property of the graph
     * \return Absolute difference between strand crossing types
     */
    int delta_t() const { return qAbs((m_wa + m_p0) - (m_w0 + m_pa)); }

    /**
     * \brief Generates a localized summary string of the graph properties
     */
    QString summary_text() const;

    /**
     * \brief Generates a text representation of the vertex degree distribution
     */
    QString vertex_distribution_text() const;

    /**
     * \brief Generates a text representation of the face degree distribution
     */
    QString face_distribution_text() const;

   signals:
    /**
     * \brief Emitted when any of the properties change
     */
    void properties_changed();

   private:
    /** \brief Updates node count and emits properties_changed() if changed */
    void set_node_count(int count);
    /** \brief Updates edge count and emits properties_changed() if changed */
    void set_edge_count(int count);
    /** \brief Updates group count and emits properties_changed() if changed */
    void set_group_count(int count);
    /** \brief Updates face count and emits properties_changed() if changed */
    void set_face_count(int count);
    /** \brief Updates vertex degree distribution and emits properties_changed() if changed */
    void set_vertex_degree_distribution(const QMap<int, int>& dist);
    /** \brief Updates face degree distribution and emits properties_changed() if changed */
    void set_face_degree_distribution(const QMap<int, int>& dist);
    /** \brief Updates edge distribution parameters and emits properties_changed() if changed */
    void set_edge_distribution(int wa, int w0, int p0, int pa);

    int m_node_count;                             ///< Current number of nodes
    int m_edge_count;                             ///< Current number of edges
    int m_group_count;                            ///< Current number of disconnected groups
    int m_face_count;                             ///< Current number of faces
    QMap<int, int> m_vertex_degree_distribution;  ///< Vertex degree -> count
    QMap<int, int> m_face_degree_distribution;    ///< Face degree -> count

    int m_wa, m_w0, m_p0, m_pa;  ///< Edge distribution parameters
};

#endif  // GRAPH_PROPERTIES_HPP
