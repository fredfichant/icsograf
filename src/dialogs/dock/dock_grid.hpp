/**
 * \file src/dialogs/dock/dock_grid.hpp
 * \brief API declarations for the dock grid dock widget.
 */

#ifndef DOCK_GRID_HPP
#define DOCK_GRID_HPP

#include <QDockWidget>

#include "snapping_grid.hpp"
#include "ui_dock_grid.h"

class Dock_Grid : public QDockWidget, private Ui::Dock_Grid
{
    Q_OBJECT

   private:
    Snapping_Grid* target;

   public:
    explicit Dock_Grid(QWidget* parent = 0);

    void set_grid(Snapping_Grid* target_grid);

    /// if grid is target grid, clear target grid
    void unset_grid(Snapping_Grid* grid);

   protected:
    void changeEvent(QEvent* e);

   signals:
    /// Emitted when the user want to move the grid with the mouse
    void move_grid();

   private slots:
    void position_spin_changed();
    void grid_moved(QPointF p);
    void on_button_reset_clicked();
    void on_combo_shape_currentIndexChanged(int index);
    void on_check_enable_toggled(bool arg1);
};

#endif  // DOCK_GRID_HPP
