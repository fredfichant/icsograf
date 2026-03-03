#include "dialog_confirm_close.hpp"

#include <QCheckBox>

Dialog_Confirm_Close::Dialog_Confirm_Close(QWidget* parent) : QDialog(parent)
{
    setupUi(this);
    connect(&mapper, SIGNAL(mapped(int)), SLOT(file_toogled(int)));
}

void Dialog_Confirm_Close::add_file(int index, QString name)
{
    files[index] = name;
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    QCheckBox* cb = new QCheckBox(name);
    cb->setChecked(true);
    mapper.setMapping(cb, index);
    connect(cb, SIGNAL(toggled(bool)), &mapper, SLOT(map()));
    tableWidget->setCellWidget(row, 0, cb);
}

QList<int> Dialog_Confirm_Close::save_files()
{
    QList<int> r;
    for (int i : files.keys()) {
        if (files[i].save) r.push_back(i);
    }
    return r;
}

void Dialog_Confirm_Close::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            retranslateUi(this);
            break;
        default:
            break;
    }
}

void Dialog_Confirm_Close::file_toogled(int i)
{
    if (files.contains(i)) files[i].save = !files[i].save;
}

void Dialog_Confirm_Close::on_button_dont_save_clicked() { done(DontSave); }
