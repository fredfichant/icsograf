#ifndef KNOT_FILE_MANAGER_HPP
#define KNOT_FILE_MANAGER_HPP

#include <QMainWindow>
#include <QObject>
#include <memory>

class Knot_View;  // Forward declaration

class KnotFileManager : public QObject
{
    Q_OBJECT
   public:
    explicit KnotFileManager(QMainWindow* mainWindow, QObject* parent = nullptr);

    // Placeholder for moved functions
    bool create_tab(QString file = QString());
    void save(bool force_select, int tab_index);
    bool check_close_all();

   signals:
    // Signals to communicate with MainWindow for UI updates
    void updateWindowTitle();
    void updateTabIcon(int index, bool isClean);
    void updateRecentFilesMenu();

   private:
    QMainWindow* m_mainWindow;
    // Add other members as needed
};

#endif  // KNOT_FILE_MANAGER_HPP