#include "knot_file_manager.hpp"

#include <QBuffer>
#include <QDrag>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>

#include "dialog_confirm_close.hpp"
#include "graph_validation.hpp"
#include "knot_view.hpp"         // For Knot_View::load_file, save_file
#include "main_window.hpp"       // For access to tabWidget and other MainWindow elements
#include "resource_manager.hpp"  // For settings, etc.

KnotFileManager::KnotFileManager(QMainWindow* mainWindow, QObject* parent)
    : QObject(parent), m_mainWindow(mainWindow)
{
    // Connect any signals from MainWindow to this manager if needed
}

bool KnotFileManager::create_tab(QString file)
{
    Main_Window* mw = qobject_cast<Main_Window*>(m_mainWindow);
    if (!mw) return false;

    bool error = false;
    // When view is not null and has no filename and no undo/redo actions
    // it means it's an empty new tab, so we can directly load into it.
    if (mw->currentView() && mw->currentView()->file_name().isEmpty() &&
        !mw->currentView()->undo_stack_pointer()->canUndo() &&
        !mw->currentView()->undo_stack_pointer()->canRedo()) {
        error = !mw->currentView()->load_file(file);
        if (!error) {
            mw->exportDialog()->set_view(mw->currentView());

            emit updateWindowTitle();

            mw->tabWidgetInstance()->setTabText(mw->tabWidgetInstance()->currentIndex(),
                                                mw->currentView()->file_name());
        }
    } else  // Create a new tab
    {
        if (mw->currentView() != nullptr) {
            // Extract style components from the current view's graph
            const QList<QColor>& colors = mw->currentView()->graph().colors();
            const Node_Style& node_style = mw->currentView()->graph().default_node_style();
            const Edge_Style& edge_style = mw->currentView()->graph().default_edge_style();

            // Call set_knot_style with the extracted components
            resource_manager().settings.set_knot_style(colors, node_style, edge_style);
        }

        Knot_View* v = new Knot_View();  // <--- Knot_View created here
        error = !v->load_file(file);
        int t = mw->tabWidgetInstance()->addTab(
            v, file.isEmpty() ? tr("New Knot") : file);  // <--- Added to tabWidget

        mw->undoGroup().addStack(v->undo_stack_pointer());

        // Always call switch_to_tab to ensure connect_view is called,
        // even if t is already the current index (e.g. the first tab)
        mw->switch_to_tab(t);

        v->set_background_color(Qt::white);
        v->set_join_style(Qt::RoundJoin);
        v->set_brush_style(Qt::SolidPattern);
        v->set_stroke_width(6);
        QList<QColor> default_colors;
        default_colors << Qt::black;
        v->set_knot_colors(default_colors);
    }

    if (error && !file.isEmpty()) {
        QMessageBox::warning(m_mainWindow, tr("File Error"),
                             tr("Error while reading \"%1\".").arg(file));
        return false;
    } else if (!error && !file.isEmpty()) {
        resource_manager().settings.add_recent_file(file);
        emit updateRecentFilesMenu();
    }

    mw->view_at(mw->tabWidgetInstance()->currentIndex())
        ->grid()
        .set_shape(resource_manager().settings.grid_shape());
    mw->view_at(mw->tabWidgetInstance()->currentIndex())
        ->grid()
        .set_size(resource_manager().settings.grid_size());
    mw->view_at(mw->tabWidgetInstance()->currentIndex())
        ->grid()
        .enable(resource_manager().settings.grid_enabled());

    return true;
}

void KnotFileManager::save(bool force_select, int tab_index)
{
    Main_Window* mw = qobject_cast<Main_Window*>(m_mainWindow);
    if (!mw) return;

    Knot_View* v = qobject_cast<Knot_View*>(mw->tabWidgetInstance()->widget(tab_index));

    if (!v) return;

    QString file = v->file_name();
    if (file.isEmpty() || force_select) {
        QString selected_filter;
        QString filters = tr("Knot files (*.knot);;XML files (*.xml);;All files (*)");
        file = QFileDialog::getSaveFileName(m_mainWindow, tr("Save Knot"), v->file_name(), filters,
                                            &selected_filter);

        QFileInfo finfo(file);
        if (!file.isEmpty() && finfo.suffix().isEmpty() && filters.startsWith(selected_filter))
            file += ".knot";
    }
    if (!file.isEmpty()) {
        const Graph_Validation_Result validation = validate_graph(v->graph());
        if (!validation.valid) {
            QMessageBox::warning(m_mainWindow, tr("Invalid Graph"),
                                 tr("Cannot save the graph because it is not valid:\n- %1")
                                     .arg(validation.reason));
            return;
        }

        if (v->save_file(file)) {
            emit updateWindowTitle();
            mw->tabWidgetInstance()->setTabText(tab_index, v->windowFilePath());
            emit updateTabIcon(tab_index, true);  // Signal MainWindow to update tab icon

            resource_manager().settings.add_recent_file(file);
            emit updateRecentFilesMenu();  // Signal MainWindow to update its menu
        } else {
            QMessageBox::warning(m_mainWindow, tr("File Error"),
                                 tr("Failed to save file \"%1\".").arg(file));
        }
    }
}

bool KnotFileManager::check_close_all()
{
    Main_Window* mw = qobject_cast<Main_Window*>(m_mainWindow);
    if (!mw) return false;

    if (resource_manager().settings.check_unsaved_files()) {
        Dialog_Confirm_Close dialog;

        for (int i = 0; i < mw->tabWidgetInstance()->count(); i++) {
            Knot_View* kv = dynamic_cast<Knot_View*>(mw->tabWidgetInstance()->widget(i));
            if (kv && !kv->undo_stack_pointer()->isClean())
                dialog.add_file(i, mw->tabWidgetInstance()->tabText(i));
        }

        if (dialog.has_files()) {
            int r = dialog.exec();
            if (r == Dialog_Confirm_Close::Rejected)
                return false;  // Don't close
            else if (r == Dialog_Confirm_Close::Accepted) {
                for (int i : dialog.save_files()) {
                    save(false, i);  // Use KnotFileManager's save method
                }
            }
        }
    }
    return true;
}
