#include "polygon_dialog.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QSpinBox>
#include <QUiLoader>

#include "knot_view.hpp"
#include "polygon_generator.hpp"
#include "resource_manager.hpp"

Polygon_Dialog::Polygon_Dialog(Knot_View* view, QWidget* parent)
    : QObject(parent), m_view(view), m_parent(parent), m_uiLoader(std::make_unique<QUiLoader>())
{
}

Polygon_Dialog::~Polygon_Dialog() = default;

void Polygon_Dialog::exec()
{
    if (!m_view) return;

    QFile file(QDir(resource_manager().program.data("plugins/polygon"))
                   .absoluteFilePath("dialog_insert_polygon.ui"));
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(m_parent, tr("Error"),
                              tr("Could not open plugin UI file: %1").arg(file.errorString()));
        return;
    }

    m_dialog.reset(qobject_cast<QDialog*>(m_uiLoader->load(&file, m_parent)));
    file.close();

    if (!m_dialog) {
        QMessageBox::critical(m_parent, tr("Error"), tr("Could not load plugin UI file."));
        return;
    }

    QSpinBox* spinSides = m_dialog->findChild<QSpinBox*>("spin_sides");
    QCheckBox* checkMiddleNode = m_dialog->findChild<QCheckBox*>("check_middle_node");

    if (!spinSides || !checkMiddleNode) {
        QMessageBox::critical(m_parent, tr("Error"), tr("Missing UI elements in plugin dialog."));
        return;
    }

    QDialogButtonBox* buttonBox = m_dialog->findChild<QDialogButtonBox*>("buttonBox");
    if (buttonBox) {
        connect(buttonBox, &QDialogButtonBox::accepted, m_dialog.get(), &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, m_dialog.get(), &QDialog::reject);
    }

    if (m_dialog->exec() == QDialog::Accepted) {
        int sides = spinSides->value();
        bool middleNode = checkMiddleNode->isChecked();
        double radius = (m_view->grid().is_enabled() ? m_view->grid().size() : 32.0) * 3.0;

        Graph polygonGraph =
            Polygon_Generator::create_polygon(sides, middleNode, radius, &m_view->grid());
        m_view->insert(polygonGraph, tr("Insert Polygon"));
    }
}
