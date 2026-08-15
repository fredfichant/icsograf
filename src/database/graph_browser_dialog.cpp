/**
 * \file src/database/graph_browser_dialog.cpp
 * \brief Construction and initialization of the graph browser dialog.
 */

#include "graph_browser_dialog.hpp"

#include <QMessageBox>

#include "edge.hpp"
#include "graph.hpp"
#include "node.hpp"

Graph_Browser_Dialog::Graph_Browser_Dialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("bibliothèque de graphes"));
    resize(1200, 700);

    QString error;
    if (!m_repo.open(&error) || !m_repo.migrate(&error)) {
        QMessageBox::critical(this,
                              QStringLiteral("base SQLite"),
                              QStringLiteral("impossible d'ouvrir la base :\n%1").arg(error));
    }

    build_ui();
    connect_signals();
    refresh_results();
}

Graph_Browser_Dialog::~Graph_Browser_Dialog() = default;
