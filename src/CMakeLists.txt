cmake_minimum_required(VERSION 3.10)

# Si ton CMakeLists racine fait déjà project() et les réglages CMAKE_AUTOMOC/AUTOUIC/AUTORCC,
# tu peux laisser ce fichier simple. On garde ici ce qui est utile au sous-projet src/.

# --- Qt (portable) ---
# Normalement déjà fait au niveau racine, mais le répéter ici ne casse rien.
find_package(Qt5 REQUIRED COMPONENTS
  Core Gui Widgets Svg PrintSupport Xml UiTools Network Sql
)

# --- Fichier généré (version/infos appli) ---
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/knotter_info.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/knotter_info.hpp
)

# --- Sources ---
set(SOURCES

  database/graph_repository.cpp
  database/graph_repository_codec.cpp
  database/graph_browser_dialog.cpp
  database/graph_browser_dialog_ui.cpp
  database/graph_browser_dialog_data.cpp
  database/graph_save_dialog.cpp

  dialogs/main_window.cpp
  dialogs/polygon_dialog.cpp
  dialogs/dialog_preferences.cpp
  dialogs/export_image_dialog.cpp
  dialogs/dock/dock_grid.cpp
  dialogs/dock/dock_properties.cpp
  dialogs/cusp_style_dialog.cpp
  dialogs/dialog_confirm_close.cpp
  dialogs/icon_dock_style.cpp

  dialogs/main_window.ui
  dialogs/dialog_preferences.ui
  dialogs/export_image_dialog.ui
  dialogs/dock/dock_grid.ui
  dialogs/cusp_style_dialog.ui
  dialogs/dialog_confirm_close.ui

  graph/node.cpp
  graph/graph.cpp
  graph/edge.cpp
  graph/edge_type.cpp
  graph/edge_type_utils.cpp
  graph/edge_handle_calculator.cpp
  graph/edge_inverted_logic.cpp
  graph/edge_normal.cpp
  graph/edge_2strand.cpp
  graph/edge_3strand.cpp
  graph/graph_item.cpp
  graph/node_style.cpp
  graph/path_builder.cpp
  graph/path_item.cpp
  graph/node_cusp_shape.cpp
  graph/edge_style.cpp
  graph/edges_mark.cpp
  graph/graph_properties.cpp
  graph/graph_validation.cpp
  graph/properties_view.cpp
  graph/faces.cpp
  graph/polygon_generator.cpp

  io/image_exporter.cpp
  io/xml_loader_v2.cpp
  io/xml_exporter.cpp
  io/xml_loader_private.cpp
  io/xml_loader.cpp

  widgets/knot_view/knot_view.cpp
  widgets/knot_view/undo_manager.cpp
  widgets/knot_view/commands.cpp
  widgets/knot_view/snapping_grid.cpp
  widgets/knot_view/node_mover.cpp
  widgets/knot_view/context_menu_node.cpp
  widgets/knot_view/context_menu_edge.cpp
  widgets/knot_view/transform_handle.cpp
  widgets/knot_view/knot_tool.cpp
  widgets/status_bar.cpp

  widgets/other_widgets/cusp_style_widget.cpp
  widgets/other_widgets/cusp_style_widget.ui
   widgets/other_widgets/edge_type_widget.cpp
  widgets/other_widgets/edge_type_widget.ui

  main.cpp
  resource_manager.cpp
  settings.cpp
  string_toolbar.cpp
  command_line.cpp
  application_info.cpp
  knot_file_manager.cpp
)

if(WIN32)
  list(APPEND SOURCES windows.rc)
endif()

# --- Exécutable ---
add_executable(${PROJECT_SLUG}
  ${SOURCES}
  resources.qrc
)

# --- Includes (portable) ---
target_include_directories(${PROJECT_SLUG}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}            # knotter_info.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/database
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/dock
    ${CMAKE_CURRENT_SOURCE_DIR}/graph
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/other_widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/knot_view
    ${CMAKE_CURRENT_SOURCE_DIR}/io
)

# --- Link (portable) ---
target_link_libraries(${PROJECT_SLUG}
  PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Svg
    Qt5::PrintSupport
    Qt5::Xml
    Qt5::Network
    Qt5::UiTools
    Qt5::Sql
)

# --- Warnings ---
target_compile_options(${PROJECT_SLUG}
  PRIVATE
    -Wall -Wextra -Wno-deprecated-declarations
)

# --- Install ---
install(TARGETS ${PROJECT_SLUG} DESTINATION bin)
