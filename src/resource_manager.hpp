#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <QNetworkAccessManager>
#include <QObject>
#include <QTranslator>

#include "application_info.hpp"
#include "edge_type.hpp"
#include "settings.hpp"

/**
 * Manage resources and data
 */
class Resource_Manager : public QObject
{
    Q_OBJECT

   private:
    Resource_Manager() {}
    Resource_Manager(const Resource_Manager&);
    Resource_Manager& operator=(const Resource_Manager&);
    ~Resource_Manager();

    QList<Edge_Type*> m_edge_types;
    QList<Cusp_Shape*> m_cusp_shapes;

    QNetworkAccessManager* m_network_access_manager;

   public:
    Settings settings;
    Application_Info program;

    static Resource_Manager& instance();
    static Resource_Manager* pointer();

    /**
     * \brief Initialize the resource system
     * \note It must be called after the QApplication has been initialized
     */
    void initialize();

    /**
     *  \brief Register an edge style
     *
     *  Takes ownership of the style pointer
     *
     *  \param style Style to be registered
     */
    void register_edge_type(Edge_Type* type);

    /**
     * \brief Removes an edge type
     *
     * The object is not destroyed but loses ownership, the caller
     * becomes responsible of destructing the shape object.
     */
    void remove_edge_type(Edge_Type* type);

    /**
     *  \brief get the default edge style
     *
     *  \returns the first inserted style or nullptr if there is no style
     */
    Edge_Type* default_edge_type();

    QList<Edge_Type*> edge_types() { return m_edge_types; }

    /**
     *  \brief Cycle edge styles
     *
     *  Returns a different edge style so that calling next_edge_style n times
     *  ( where n is edge_styles().size() ) will return all installed styles.
     *
     *  Only returns NULL if there is no style available
     */
    Edge_Type* next_edge_type(Edge_Type* style);
    /**
     *  \brief Cycle edge styles
     *
     *  Same as next_edge_style() but reversed
     *
     *  \sa next_edge_style
     */
    Edge_Type* prev_edge_type(Edge_Type* type);

    /**
     *  \brief Get edge style from its machine-readable name
     *
     *  Scans every register style to check a match to the given name,
     *  if none is found, the default style is returned.
     *
     *  Resurns NULL only if there are no registered styles
     */
    Edge_Type* edge_type_from_machine_name(QString name);

    /**
     *  \brief Register a cusp style
     *
     *  Takes ownership of the style pointer
     *
     *  \param style Style to be registered
     */
    void register_cusp_shape(Cusp_Shape* style);

    /**
     * \brief Removes a cusp style
     *
     * The object is not destroyed but loses ownership, the caller
     * becomes responsible of destructing the shape object.
     */
    void remove_cusp_shape(Cusp_Shape* shape);

    QList<Cusp_Shape*> cusp_shapes() { return m_cusp_shapes; }

    Cusp_Shape* default_cusp_shape();
    /**
     *  \brief Get cusp shape from its machine-readable name
     *
     *  Scans every register style to check a match to the given name,
     *  if none is found, the default style is returned.
     *
     *  Resurns NULL only if there are no registered styles
     */
    Cusp_Shape* cusp_shape_from_machine_name(QString name);

    // network
    QNetworkAccessManager* network_access_manager() { return m_network_access_manager; }

    QNetworkReply* network_get(QString url);

   public slots:

    void save_settings();

   signals:

    /// Emitted when a cusp shape is registered or removed
    void cusp_shapes_changed();

    /// Emitted when an edge type is registered or removed
    void edge_types_changed();
};

inline Resource_Manager& resource_manager() { return Resource_Manager::instance(); }
#endif  // RESOURCE_MANAGER_HPP
