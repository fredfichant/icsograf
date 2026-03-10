#include "graph_save_dialog.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "graph.hpp"
#include "graph_properties.hpp"

Graph_Save_Dialog::Graph_Save_Dialog(const Graph& graph, QWidget* parent)
    : QDialog(parent),
      m_graph(graph)
{
    setWindowTitle(QStringLiteral("Enregistrer dans la bibliothèque"));
    resize(480, 180);

    QString error;
    if (!m_repo.open(&error) || !m_repo.migrate(&error)) {
        QMessageBox::critical(this,
                              QStringLiteral("Base SQLite"),
                              QStringLiteral("Impossible d'ouvrir la base :\n%1").arg(error));
    }

    build_ui();
}

void Graph_Save_Dialog::build_ui()
{
    auto* root = new QVBoxLayout(this);

    const Graph_Properties* props = m_graph.properties();
    QString summary = props ? props->summary_text()
                            : QStringLiteral("Propriétés indisponibles");

    m_info_label = new QLabel(summary, this);
    m_info_label->setWordWrap(true);

    m_title_edit = new QLineEdit(this);
    m_title_edit->setPlaceholderText(QStringLiteral("Titre du graphe"));

    auto* buttons = new QHBoxLayout();
    m_save_button = new QPushButton(QStringLiteral("Enregistrer"), this);
    m_cancel_button = new QPushButton(QStringLiteral("Annuler"), this);

    buttons->addStretch(1);
    buttons->addWidget(m_save_button);
    buttons->addWidget(m_cancel_button);

    root->addWidget(new QLabel(QStringLiteral("Titre :"), this));
    root->addWidget(m_title_edit);
    root->addWidget(m_info_label);
    root->addStretch(1);
    root->addLayout(buttons);

    connect(m_save_button, &QPushButton::clicked,
            this, &Graph_Save_Dialog::on_save_clicked);
    connect(m_cancel_button, &QPushButton::clicked,
            this, &QDialog::reject);
}

void Graph_Save_Dialog::on_save_clicked()
{
    m_result = m_repo.save_graph(m_graph, m_title_edit->text().trimmed());

    if (!m_result.ok) {
        QMessageBox::warning(this,
                             QStringLiteral("Enregistrement"),
                             QStringLiteral("Échec de l'enregistrement :\n%1").arg(m_result.error));
        return;
    }

    if (m_result.inserted) {
        QMessageBox::information(this,
                                 QStringLiteral("Enregistrement"),
                                 QStringLiteral("Graphe enregistré.\nID = %1").arg(m_result.diagram_id));
    } else {
        QMessageBox::information(this,
                                 QStringLiteral("Enregistrement"),
                                 QStringLiteral("Doublon détecté.\nGraphe déjà présent avec l'ID %1")
                                     .arg(m_result.diagram_id));
    }

    accept();
}
