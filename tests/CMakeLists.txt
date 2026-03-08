cmake_minimum_required(VERSION 3.10)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Svg PrintSupport Xml UiTools Network)

set(TEST_CORE_SOURCES
  ${CMAKE_SOURCE_DIR}/src/graph/graph_item.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/node.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/graph.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge_type.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge_type_utils.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge_handle_calculator.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge_inverted_logic.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge_normal.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge_2strand.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge_3strand.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/node_style.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/node_cusp_shape.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/path_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/path_item.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edge_style.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/graph_properties.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/edges_mark.cpp
  ${CMAKE_SOURCE_DIR}/src/graph/faces.cpp
  ${CMAKE_SOURCE_DIR}/src/resource_manager.cpp
  ${CMAKE_SOURCE_DIR}/src/settings.cpp
  ${CMAKE_SOURCE_DIR}/src/application_info.cpp
  ${CMAKE_SOURCE_DIR}/src/string_toolbar.cpp
  ${CMAKE_SOURCE_DIR}/src/widgets/knot_view/snapping_grid.cpp
  ${CMAKE_SOURCE_DIR}/src/io/xml_loader.cpp
  ${CMAKE_SOURCE_DIR}/src/io/xml_loader_private.cpp
  ${CMAKE_SOURCE_DIR}/src/io/xml_loader_v2.cpp
  ${CMAKE_SOURCE_DIR}/src/io/xml_exporter.cpp
  ${CMAKE_SOURCE_DIR}/src/io/image_exporter.cpp
)

set(TEST_INCLUDE_DIRS
  ${CMAKE_BINARY_DIR}/bin
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/graph
  ${CMAKE_SOURCE_DIR}/src/io
  ${CMAKE_SOURCE_DIR}/src/dialogs
  ${CMAKE_SOURCE_DIR}/src/widgets
  ${CMAKE_SOURCE_DIR}/src/widgets/other_widgets
  ${CMAKE_SOURCE_DIR}/src/widgets/knot_view
)

function(add_icsograf_test test_name test_file)
  add_executable(${test_name}
    ${test_file}
    ${TEST_CORE_SOURCES}
  )

  target_include_directories(${test_name}
    PRIVATE
      ${TEST_INCLUDE_DIRS}
  )

  target_link_libraries(${test_name}
    PRIVATE
      Qt5::Core
      Qt5::Gui
      Qt5::Widgets
      Qt5::Svg
      Qt5::PrintSupport
      Qt5::Xml
      Qt5::Network
      Qt5::UiTools
  )

  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_icsograf_test(test_edge_handles ${CMAKE_CURRENT_SOURCE_DIR}/test_edge_handles.cpp)
add_icsograf_test(test_edge_2strand ${CMAKE_CURRENT_SOURCE_DIR}/test_edge_2strand.cpp)
add_icsograf_test(test_edge_3strand ${CMAKE_CURRENT_SOURCE_DIR}/test_edge_3strand.cpp)
add_icsograf_test(test_edge_isolated_routing ${CMAKE_CURRENT_SOURCE_DIR}/test_edge_isolated_routing.cpp)
add_icsograf_test(test_graph_two_edges_2strand ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_two_edges_2strand.cpp)
add_icsograf_test(test_graph_three_edges_2strand ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_three_edges_2strand.cpp)
add_icsograf_test(test_graph_three_edges_2strand_inverted ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_three_edges_2strand_inverted.cpp)
add_icsograf_test(test_graph_three_edges_3strand ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_three_edges_3strand.cpp)
add_icsograf_test(test_graph_first_edge_3strand ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_first_edge_3strand.cpp)
add_icsograf_test(test_graph_properties_2strand_faces ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_properties_2strand_faces.cpp)
add_icsograf_test(test_graph_properties_3strand_faces ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_properties_3strand_faces.cpp)
add_icsograf_test(test_graph_properties_edge_distribution ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_properties_edge_distribution.cpp)
add_icsograf_test(test_graph_properties_multistrand_weights ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_properties_multistrand_weights.cpp)
