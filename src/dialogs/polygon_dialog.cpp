/**
 * \file src/dialogs/polygon_dialog.cpp
 * \brief Implementation of polygon dialog dialog and window components.
 */

#include "polygon_dialog.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

#include "knot_view.hpp"
#include "polygon_generator.hpp"

Polygon_Dialog::Polygon_Dialog(Knot_View* view, QWidget* parent)
    : QObject(parent), m_view(view), m_parent(parent)
{}

Polygon_Dialog::~Polygon_Dialog() = default;

void Polygon_Dialog::exec()
{
    if (!m_view) return;

    QDialog dialog(m_parent);
    dialog.setWindowTitle(tr("Insert Polygon"));

    auto* layout = new QGridLayout(&dialog);

    auto* check_middle_node = new QCheckBox(tr("Node at Center"), &dialog);
    check_middle_node->setObjectName(QStringLiteral("check_middle_node"));
    check_middle_node->setToolTip(
        tr("Whether there should be a node connected to the vertices on the center of the polygon"));
    layout->addWidget(check_middle_node, 0, 0, 1, 2);

    auto* sides_label = new QLabel(tr("Sides"), &dialog);
    layout->addWidget(sides_label, 1, 0);

    auto* spin_sides = new QSpinBox(&dialog);
    spin_sides->setObjectName(QStringLiteral("spin_sides"));
    spin_sides->setRange(3, 32);
    spin_sides->setValue(5);
    layout->addWidget(spin_sides, 1, 1);

    auto* button_box =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    button_box->setObjectName(QStringLiteral("buttonBox"));
    connect(button_box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(button_box, 2, 0, 1, 2);

    if (dialog.exec() == QDialog::Accepted) {
        int sides = spin_sides->value();
        bool middleNode = check_middle_node->isChecked();
        double radius = (m_view->grid().is_enabled() ? m_view->grid().size() : 32.0) * 3.0;

        Graph polygonGraph =
            Polygon_Generator::create_polygon(sides, middleNode, radius, &m_view->grid());
        m_view->insert(polygonGraph, tr("Insert Polygon"));
    }
}
