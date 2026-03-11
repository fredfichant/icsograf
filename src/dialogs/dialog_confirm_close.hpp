/**
 * \file src/dialogs/dialog_confirm_close.hpp
 * \brief API declarations for the dialog confirm close dialog or window.
 */

#ifndef DIALOG_CONFIRM_CLOSE_HPP
#define DIALOG_CONFIRM_CLOSE_HPP

#include <QSignalMapper>

#include "ui_dialog_confirm_close.h"

class Dialog_Confirm_Close : public QDialog, private Ui::Dialog_Confirm_Close
{
    Q_OBJECT

    /**
     * \brief Simple helper class
     */
    struct Save_File
    {
        QString file_name;
        bool save;
        Save_File(QString file_name = "") : file_name(file_name), save(true) {}
    };

    QMap<int, Save_File> files;
    QSignalMapper mapper;

   public:
    static const int DontSave = Accepted + 1;

    explicit Dialog_Confirm_Close(QWidget* parent = 0);

    /**
     * \brief Insert file to be saved
     * \param index  An identifier used by the caller to associate to the file
     * \param name   File name
     * \post  has_file() == true
     * \note  \c index should be unique, for a single file
     */
    void add_file(int index, QString name);

    bool has_files() const { return !files.empty(); }

    /**
     * \brief Get files to be saved
     * \return A list of indices representing the files that need to be saved
     */
    QList<int> save_files();

   protected:
    void changeEvent(QEvent* e);

   private slots:
    void file_toogled(int i);
    void on_button_dont_save_clicked();
};

#endif  // DIALOG_CONFIRM_CLOSE_HPP
