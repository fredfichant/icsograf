/**
 * \file src/io/xml_loader.cpp
 * \brief Implementation of xml loader import and export routines.
 */

#include "xml_loader.hpp"

#include <QBuffer>

#include "xml_loader_private.hpp"  // We'll rename this file later
#include "xml_loader_v2.hpp"

void import_xml_style(QString style, Graph& graph)
{
    QByteArray output(style.toUtf8());
    QBuffer buffer(&output);
    buffer.open(QIODevice::ReadOnly);
    XML_Loader().load_style(&buffer, &graph);
}

bool import_xml(QIODevice& file, Graph& graph)
{
    if (!file.isOpen() && !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    XML_Loader xml;

    if (xml.load(&file, &graph)) return true;

    // Fallback to legacy V2
    if (XML_Loader_v2::supports_version(xml.version())) {
        file.seek(0);
        XML_Loader_v2 xml2;

        if (!xml2.load(&file)) return false;

        xml2.get_graph(graph);
        return true;
    }

    return false;
}
