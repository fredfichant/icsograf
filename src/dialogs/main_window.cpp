/**
 * \file src/dialogs/main_window.cpp
 * \brief Implementation of main window dialog and window components.
 */

#include "main_window.hpp"

#include <QBuffer>
#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>  // Added
#include <QDockWidget>
#include <QDrag>
#include <QFileDialog>
#include <QMessageBox>  // Added
#include <QMouseEvent>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrintPreviewDialog>  // Corrected include
#include <QUrl>
#include <memory>

#include "dialog_preferences.hpp"
#include "icon_dock_style.hpp"
#include "polygon_dialog.hpp"
#include "resource_manager.hpp"
#include "status_bar.hpp"
#include "xml_exporter.hpp"
#include "database/graph_browser_dialog.hpp"
#include "database/graph_save_dialog.hpp"
// #include <QUiLoader> // Removed
#include <QCheckBox>  // Added
#include <QSpinBox>   // Added

#include "icsograf_info.hpp"

// Newly added includes for forward-declared classes
#include <QDoubleSpinBox>

#include "dock/dock_properties.hpp"
#include "dock/dock_symmetries.hpp"
#include "dock_grid.hpp"
#include "commands.hpp"
#include "knot_view.hpp"

Main_Window::Main_Window(QWidget* parent)
    : QMainWindow(parent),
      m_statusBar(nullptr),
      view(nullptr),
      dock_properties(nullptr),
      dock_symmetries(nullptr),
      m_fileManager(std::make_unique<KnotFileManager>(this, this))  // Initialize here
{
    setupUi(this);
    setStatusBar(m_statusBar = new StatusBar(this));
    setWindowIcon(QIcon(resource_manager().program.data("img/icon.svg")));

    setWindowTitle(resource_manager().program.name());

    dialog_export_image.reset(new Export_Image_Dialog(this));  // Corrected initialization

    init_docks();

    m_fileManager->create_tab();  // Re-add this line to create initial tab

    init_menus();
    connectActions();  // Added call to new method

    load_config();

    // init_toolbars must come load_config in order to configure properly user-defined toolbars
    init_toolbars();

    // Scripting connects removed

    // Connect KnotFileManager signals to MainWindow slots
    connect(m_fileManager.get(), &KnotFileManager::updateWindowTitle, this,
            &Main_Window::update_title);
    connect(m_fileManager.get(), &KnotFileManager::updateTabIcon, this,
            &Main_Window::set_clean_icon_for_tab);  // New slot needed
    connect(m_fileManager.get(), &KnotFileManager::updateRecentFilesMenu, this,
            &Main_Window::update_recent_files);
}

void Main_Window::connectActions()
{
    // Connections moved from init_menus()
    connect(action_Export, &QAction::triggered, dialog_export_image.get(),
            &Export_Image_Dialog::show);
    connect(action_About, &QAction::triggered, this, &Main_Window::handleAboutTriggered);

    // Other action connections
    connect(action_Preferences, &QAction::triggered, this,
            &Main_Window::handlePreferencesTriggered);
    connect(action_Display_Graph, &QAction::toggled, this, &Main_Window::handleDisplayGraphToggled);
    connect(action_Zoom_In, &QAction::triggered, this, &Main_Window::handleZoomInTriggered);
    connect(action_Zoom_Out, &QAction::triggered, this, &Main_Window::handleZoomOutTriggered);
    connect(action_Reset_Zoom, &QAction::triggered, this, &Main_Window::handleResetZoomTriggered);
    connect(action_Reset_View, &QAction::triggered, this, &Main_Window::handleResetViewTriggered);
    connect(action_Report_Bugs, &QAction::triggered, this, &Main_Window::handleReportBugsTriggered);
    connect(action_Manual, &QAction::triggered, this, &Main_Window::handleManualTriggered);
    connect(action_Refresh_Path, &QAction::triggered, this,
            &Main_Window::handleRefreshPathTriggered);
    connect(action_Open, &QAction::triggered, this, &Main_Window::handleOpenTriggered);
    connect(action_New, &QAction::triggered, this, [this]() { create_new_tab(); });
    connect(action_Save, &QAction::triggered, this, &Main_Window::handleSaveTriggered);
    connect(action_Save_As, &QAction::triggered, this, &Main_Window::handleSaveAsTriggered);
    connect(action_Mirror_Horizontal, &QAction::triggered, this, [this]() {
        if (view) view->flip_horiz_selection();
    });
    connect(action_Mirror_Vertical, &QAction::triggered, this, [this]() {
        if (view) view->flip_vert_selection();
    });
    connect(action_Select_All, &QAction::triggered, this, [this]() {
        if (view) view->select_all();
    });
    connect(actionSelect_Connected, &QAction::triggered, this, [this]() {
        if (view) view->select_connected();
    });
    connect(action_Copy, &QAction::triggered, this, [this]() {
        if (view) view->copy_selection();
    });
    connect(action_Paste, &QAction::triggered, this, [this]() {
        if (view) view->paste_selection();
    });
    connect(action_Cut, &QAction::triggered, this, [this]() {
        if (view) view->cut_selection();
    });
    connect(action_Snap_to_Grid, &QAction::triggered, this, [this]() {
        if (view) view->snap_selection_to_grid();
    });
    connect(action_Erase, &QAction::triggered, this, [this]() {
        if (view) view->erase_selection();
    });
    connect(action_Close, &QAction::triggered, this, &Main_Window::handleCloseTriggered);
    connect(action_Close_All, &QAction::triggered, this, &Main_Window::handleCloseAllTriggered);
    connect(action_Save_All, &QAction::triggered, this, &Main_Window::handleSaveAllTriggered);
    connect(action_Connect, &QAction::triggered, this, [this]() {
        if (view) view->connect_selection();
    });
    connect(action_Disconnect, &QAction::triggered, this, [this]() {
        if (view) view->disconnect_selection();
    });
    connect(action_Merge, &QAction::triggered, this, [this]() {
        if (view) view->merge_selection();
    });
    connect(action_Rotate, &QAction::toggled, this, &Main_Window::handleRotateToggled);
    connect(action_Scale, &QAction::toggled, this, &Main_Window::handleScaleToggled);
    connect(action_Print, &QAction::triggered, this, &Main_Window::handlePrintTriggered);
    connect(action_Page_Setup, &QAction::triggered, this, &Main_Window::handlePageSetupTriggered);
    connect(action_Print_Preview, &QAction::triggered, this,
            &Main_Window::handlePrintPreviewTriggered);
    connect(action_Insert_Polygon, &QAction::triggered, this,
            &Main_Window::handleInsertPolygonTriggered);  // New connection
    if (m_action_save_graph_library) {
        connect(m_action_save_graph_library, &QAction::triggered, this,
                &Main_Window::handleSaveToLibraryTriggered);
    }
    if (m_action_open_graph_library) {
        connect(m_action_open_graph_library, &QAction::triggered, this,
                &Main_Window::handleOpenFromLibraryTriggered);
    }
}

void Main_Window::init_menus()
{
    // Menu File
    action_New->setShortcut(QKeySequence::New);
    action_Open->setShortcut(QKeySequence::Open);
    action_Save->setShortcut(QKeySequence::Save);
    action_Save_As->setShortcut(QKeySequence::SaveAs);
    action_Close->setShortcut(QKeySequence::Close);
    action_Print->setShortcut(QKeySequence::Print);
    action_Exit->setShortcut(QKeySequence::Quit);
    update_recent_files();

    if (!m_action_save_graph_library) {
        m_action_save_graph_library =
            new QAction(QIcon::fromTheme("document-save"), tr("Save Graph to Library"), this);
        m_action_save_graph_library->setObjectName("action_Save_Graph_To_Library");
    }
    if (!m_action_open_graph_library) {
        m_action_open_graph_library =
            new QAction(QIcon::fromTheme("repository"), tr("Open Graph from Library"), this);
        m_action_open_graph_library->setObjectName("action_Open_Graph_From_Library");
    }
    if (!menu_File->actions().contains(m_action_save_graph_library)) {
        menu_File->insertAction(action_Export, m_action_save_graph_library);
        menu_File->insertAction(action_Export, m_action_open_graph_library);
        menu_File->insertSeparator(action_Export);
    }
    if (toolbar_main && !toolbar_main->actions().contains(m_action_save_graph_library)) {
        toolbar_main->addSeparator();
        toolbar_main->addAction(m_action_save_graph_library);
        toolbar_main->addAction(m_action_open_graph_library);
    }

    // Menu Edit
    action_Undo->setShortcut(QKeySequence::Undo);
    action_Redo->setShortcut(QKeySequence::Redo);
    action_Cut->setShortcut(QKeySequence::Cut);
    action_Copy->setShortcut(QKeySequence::Copy);
    action_Paste->setShortcut(QKeySequence::Paste);
    action_Select_All->setShortcut(QKeySequence::SelectAll);
    action_Preferences->setShortcut(QKeySequence::Preferences);

    // Menu View

    action_Zoom_In->setShortcut(QKeySequence::ZoomIn);
    action_Zoom_Out->setShortcut(QKeySequence::ZoomOut);

    // Menu Nodes
    QActionGroup* transform_mode = new QActionGroup(this);
    transform_mode->addAction(action_Scale);
    transform_mode->addAction(action_Rotate);

    QActionGroup* edit_mode = new QActionGroup(this);
    edit_mode->addAction(action_Edit_Graph);
    edit_mode->addAction(action_Edge_Loop);
    edit_mode->addAction(action_Toggle_Edges);

    // Menu Tools
    action_Refresh_Path->setShortcut(QKeySequence::Refresh);

    // Menu Help
    action_Manual->setShortcut(QKeySequence::HelpContents);
}

void Main_Window::init_toolbars()
{
    for (const QToolBar* tb : findChildren<QToolBar*>())
        menu_Toolbars->insertAction(0, tb->toggleViewAction());
}

void Main_Window::init_docks()
{
    // Grid config
    dock_grid = new Dock_Grid(this);
    addDockWidget(Qt::RightDockWidgetArea, dock_grid);
    menu_Grid->insertAction(0, dock_grid->toggleViewAction());

    // Graph Properties
    dock_properties = new Dock_Properties(view ? view->graph().properties() : nullptr, this);
    addDockWidget(Qt::RightDockWidgetArea, dock_properties);
    tabifyDockWidget(dock_grid, dock_properties);

    dock_symmetries = new Dock_Symmetries(this);
    addDockWidget(Qt::RightDockWidgetArea, dock_symmetries);
    tabifyDockWidget(dock_properties, dock_symmetries);

    connect(&undo_group, SIGNAL(cleanChanged(bool)), SLOT(set_clean_icon(bool)));
    connect(&undo_group, SIGNAL(undoTextChanged(QString)), SLOT(set_undo_text(QString)));
    connect(&undo_group, SIGNAL(redoTextChanged(QString)), SLOT(set_redo_text(QString)));
    connect(&undo_group, SIGNAL(canUndoChanged(bool)), action_Undo, SLOT(setEnabled(bool)));
    connect(action_Undo, SIGNAL(triggered()), &undo_group, SLOT(undo()));
    connect(action_Redo, SIGNAL(triggered()), &undo_group, SLOT(redo()));

    connect(action_Undo, SIGNAL(triggered()), SLOT(update_style()));
    connect(action_Redo, SIGNAL(triggered()), SLOT(update_style()));

    // Menu entries
    for (QDockWidget* dw : findChildren<QDockWidget*>()) {
        QAction* a = dw->toggleViewAction();
        a->setIcon(dw->windowIcon());
        menu_Docks->insertAction(0, a);
        dw->setStyle(new Icon_Dock_Style(dw));
    }

    dock_properties->setWindowTitle(tr("Graph Properties"));
}

void Main_Window::load_config()
{
    connect(&resource_manager().settings, SIGNAL(icon_size_changed(int)), this,
            SLOT(set_icon_size(int)));
    connect(&resource_manager().settings, SIGNAL(tool_button_style_changed(Qt::ToolButtonStyle)),
            this, SLOT(set_tool_button_style(Qt::ToolButtonStyle)));

    if (!resource_manager().settings.least_version(0, 9)) {
        qWarning() << tr("Warning:") << tr("Discarding old configuration");
        return;
    }
    if (!resource_manager().settings.current_version()) {
        int load_old =
            QMessageBox::question(this, tr("Load old configuration"),
                                  tr("icsograf has detected configuration for version %1,\n"
                                     "this is version %2.\n"
                                     "Do you want to load it anyways?")
                                      .arg(resource_manager().settings.version())
                                      .arg(resource_manager().program.version()),
                                  QMessageBox::Yes, QMessageBox::No);
        if (load_old != QMessageBox::Yes) return;
    }
    resource_manager().settings.initialize_window(this);
}

void Main_Window::connect_view(Knot_View* v)
{
    // qDebug() << "Main_Window: Connecting view" << v;
    //  set current
    view = v;
    if (!v) return;

    // statusbar
    m_statusBar->connectView(v);

    // zoom/view
    connect(action_Fit_View, SIGNAL(triggered()), v, SLOT(view_fit()));

    // edit mode
    connect(action_Edit_Graph, SIGNAL(triggered()), v, SLOT(set_mode_edit_graph()),
            Qt::UniqueConnection);
    connect(action_Edge_Loop, SIGNAL(triggered()), v, SLOT(set_mode_edge_chain()),
            Qt::UniqueConnection);
    connect(action_Toggle_Edges, SIGNAL(triggered()), v, SLOT(set_mode_toggle_edges()),
            Qt::UniqueConnection);
    action_Edit_Graph->setChecked(v->edit_graph_mode_enabled());
    action_Edge_Loop->setChecked(v->edge_loop_mode_enabled());
    action_Toggle_Edges->setChecked(v->toggle_edges_mode_enabled());

    // undo/redo
    v->undo_stack_pointer()->setActive(true);
    undo_group.setActiveStack(v->undo_stack_pointer());

    // grid editor
    dock_grid->set_grid(&v->grid());
    connect(dock_grid, SIGNAL(move_grid()), v, SLOT(set_mode_move_grid()));
    action_Enable_Grid->setChecked(v->grid().is_enabled());
    connect(action_Enable_Grid, SIGNAL(triggered(bool)), &v->grid(), SLOT(enable(bool)));
    connect(&v->grid(), SIGNAL(enabled(bool)), action_Enable_Grid, SLOT(setChecked(bool)));
    connect(&v->grid(), SIGNAL(shape_changed(int)), SLOT(update_grid_icon(int)));

    connect(v, SIGNAL(selection_changed(QList<Node*>, QList<Edge*>)), this,
            SLOT(update_selection(QList<Node*>, QList<Edge*>)));

    // graph properties dock
    dock_properties->set_properties(*v->graph().properties());
    dock_symmetries->set_graph(v->graph());
    connect(v, &Knot_View::graph_changed, dock_symmetries, &Dock_Symmetries::refresh,
            Qt::UniqueConnection);

    // export
    dialog_export_image->set_view(v);

    // modes
    v->set_display_graph(action_Display_Graph->isChecked());
    action_Rotate->setChecked(v->transform_mode() == Transform_Handle::ROTATE);
    action_Scale->setChecked(v->transform_mode() == Transform_Handle::SCALE);

    // Performance
    v->set_fluid_refresh(resource_manager().settings.fluid_refresh());
    v->enable_cache(resource_manager().settings.graph_cache());
    v->set_antialiasing(resource_manager().settings.antialiasing());
}

void Main_Window::update_style()
{
    if (view) {
        update_selection(view->selected_nodes(), view->selected_edges());
    }
}

void Main_Window::disconnect_view(Knot_View* v)
{
    if (v != nullptr) {
        disconnect(v);

        m_statusBar->disconnectView(v);

        disconnect(action_Fit_View, SIGNAL(triggered()), v, SLOT(view_fit()));

        dock_grid->disconnect(v);
        dock_grid->unset_grid(&v->grid());
        v->grid().disconnect(this);
        v->grid().disconnect(action_Enable_Grid);
        action_Enable_Grid->disconnect(&v->grid());

        update_selection(QList<Node*>(), QList<Edge*>());

        disconnect(v, SIGNAL(selection_changed(QList<Node*>, QList<Edge*>)), this,
                   SLOT(update_selection(QList<Node*>, QList<Edge*>)));
    }
}

void Main_Window::set_icon_size(int sz) { setIconSize(QSize(sz, sz)); }

void Main_Window::set_clean_icon(bool clean)
{
    tabWidget->setTabIcon(tabWidget->currentIndex(),
                          clean ? QIcon() : QIcon::fromTheme("document-save"));
    update_title();
}

void Main_Window::update_title()
{
    if (!view) {
        setWindowTitle(resource_manager().program.name());
        return;
    }

    bool clean = view->undo_stack_pointer()->isClean();
    QString filename = view->windowFilePath();
    if (filename.isEmpty()) filename = tr("New Knot");

    /*: Main window title
     *  %1 is the program name
     *  %2 is the file name
     *  %3 is a star * or an empty string depending on whether the file was modified
     */
    setWindowTitle(
        tr("%1 - %2%3").arg(resource_manager().program.name()).arg(filename).arg(clean ? "" : "*"));
}

void Main_Window::set_tool_button_style(Qt::ToolButtonStyle tbs) { setToolButtonStyle(tbs); }

void Main_Window::update_selection(QList<Node*> nodes, QList<Edge*> edges)
{
    Q_UNUSED(nodes);
    Q_UNUSED(edges);
}

void Main_Window::handlePreferencesTriggered()
{
    Dialog_Preferences(this).exec();
    view->set_fluid_refresh(resource_manager().settings.fluid_refresh());
    view->enable_cache(resource_manager().settings.graph_cache());
    view->set_antialiasing(resource_manager().settings.antialiasing());
    update_recent_files();
}

void Main_Window::switch_to_tab(int i)
{
    // qDebug() << "Main_Window: Switching to tab index" << i;
    tabWidget->setCurrentIndex(i);
    /*setWindowTitle(tr("%1 - %2").arg(resource_manager().program.name())
                   .arg(tabWidget->tabText(i)));*/
    disconnect_view(view);
    Knot_View* current_widget = dynamic_cast<Knot_View*>(tabWidget->currentWidget());
    // qDebug() << "Main_Window: Current widget after switch is" << current_widget;
    connect_view(current_widget);
    update_title();
}

void Main_Window::close_tab(int i, bool confirm_if_changed)
{
    Knot_View* kv = dynamic_cast<Knot_View*>(tabWidget->widget(i));
    if (kv) {
        if (resource_manager().settings.check_unsaved_files() && confirm_if_changed &&
            !kv->undo_stack_pointer()->isClean()) {
            int r = QMessageBox::question(
                this, tr("Close File"),
                tr("The file \"%1\" has been modified.\nDo you want to save changes?")
                    .arg(tabWidget->tabText(i)),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if (r == QMessageBox::Yes)
                m_fileManager->save(false, i);
            else if (r == QMessageBox::Cancel)
                return;
        }

        if (kv == view) {
            disconnect_view(kv);
            dock_symmetries->clear_graph();
            view = nullptr;
        }

        undo_group.removeStack(kv->undo_stack_pointer());

        emit tab_closing(kv);

        delete kv;
    }
    // tabWidget->removeTab(i);
    if (tabWidget->count() == 0) m_fileManager->create_tab();
}

void Main_Window::set_undo_text(QString txt) { action_Undo->setText(tr("Undo %1").arg(txt)); }

void Main_Window::set_redo_text(QString txt) { action_Redo->setText(tr("Redo %1").arg(txt)); }

void Main_Window::handleZoomInTriggered() { view->zoom_view(1.25); }

void Main_Window::handleZoomOutTriggered() { view->zoom_view(0.8); }

void Main_Window::handleResetZoomTriggered() { view->set_zoom(1); }

void Main_Window::handleResetViewTriggered() { view->reset_view(); }

void Main_Window::handleReportBugsTriggered() { QDesktopServices::openUrl(QUrl(BUG_URL)); }

void Main_Window::handleManualTriggered()
{
    const QString manual_path = resource_manager().program.data("help/manual.html");
    if (manual_path.isEmpty()) {
        QMessageBox::warning(this, tr("Manual unavailable"),
                             tr("The local user manual could not be found in the application "
                                "data directories."));
        return;
    }

    const QUrl manual_url = QUrl::fromLocalFile(manual_path);
    if (!QDesktopServices::openUrl(manual_url)) {
        QMessageBox::warning(this, tr("Manual unavailable"),
                             tr("Unable to open the local user manual:\n%1")
                                 .arg(QDir::toNativeSeparators(manual_path)));
    }
}

void Main_Window::handleRefreshPathTriggered() { view->update_knot(); }

void Main_Window::handleOpenTriggered()
{

    QString initialPath = QDir::homePath();  // Always start in a known good path

    QStringList files =
        QFileDialog::getOpenFileNames(this, tr("Open Knot"), initialPath,
        "Knot files (*.knot);;XML files (*.xml);;All files (*)");

    for (QString file : files) {
        m_fileManager->create_tab(file);
    }
}

void Main_Window::handleSaveTriggered() { m_fileManager->save(false, tabWidget->currentIndex()); }

void Main_Window::handleSaveAsTriggered() { m_fileManager->save(true, tabWidget->currentIndex()); }

void Main_Window::update_grid_icon(int shape)
{
    QAction* act = findChild<QAction*>("action_Enable_Grid");
    if (!act) return;
    if (shape == Snapping_Grid::TRIANGLE1)
        act->setIcon(QIcon::fromTheme("grid-triangle-h"));
    else if (shape == Snapping_Grid::TRIANGLE2)
        act->setIcon(QIcon::fromTheme("grid-triangle-v"));
    else
        act->setIcon(QIcon::fromTheme("grid-square"));
}

void Main_Window::update_recent_files()
{
    menu_Open_Recent->clear();

    if (resource_manager().settings.recent_files().empty())
        menu_Open_Recent->addAction(tr("No recent files"))->setEnabled(false);
    else {
        for (QString savefile : resource_manager().settings.recent_files()) {
            QAction* a = menu_Open_Recent->addAction(
                QIcon(resource_manager().program.data("img/icon.svg")), savefile);
            connect(a, SIGNAL(triggered()), this, SLOT(click_recent_file()));
        }
    }
}

void Main_Window::click_recent_file()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) m_fileManager->create_tab(action->text());
}

void Main_Window::handleDisplayGraphToggled(bool checked)
{
    action_Display_Graph->setIcon(QIcon::fromTheme(checked ? "knot-graph-on" : "knot-graph-off"));
    view->set_display_graph(checked);
}

void Main_Window::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list") ||
        event->mimeData()->hasFormat("application/x-icsograf"))
        event->acceptProposedAction();
}

void Main_Window::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        for (QUrl url : event->mimeData()->urls()) {
#if HAS_QT_4_8
            if (url.isLocalFile())
#endif
            {
                m_fileManager->create_tab(url.toLocalFile());
            }
        }
    }
    if (event->mimeData()->hasFormat("application/x-icsograf")) {
        m_fileManager->create_tab();

        QByteArray clip_data = event->mimeData()->data("application/x-icsograf");
        QBuffer read_data(&clip_data);
        view->load_file(read_data, tr("Drop"));
    }
}

void Main_Window::closeEvent(QCloseEvent* ev)
{
    // Capture the current style components BEFORE potential destruction of 'view'
    QList<QColor> current_colors;
    Node_Style current_node_style;
    Edge_Style current_edge_style;
    bool has_active_view = false;

    if (view) {
        current_colors = view->graph().colors();
        current_node_style = view->graph().default_node_style();
        current_edge_style = view->graph().default_edge_style();
        has_active_view = true;
    }

    if (!m_fileManager->check_close_all()) {
        ev->ignore();
        return;
    }

    resource_manager().settings.save_window(this);

    // Now use the captured style components (if a view was active)
    if (has_active_view) {
        resource_manager().settings.set_knot_style(current_colors, current_node_style,
                                                   current_edge_style);
    }
    QMainWindow::closeEvent(ev);
}

void Main_Window::handleCloseTriggered() { close_tab(tabWidget->currentIndex(), true); }

void Main_Window::handleCloseAllTriggered()
{
    if (m_fileManager->check_close_all())
        for (int i = 0, c = tabWidget->count(); i < c; i++) close_tab(0, false);
}

void Main_Window::handleSaveAllTriggered()
{
    for (int i = 0; i < tabWidget->count(); i++) m_fileManager->save(false, i);
}

void Main_Window::handleRotateToggled(bool checked)
{
    if (checked) view->set_transform_mode(Transform_Handle::ROTATE);
}

void Main_Window::handleScaleToggled(bool checked)
{
    if (checked) view->set_transform_mode(Transform_Handle::SCALE);
}

void Main_Window::print(QPrinter* pr)
{
    if (view) view->print(pr);
}

void Main_Window::handlePrintTriggered()
{
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        print(&printer);
    }
}

void Main_Window::handlePageSetupTriggered() { QPageSetupDialog(&printer, this).exec(); }

void Main_Window::handlePrintPreviewTriggered()
{
    QPrintPreviewDialog dialog(&printer, this);
    connect(&dialog, SIGNAL(paintRequested(QPrinter*)), SLOT(print(QPrinter*)));
    dialog.exec();
}

Knot_View* Main_Window::view_at(int n) { return dynamic_cast<Knot_View*>(tabWidget->widget(n)); }

bool Main_Window::create_new_tab(QString file) { return m_fileManager->create_tab(file); }

void Main_Window::set_clean_icon_for_tab(int index, bool isClean)
{
    tabWidget->setTabIcon(index, isClean ? QIcon() : QIcon::fromTheme("document-save"));
}

Knot_View* Main_Window::currentView() const { return view; }
Export_Image_Dialog* Main_Window::exportDialog() { return dialog_export_image.get(); }
QTabWidget* Main_Window::tabWidgetInstance() { return tabWidget; }
QUndoGroup& Main_Window::undoGroup() { return undo_group; }
void Main_Window::handleAboutTriggered()
{
    QMessageBox::about(this, tr("About %1").arg(resource_manager().program.name()),
                       tr("<b>%1 %2</b><br/>%3")
                           .arg(resource_manager().program.name())
                           .arg(resource_manager().program.version())
                           .arg(PROGRAM_DESCRIPTION));
}

void Main_Window::handleInsertPolygonTriggered() { Polygon_Dialog(view, this).exec(); }

void Main_Window::handleSaveToLibraryTriggered()
{
    if (!view) {
        QMessageBox::warning(this, tr("Graph Library"), tr("No active graph to save."));
        return;
    }

    Graph_Save_Dialog dialog(view->graph(), this);
    dialog.exec();
}

void Main_Window::handleOpenFromLibraryTriggered()
{
    if (!view) {
        QMessageBox::warning(this, tr("Graph Library"), tr("No active view to load into."));
        return;
    }

    Graph_Browser_Dialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    Graph loaded;
    QList<Node*> loaded_nodes;
    QList<Edge*> loaded_edges;
    QString error_message;
    if (!dialog.load_selected_graph(loaded, loaded_nodes, loaded_edges, &error_message)) {
        QMessageBox::warning(this, tr("Graph Library"),
                             tr("Failed to load the selected graph:\n%1").arg(error_message));
        return;
    }

    view->begin_macro(tr("Load Graph From Library"));

    for (Node* n : view->graph().nodes()) {
        view->push_command(new Remove_Node(n, view));
    }
    for (Edge* e : view->graph().edges()) {
        view->push_command(new Remove_Edge(e, view));
    }

    for (Node* n : loaded_nodes) {
        view->push_command(new Create_Node(n, view));
    }
    for (Edge* e : loaded_edges) {
        view->push_command(new Create_Edge(e, view));
    }

    view->end_macro();
    view->undo_stack_pointer()->setClean();
    view->set_file_name(QString());
    view->setWindowFilePath(QString());
    tabWidget->setTabText(tabWidget->currentIndex(), tr("Library Graph"));
    set_clean_icon_for_tab(tabWidget->currentIndex(), true);
    update_title();
}
