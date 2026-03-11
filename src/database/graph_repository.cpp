/**
 * \file src/database/graph_repository.cpp
 * \brief SQLite repository implementation for graph persistence, filtering, and deletion.
 */

#include "graph_repository.hpp"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>

#include "graph.hpp"
#include "graph_properties.hpp"
#include "graph_validation.hpp"

namespace
{
QString app_data_dir()
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (base.isEmpty()) {
        base = QDir::homePath() + "/.icsograf";
    }

    QDir dir(base);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.absolutePath();
}

QSqlDatabase repo_db(const QString& connection_name)
{
    return QSqlDatabase::database(connection_name, false);
}

QString sql_error_text(const QSqlQuery& q)
{
    return q.lastError().text();
}
}  // namespace

Graph_Repository::Graph_Repository(QObject* parent)
    : QObject(parent),
      m_connection_name(QString("icsograf_repo_%1").arg(
          QUuid::createUuid().toString(QUuid::WithoutBraces))),
      m_database_path(app_data_dir() + "/icsograf_graphs.sqlite")
{
}

Graph_Repository::~Graph_Repository()
{
    {
        QSqlDatabase db = QSqlDatabase::database(m_connection_name, false);
        if (db.isValid()) {
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(m_connection_name);
}

bool Graph_Repository::open(QString* error_message)
{
    if (QSqlDatabase::contains(m_connection_name)) {
        QSqlDatabase db = QSqlDatabase::database(m_connection_name, false);
        if (db.isOpen())
            return true;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connection_name);
    db.setDatabaseName(m_database_path);

    if (!db.open()) {
        if (error_message)
            *error_message = db.lastError().text();
        return false;
    }

    return true;
}

bool Graph_Repository::ensure_open(QString* error_message) const
{
    QSqlDatabase db = repo_db(m_connection_name);
    if (!db.isValid() || !db.isOpen()) {
        if (error_message)
            *error_message = QStringLiteral("database is not open");
        return false;
    }
    return true;
}

bool Graph_Repository::migrate(QString* error_message)
{
    if (!open(error_message))
        return false;
    if (!ensure_open(error_message))
        return false;

    QSqlDatabase db = repo_db(m_connection_name);
    QSqlQuery q(db);

    const char* sql_statements[] = {
        "PRAGMA foreign_keys = ON;",

        "CREATE TABLE IF NOT EXISTS diagrams ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  title TEXT,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
        "  topology_hash TEXT NOT NULL UNIQUE,"
        "  graph_json TEXT NOT NULL,"
        "  node_count INTEGER NOT NULL,"
        "  edge_count INTEGER NOT NULL,"
        "  group_count INTEGER NOT NULL,"
        "  face_count INTEGER NOT NULL,"
        "  wa INTEGER NOT NULL,"
        "  w0 INTEGER NOT NULL,"
        "  p0 INTEGER NOT NULL,"
        "  pa INTEGER NOT NULL,"
        "  delta_t INTEGER NOT NULL,"
        "  span_formula TEXT NOT NULL,"
        "  is_non_reducible INTEGER NOT NULL,"
        "  vertex_degree_distribution_json TEXT NOT NULL,"
        "  face_degree_distribution_json TEXT NOT NULL"
        ");",

        "CREATE INDEX IF NOT EXISTS idx_diagrams_node_count ON diagrams(node_count);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_edge_count ON diagrams(edge_count);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_group_count ON diagrams(group_count);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_face_count ON diagrams(face_count);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_delta_t ON diagrams(delta_t);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_span_formula ON diagrams(span_formula);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_non_reducible ON diagrams(is_non_reducible);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_wa ON diagrams(wa);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_w0 ON diagrams(w0);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_p0 ON diagrams(p0);",
        "CREATE INDEX IF NOT EXISTS idx_diagrams_pa ON diagrams(pa);"};

    for (const char* sql : sql_statements) {
        if (!q.exec(QString::fromUtf8(sql))) {
            if (error_message)
                *error_message = q.lastError().text();
            return false;
        }
    }

    return true;
}

Graph_Save_Result Graph_Repository::save_graph(const Graph& graph, const QString& title)
{
    Graph_Save_Result result;

    QString error;
    if (!migrate(&error)) {
        result.error = error;
        return result;
    }

    const Graph_Validation_Result validation = validate_graph(graph);
    if (!validation.valid) {
        result.error = validation.reason;
        return result;
    }

    const Graph_Properties* properties = graph.properties();
    if (!properties) {
        result.error = QStringLiteral("graph.properties() returned nullptr");
        return result;
    }

    const QByteArray graph_json = serialize_graph(graph);
    const QByteArray fingerprint_json = serialize_properties_fingerprint(*properties);
    const QString hash = sha256_hex(fingerprint_json);

    result.topology_hash = hash;

    QSqlDatabase db = repo_db(m_connection_name);
    QSqlQuery q(db);

    q.prepare(
        "INSERT OR IGNORE INTO diagrams ("
        "  title, topology_hash, graph_json, "
        "  node_count, edge_count, group_count, face_count, "
        "  wa, w0, p0, pa, delta_t, span_formula, is_non_reducible, "
        "  vertex_degree_distribution_json, face_degree_distribution_json"
        ") VALUES ("
        "  :title, :topology_hash, :graph_json, "
        "  :node_count, :edge_count, :group_count, :face_count, "
        "  :wa, :w0, :p0, :pa, :delta_t, :span_formula, :is_non_reducible, "
        "  :vertex_degree_distribution_json, :face_degree_distribution_json"
        ");");

    q.bindValue(":title", title);
    q.bindValue(":topology_hash", hash);
    q.bindValue(":graph_json", QString::fromUtf8(graph_json));

    q.bindValue(":node_count", properties->node_count());
    q.bindValue(":edge_count", properties->edge_count());
    q.bindValue(":group_count", properties->group_count());
    q.bindValue(":face_count", properties->face_count());

    q.bindValue(":wa", properties->wa());
    q.bindValue(":w0", properties->w0());
    q.bindValue(":p0", properties->p0());
    q.bindValue(":pa", properties->pa());
    q.bindValue(":delta_t", properties->delta_t());

    q.bindValue(":span_formula", properties->span_formula());
    q.bindValue(":is_non_reducible", properties->is_non_reducible() ? 1 : 0);

    q.bindValue(":vertex_degree_distribution_json",
                QString::fromUtf8(map_to_json(properties->vertex_degree_distribution())));
    q.bindValue(":face_degree_distribution_json",
                QString::fromUtf8(map_to_json(properties->face_degree_distribution())));

    if (!q.exec()) {
        result.error = sql_error_text(q);
        return result;
    }

    if (q.numRowsAffected() > 0) {
        result.ok = true;
        result.inserted = true;
        result.diagram_id = q.lastInsertId().toLongLong();
        return result;
    }

    QSqlQuery find_q(db);
    find_q.prepare("SELECT id FROM diagrams WHERE topology_hash = :topology_hash;");
    find_q.bindValue(":topology_hash", hash);

    if (!find_q.exec()) {
        result.error = sql_error_text(find_q);
        return result;
    }

    if (find_q.next()) {
        result.ok = true;
        result.inserted = false;
        result.diagram_id = find_q.value(0).toLongLong();
        return result;
    }

    result.error = QStringLiteral("insert ignored but existing row was not found");
    return result;
}

QList<Graph_Record> Graph_Repository::find_graphs_by_title(const QString& title_substring,
                                                           int limit,
                                                           QString* error_message) const
{
    Graph_Query_Filter filter;
    filter.title_contains = title_substring;
    filter.limit = limit;
    return find_graphs(filter, error_message);
}

QList<Graph_Record> Graph_Repository::find_graphs(const Graph_Query_Filter& filter,
                                                  QString* error_message) const
{
    QList<Graph_Record> out;

    if (!ensure_open(error_message))
        return out;

    QSqlDatabase db = repo_db(m_connection_name);

    QString sql =
        "SELECT "
        "  id, title, created_at, topology_hash, graph_json, "
        "  node_count, edge_count, group_count, face_count, "
        "  wa, w0, p0, pa, delta_t, span_formula, is_non_reducible, "
        "  vertex_degree_distribution_json, face_degree_distribution_json "
        "FROM diagrams WHERE 1=1";

    if (filter.node_count >= 0) sql += " AND node_count = :node_count";
    if (filter.edge_count >= 0) sql += " AND edge_count = :edge_count";
    if (filter.group_count >= 0) sql += " AND group_count = :group_count";
    if (filter.face_count >= 0) sql += " AND face_count = :face_count";
    if (filter.wa >= 0) sql += " AND wa = :wa";
    if (filter.w0 >= 0) sql += " AND w0 = :w0";
    if (filter.p0 >= 0) sql += " AND p0 = :p0";
    if (filter.pa >= 0) sql += " AND pa = :pa";
    if (filter.delta_t >= 0) sql += " AND delta_t = :delta_t";
    if (!filter.span_formula.isEmpty()) sql += " AND span_formula = :span_formula";
    if (!filter.title_contains.trimmed().isEmpty()) sql += " AND lower(title) LIKE :title_contains";
    if (filter.is_non_reducible >= 0) sql += " AND is_non_reducible = :is_non_reducible";

    sql += " ORDER BY created_at DESC";

    if (filter.limit > 0) {
        sql += " LIMIT :limit";
    }

    QSqlQuery q(db);
    q.prepare(sql);

    if (filter.node_count >= 0) q.bindValue(":node_count", filter.node_count);
    if (filter.edge_count >= 0) q.bindValue(":edge_count", filter.edge_count);
    if (filter.group_count >= 0) q.bindValue(":group_count", filter.group_count);
    if (filter.face_count >= 0) q.bindValue(":face_count", filter.face_count);
    if (filter.wa >= 0) q.bindValue(":wa", filter.wa);
    if (filter.w0 >= 0) q.bindValue(":w0", filter.w0);
    if (filter.p0 >= 0) q.bindValue(":p0", filter.p0);
    if (filter.pa >= 0) q.bindValue(":pa", filter.pa);
    if (filter.delta_t >= 0) q.bindValue(":delta_t", filter.delta_t);
    if (!filter.span_formula.isEmpty()) q.bindValue(":span_formula", filter.span_formula);
    if (!filter.title_contains.trimmed().isEmpty())
        q.bindValue(":title_contains", "%" + filter.title_contains.trimmed().toLower() + "%");
    if (filter.is_non_reducible >= 0) q.bindValue(":is_non_reducible", filter.is_non_reducible);
    if (filter.limit > 0) q.bindValue(":limit", filter.limit);

    if (!q.exec()) {
        if (error_message)
            *error_message = sql_error_text(q);
        return out;
    }

    while (q.next()) {
        Graph_Record rec;
        rec.id = q.value("id").toLongLong();
        rec.title = q.value("title").toString();
        rec.created_at = q.value("created_at").toString();
        rec.topology_hash = q.value("topology_hash").toString();
        rec.graph_json = q.value("graph_json").toString().toUtf8();

        rec.node_count = q.value("node_count").toInt();
        rec.edge_count = q.value("edge_count").toInt();
        rec.group_count = q.value("group_count").toInt();
        rec.face_count = q.value("face_count").toInt();

        rec.wa = q.value("wa").toInt();
        rec.w0 = q.value("w0").toInt();
        rec.p0 = q.value("p0").toInt();
        rec.pa = q.value("pa").toInt();
        rec.delta_t = q.value("delta_t").toInt();

        rec.span_formula = q.value("span_formula").toString();
        rec.is_non_reducible = q.value("is_non_reducible").toInt() != 0;

        rec.vertex_degree_distribution =
            json_to_map(q.value("vertex_degree_distribution_json").toString().toUtf8());
        rec.face_degree_distribution =
            json_to_map(q.value("face_degree_distribution_json").toString().toUtf8());

        out.push_back(rec);
    }

    return out;
}

bool Graph_Repository::load_graph_record(qint64 id, Graph_Record* out_record,
                                         QString* error_message) const
{
    if (!out_record) {
        if (error_message)
            *error_message = QStringLiteral("out_record is nullptr");
        return false;
    }

    if (!ensure_open(error_message))
        return false;

    QSqlDatabase db = repo_db(m_connection_name);
    QSqlQuery q(db);
    q.prepare(
        "SELECT "
        "  id, title, created_at, topology_hash, graph_json, "
        "  node_count, edge_count, group_count, face_count, "
        "  wa, w0, p0, pa, delta_t, span_formula, is_non_reducible, "
        "  vertex_degree_distribution_json, face_degree_distribution_json "
        "FROM diagrams WHERE id = :id;");
    q.bindValue(":id", id);

    if (!q.exec()) {
        if (error_message)
            *error_message = sql_error_text(q);
        return false;
    }

    if (!q.next()) {
        if (error_message)
            *error_message = QStringLiteral("graph not found");
        return false;
    }

    Graph_Record rec;
    rec.id = q.value("id").toLongLong();
    rec.title = q.value("title").toString();
    rec.created_at = q.value("created_at").toString();
    rec.topology_hash = q.value("topology_hash").toString();
    rec.graph_json = q.value("graph_json").toString().toUtf8();

    rec.node_count = q.value("node_count").toInt();
    rec.edge_count = q.value("edge_count").toInt();
    rec.group_count = q.value("group_count").toInt();
    rec.face_count = q.value("face_count").toInt();

    rec.wa = q.value("wa").toInt();
    rec.w0 = q.value("w0").toInt();
    rec.p0 = q.value("p0").toInt();
    rec.pa = q.value("pa").toInt();
    rec.delta_t = q.value("delta_t").toInt();

    rec.span_formula = q.value("span_formula").toString();
    rec.is_non_reducible = q.value("is_non_reducible").toInt() != 0;

    rec.vertex_degree_distribution =
        json_to_map(q.value("vertex_degree_distribution_json").toString().toUtf8());
    rec.face_degree_distribution =
        json_to_map(q.value("face_degree_distribution_json").toString().toUtf8());

    *out_record = rec;
    return true;
}

bool Graph_Repository::delete_graph(qint64 id, QString* error_message)
{
    if (!ensure_open(error_message))
        return false;

    QSqlDatabase db = repo_db(m_connection_name);
    QSqlQuery q(db);
    q.prepare("DELETE FROM diagrams WHERE id = :id;");
    q.bindValue(":id", id);

    if (!q.exec()) {
        if (error_message)
            *error_message = q.lastError().text();
        return false;
    }

    return true;
}
