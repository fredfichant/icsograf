/**
 * \file src/dialogs/main_window.hpp
 * \brief API declarations for the main window dialog or window.
 */

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QPrinter>
#include <QUndoGroup>
#include <memory>  // Added for std::unique_ptr

#include "export_image_dialog.hpp"
#include "knot_file_manager.hpp"
#include "ui_main_window.h"

// Forward declarations
class QDoubleSpinBox;
class Knot_View;
class Dock_Grid;
class QAction;

// Q_DECLARE_METATYPE(Color_Preview::Display_Mode)

class Main_Window : public QMainWindow, private Ui::Main_Window
{
    Q_OBJECT

   private:
    class StatusBar* m_statusBar;
    Knot_View* view;        ///< Active Knot_View (hopefully never NULL)
    QUndoGroup undo_group;  ///< Groups undo stacks
    std::unique_ptr<Export_Image_Dialog> dialog_export_image;
    Dock_Grid* dock_grid;                    ///< Grid conf dock
    class Dock_Properties* dock_properties;  ///< Properties dock
    class Dock_Symmetries* dock_symmetries;
    std::unique_ptr<KnotFileManager> m_fileManager;
    QAction* m_action_save_graph_library = nullptr;
    QAction* m_action_open_graph_library = nullptr;

    QPrinter printer;

   public:
    explicit Main_Window(QWidget* parent = 0);

    /**
     * @brief Get view at given tab index
     * @param n Tab index
     * @return The view or nullptr
     */
    Knot_View* view_at(int n);

    // Accessor for private members
    Knot_View* currentView() const;
    Export_Image_Dialog* exportDialog();
    QTabWidget* tabWidgetInstance();  // Note: QTabWidget is forward declared in ui_main_window.h
    QUndoGroup& undoGroup();

   public slots:
    /**
     * @brief Creates a new tab and optionally loads a file.
     * @param file File name, if empty an empty tab is created.
     * @return Whether the tab has been created successfully.
     */
    bool create_new_tab(QString file = QString());

    /**
     *  \brief Switch to the gien tab
     *  \param i tab index
     *  \pre i is a valid index
     */
    void switch_to_tab(int i);

    /**
     *  \brief Close the tab with the given index
     *  \param i Tab index
     *  \param confirm_if_changed Whether to show a dialog asking to save the file
     *  \pre i is a valid index
     *  \post The tab is closed. If it were the last tab, a new one is created
     */
    void close_tab(int i, bool confirm_if_changed = true);

    void print(QPrinter* pr);

   signals:
    /**
     * @brief Emitted whet a tab is about to be closed
     */
    void tab_closing(Knot_View*);

   protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent*) override;
    void closeEvent(QCloseEvent*) override;

   private:
    /// Initialize menus
    void init_menus();
    /// Initialize toolbars
    void init_toolbars();
    /// Initialize dock widgets
    void init_docks();
    /// Load saved configuration
    void load_config();

    /**
     *  \brief Ensure view is connected to the proper signals/slots
     */
    void connect_view(Knot_View* v);
    /**
     *  \brief Ensure view is disconnected connected from the relevant signals/slots
     */
    void disconnect_view(Knot_View* v);

    void connectActions();  // New method

   private slots:
    void set_icon_size(int);
    /**
     *  \brief Toggle tab icon to show whether the file has been modified
     */
    void set_clean_icon(bool clean);
    /**
     *  \brief Update title to current file and add a *star* if there are unsaved changes
     */
    void update_title();
    void set_undo_text(QString txt);
    void set_redo_text(QString txt);
    void set_clean_icon_for_tab(int index, bool isClean);
    /// Update style on undo/redo
    void update_style();
    void set_tool_button_style(Qt::ToolButtonStyle);

    /**
     *  \brief Update selected style dialog
     */
    void update_selection(QList<Node*> nodes, QList<Edge*> edges);
    void update_grid_icon(int shape);
    void update_recent_files();
    
    void click_recent_file();

    void handlePreferencesTriggered();
    void handleDisplayGraphToggled(bool arg1);
    void handleZoomInTriggered();
    void handleZoomOutTriggered();
    void handleResetZoomTriggered();
    void handleResetViewTriggered();
    void handleReportBugsTriggered();
    void handleManualTriggered();
    void handleRefreshPathTriggered();
    void handleOpenTriggered();
    void handleSaveTriggered();
    void handleSaveAsTriggered();
    void handleRotateToggled(bool checked);
    void handleScaleToggled(bool checked);
    void handlePrintTriggered();
    void handlePageSetupTriggered();
    void handlePrintPreviewTriggered();
    void handleAboutTriggered();
    void handleInsertPolygonTriggered();  // New slot
    void handleCloseTriggered();
    void handleCloseAllTriggered();
    void handleSaveAllTriggered();
    void handleSaveToLibraryTriggered();
    void handleOpenFromLibraryTriggered();
};

#endif  // MAIN_WINDOW_HPP
