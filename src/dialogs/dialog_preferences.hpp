#ifndef DIALOG_PREFERENCES_HPP
#define DIALOG_PREFERENCES_HPP

#include <QMainWindow>

#include "ui_dialog_preferences.h"

class Dialog_Preferences : public QDialog, private Ui::Dialog_Preferences
{
    Q_OBJECT

   public:
    explicit Dialog_Preferences(QMainWindow* parent);

   private:
    void init_combos();
    void updateColorButton(QPushButton* button, const QColor& color);

    // QColor member variables
    QColor m_colorGridLines;
    QColor m_colorNodeResting;
    QColor m_colorNodeHighlighted;
    QColor m_colorNodeSelected;
    QColor m_colorEdgeResting;
    QColor m_colorEdgeHighlighted;
    QColor m_colorEdgeSelected;

   protected slots:
    void set_preferences();

   private slots:
    void on_button_clear_recent_clicked();
    void on_button_clear_settings_clicked();
    void on_combo_widget_style_currentIndexChanged(int);

    // New slots for color buttons
    void on_button_color_grid_lines_clicked();
    void on_button_color_node_resting_clicked();
    void on_button_color_node_highlighted_clicked();
    void on_button_color_node_selected_clicked();
    void on_button_color_edge_resting_clicked();
    void on_button_color_edge_highlighted_clicked();
    void on_button_color_edge_selected_clicked();
};

#endif  // DIALOG_PREFERENCES_HPP
