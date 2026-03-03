/**
 * \file xml_loader.hpp
 * \brief High-level XML importing for Knotter
 */

#ifndef XML_LOADER_HPP
#define XML_LOADER_HPP

#include <QDomDocument>

#include "graph.hpp"

/**
 * \brief Imports a graph from an XML file or device
 * \param file The input device containing XML data
 * \param graph The graph object to populate
 * \return true if successful, false otherwise
 */
bool import_xml(QIODevice& file, Graph& graph);

/**
 * \brief Imports style information only from an XML string
 * \param style The XML string containing style information
 * \param graph The graph to apply the style to
 */
void import_xml_style(QString style, Graph& graph);

#endif  // XML_LOADER_HPP
