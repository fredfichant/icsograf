#include "dialog_preferences.hpp"

#include <QColorDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStyleFactory>

#include "resource_manager.hpp"

Dialog_Preferences::Dialog_Preferences(QMainWindow* parent)
    : QDialog(parent),
      m_colorGridLines(Snapping_Grid::line_color),
      m_colorNodeResting(Node::color_resting),
      m_colorNodeHighlighted(Node::color_highlighted),
      m_colorNodeSelected(Node::color_selected),
      m_colorEdgeResting(Edge::color_resting),
      m_colorEdgeHighlighted(Edge::color_highlighted),
      m_colorEdgeSelected(Edge::color_selected)
{
    setupUi(this);

    init_combos();

    check_fluid_refresh->setChecked(resource_manager().settings.fluid_refresh());
    check_cache_image->setChecked(resource_manager().settings.graph_cache());
    check_antialiasing->setChecked(resource_manager().settings.antialiasing());

    spin_recent_files->setValue(resource_manager().settings.max_recent_files());
    check_save_geometry->setChecked(resource_manager().settings.save_ui());

    check_save_grid->setChecked(resource_manager().settings.save_grid());
    check_save_toolbars->setChecked(resource_manager().settings.save_toolbars());
    check_save_style->setChecked(resource_manager().settings.save_knot_style());

    check_files_close->setChecked(resource_manager().settings.check_unsaved_files());

    check_clipboard_png->setChecked(resource_manager().settings.clipboard_feature(Settings::PNG));
    check_clipboard_tiff->setChecked(resource_manager().settings.clipboard_feature(Settings::TIFF));
    check_clipboard_svg->setChecked(resource_manager().settings.clipboard_feature(Settings::SVG));
    check_clipboard_xml->setChecked(resource_manager().settings.clipboard_feature(Settings::XML));

    stackedWidget->setCurrentIndex(0);
    tableWidget->setCurrentCell(0, 0);

    foreach (QString k, QStyleFactory::keys()) combo_widget_style->addItem(k);
    combo_widget_style->setCurrentIndex(
        combo_widget_style->findText(QApplication::style()->objectName(), Qt::MatchFixedString));

    updateColorButton(button_color_grid_lines, m_colorGridLines);
    updateColorButton(button_color_node_resting, m_colorNodeResting);
    updateColorButton(button_color_node_highlighted, m_colorNodeHighlighted);
    updateColorButton(button_color_node_selected, m_colorNodeSelected);
    updateColorButton(button_color_edge_resting, m_colorEdgeResting);
    updateColorButton(button_color_edge_highlighted, m_colorEdgeHighlighted);
    updateColorButton(button_color_edge_selected, m_colorEdgeSelected);

    // Initialize the appearance of the new color buttons
    updateColorButton(button_color_grid_lines, m_colorGridLines);
    updateColorButton(button_color_node_resting, m_colorNodeResting);
    updateColorButton(button_color_node_highlighted, m_colorNodeHighlighted);
    updateColorButton(button_color_node_selected, m_colorNodeSelected);
    updateColorButton(button_color_edge_resting, m_colorEdgeResting);
    updateColorButton(button_color_edge_highlighted, m_colorEdgeHighlighted);
    updateColorButton(button_color_edge_selected, m_colorEdgeSelected);
}

void Dialog_Preferences::init_combos()
{
    combo_node_size->clear();
    combo_node_size->addItem(tr("Small"), 3);
    combo_node_size->addItem(tr("Medium"), 5);
    combo_node_size->addItem(tr("Large"), 15);
    combo_node_size->setCurrentIndex(combo_node_size->findData(Node::radius));
}

void Dialog_Preferences::set_preferences()
{
    Node::radius = combo_node_size->itemData(combo_node_size->currentIndex()).toInt();
    Node::color_resting = m_colorNodeResting;
    Node::color_highlighted = m_colorNodeHighlighted;
    Node::color_selected = m_colorNodeSelected;

    Edge::color_highlighted = m_colorEdgeHighlighted;
    Edge::color_resting = m_colorEdgeResting;
    Edge::color_selected = m_colorEdgeSelected;

    Snapping_Grid::line_color = m_colorGridLines;

    QApplication::setStyle(combo_widget_style->currentText());
    combo_widget_style->blockSignals(true);  //< prevent retranslate to mess up things

    resource_manager().settings.set_fluid_refresh(check_fluid_refresh->isChecked());
    resource_manager().settings.set_graph_cache(check_cache_image->isChecked());
    resource_manager().settings.set_antialiasing(check_antialiasing->isChecked());

    resource_manager().settings.set_max_recent_files(spin_recent_files->value());
    resource_manager().settings.set_save_ui(check_save_geometry->isChecked());

    resource_manager().settings.set_save_grid(check_save_grid->isChecked());
    resource_manager().settings.set_save_toolbars(check_save_toolbars->isChecked());
    resource_manager().settings.set_save_knot_style(check_save_style->isChecked());

    resource_manager().settings.set_check_unsaved_files(check_files_close->isChecked());

    resource_manager().settings.set_clipboard_feature(Settings::PNG,
                                                      check_clipboard_png->isChecked());
    resource_manager().settings.set_clipboard_feature(Settings::TIFF,
                                                      check_clipboard_tiff->isChecked());
    resource_manager().settings.set_clipboard_feature(Settings::SVG,
                                                      check_clipboard_svg->isChecked());
    resource_manager().settings.set_clipboard_feature(Settings::XML,
                                                      check_clipboard_xml->isChecked());
}

void Dialog_Preferences::on_button_clear_recent_clicked()
{
    resource_manager().settings.clear_recent_files();
}

void Dialog_Preferences::on_button_clear_settings_clicked()
{
    resource_manager().settings.clear_config();
    QMessageBox::information(this, tr("Clearing Configuration"),
                             tr("Next time %1 will start with the default settings.")
                                 .arg(resource_manager().program.name()));
    // group_save->setEnabled(false);
}

void Dialog_Preferences::on_combo_widget_style_currentIndexChanged(int)
{
    QStyle* old_style = group_style_preview->style();
    QStyle* style = QStyleFactory::create(combo_widget_style->currentText());
    group_style_preview->setStyle(style);
    foreach (QWidget* c, group_style_preview->findChildren<QWidget*>()) {
        c->setStyle(style);
    }
    if (old_style != QApplication::style()) delete old_style;
}

void Dialog_Preferences::updateColorButton(QPushButton* button, const QColor& color)
{
    if (button) {
        QString styleSheet = QString("background-color: %1; border: 1px solid gray;")
                                 .arg(color.name(QColor::HexArgb));
        button->setStyleSheet(styleSheet);
    }
}

void Dialog_Preferences::on_button_color_grid_lines_clicked()
{
    QColor newColor = QColorDialog::getColor(m_colorGridLines, this, tr("Select Grid Line Color"));
    if (newColor.isValid()) {
        m_colorGridLines = newColor;
        updateColorButton(button_color_grid_lines, m_colorGridLines);
    }
}

void Dialog_Preferences::on_button_color_node_resting_clicked()
{
    QColor newColor =
        QColorDialog::getColor(m_colorNodeResting, this, tr("Select Node Resting Color"));
    if (newColor.isValid()) {
        m_colorNodeResting = newColor;
        updateColorButton(button_color_node_resting, m_colorNodeResting);
    }
}

void Dialog_Preferences::on_button_color_node_highlighted_clicked()
{
    QColor newColor =
        QColorDialog::getColor(m_colorNodeHighlighted, this, tr("Select Node Highlighted Color"));
    if (newColor.isValid()) {
        m_colorNodeHighlighted = newColor;
        updateColorButton(button_color_node_highlighted, m_colorNodeHighlighted);
    }
}

void Dialog_Preferences::on_button_color_node_selected_clicked()
{
    QColor newColor =
        QColorDialog::getColor(m_colorNodeSelected, this, tr("Select Node Selected Color"));
    if (newColor.isValid()) {
        m_colorNodeSelected = newColor;
        updateColorButton(button_color_node_selected, m_colorNodeSelected);
    }
}

void Dialog_Preferences::on_button_color_edge_resting_clicked()
{
    QColor newColor =
        QColorDialog::getColor(m_colorEdgeResting, this, tr("Select Edge Resting Color"));
    if (newColor.isValid()) {
        m_colorEdgeResting = newColor;
        updateColorButton(button_color_edge_resting, m_colorEdgeResting);
    }
}

void Dialog_Preferences::on_button_color_edge_highlighted_clicked()
{
    QColor newColor =
        QColorDialog::getColor(m_colorEdgeHighlighted, this, tr("Select Edge Highlighted Color"));
    if (newColor.isValid()) {
        m_colorEdgeHighlighted = newColor;
        updateColorButton(button_color_edge_highlighted, m_colorEdgeHighlighted);
    }
}

void Dialog_Preferences::on_button_color_edge_selected_clicked()
{
    QColor newColor =
        QColorDialog::getColor(m_colorEdgeSelected, this, tr("Select Edge Selected Color"));
    if (newColor.isValid()) {
        m_colorEdgeSelected = newColor;
        updateColorButton(button_color_edge_selected, m_colorEdgeSelected);
    }
}
