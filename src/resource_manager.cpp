#include "resource_manager.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QNetworkRequest>
#include <QStyle>

#include "knotter_info.hpp"

void Resource_Manager::initialize()
{
    qApp->setApplicationName(TARGET);
    qApp->setApplicationVersion(program.version());
    qApp->setOrganizationDomain(DOMAIN_NAME);
    qApp->setOrganizationName(TARGET);

    // Clean up
    connect(qApp, SIGNAL(aboutToQuit()), pointer(), SLOT(save_settings()));

    // Initialize Icon theme
    QString iconPath = QCoreApplication::applicationDirPath() + "/../data/img/icons";

    QIcon::setThemeSearchPaths({iconPath});
    QIcon::setThemeName("knotter-icons");

    // network
    m_network_access_manager = new QNetworkAccessManager;

    // Load Settings: note after load_plugins
    settings.load_config();
}

Resource_Manager::~Resource_Manager()
{
    for (Edge_Type* es : m_edge_types) delete es;

#if !QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    delete m_network_access_manager;
#endif
}

Resource_Manager& Resource_Manager::instance()
{
    static Resource_Manager singleton;
    return singleton;
}

Resource_Manager* Resource_Manager::pointer() { return &instance(); }

void Resource_Manager::save_settings() { settings.save_config(); }

void Resource_Manager::register_edge_type(Edge_Type* type)
{
    m_edge_types.push_back(type);
    emit edge_types_changed();
}

void Resource_Manager::remove_edge_type(Edge_Type* type)
{
    m_edge_types.removeOne(type);
    emit edge_types_changed();
}

Edge_Type* Resource_Manager::default_edge_type()
{
    if (m_edge_types.empty()) return nullptr;
    return m_edge_types.front();
}

Edge_Type* Resource_Manager::next_edge_type(Edge_Type* type)
{
    int sz = m_edge_types.size();
    for (int i = 0; i < sz; i++)
        if (m_edge_types[i] == type) return m_edge_types[(i + 1) % sz];
    return default_edge_type();
}

Edge_Type* Resource_Manager::prev_edge_type(Edge_Type* type)
{
    int sz = m_edge_types.size();
    for (int i = sz - 1; i >= 0; i--) {
        if (m_edge_types[i] == type) {
            if (i == 0) return m_edge_types.back();

            return m_edge_types[i - 1];
        }
    }
    return default_edge_type();
}

Edge_Type* Resource_Manager::edge_type_from_machine_name(QString name)
{
    for (Edge_Type* st : m_edge_types)
        if (st->machine_name() == name) return st;
    return default_edge_type();
}

void Resource_Manager::register_cusp_shape(Cusp_Shape* style)
{
    m_cusp_shapes.push_back(style);
    emit cusp_shapes_changed();
}

void Resource_Manager::remove_cusp_shape(Cusp_Shape* shape)
{
    m_cusp_shapes.removeOne(shape);
    emit cusp_shapes_changed();
}

Cusp_Shape* Resource_Manager::default_cusp_shape()
{
    if (m_cusp_shapes.empty()) return nullptr;
    return m_cusp_shapes.front();
}

Cusp_Shape* Resource_Manager::cusp_shape_from_machine_name(QString name)
{
    for (Cusp_Shape* st : m_cusp_shapes)
        if (st->machine_name() == name) return st;
    return default_cusp_shape();
}

QNetworkReply* Resource_Manager::network_get(QString url)
{
    return m_network_access_manager->get(QNetworkRequest(QUrl(url)));
}
