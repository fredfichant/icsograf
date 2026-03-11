/**
 * \file src/command_line.cpp
 * \brief Implementation of command line application module.
 */

#include "command_line.hpp"

#include <QFile>
#include <iostream>

#include "graph.hpp"
#include "image_exporter.hpp"
#include "resource_manager.hpp"
#include "xml_loader.hpp"

Command_Line::Command_Line(int argc, char* argv[])
    : ui(true), antialias(false), include_graph(false)
{
    for (int i = 1; i < argc; i++) {
        QString arg = QString::fromLocal8Bit(argv[i]);

        if (arg == "-h" || arg == "--help") {
            ui = false;
            help();
            break;
        } else if (arg == "-v" || arg == "--version") {
            ui = false;
            version();
            break;
        } else if (arg == "-l" || arg == "--license") {
            ui = false;
            license();
            break;
        } else if (arg == "-b" || arg == "--no-gui") {
            ui = false;
        } else if (arg == "-a" || arg == "--antialias") {
            antialias = true;
        } else if (arg == "-na" || arg == "--no-antialias") {
            antialias = false;
        } else if (arg == "-g" || arg == "--graph") {
            include_graph = true;
        } else if (arg == "-ng" || arg == "--no-graph") {
            include_graph = false;
        } else if (arg == "-o" || arg == "--output") {
            if (i >= argc - 1)
                qWarning() << QObject::tr("Warning:")
                           << QObject::tr("Missing file for argument %1").arg(arg);
            else if (m_files.isEmpty())
                qWarning() << QObject::tr("Warning:")
                           << QObject::tr("No input file specified for option %1").arg(arg);
            else {
                i++;
                QFile infile(m_files.back());
                QFile outfile(QString::fromLocal8Bit(argv[i]));
                Graph g;

                if (import_xml(infile, g)) {
                    if (outfile.fileName().endsWith(".svg")) export_svg(outfile, g, include_graph);
                }
            }
        } else {
            m_files.push_back(arg);
        }
    }
}

void Command_Line::license() const
{
    std::cout << "GNU General Public License version 3 or any later version." << std::endl;
}

void Command_Line::help() const
{
    version();
    std::cout << "Usage:\n"
              << "knotter [args file [-o output] ...] [qt-options ...]\n"
              << "knotter -(h|v|l)\n"

              << "\n"
              << "\tInformation:\n"
              << "-h, --help\n"
              << "\tPrint help and exit\n"
              << "-v, --version\n"
              << "\tDisplay version information and exit\n"
              << "-l, --license\n"
              << "\tDisplay license and exit\n"

              << "\n"
              << "Output:\n"
              << "-o file, --output file\n"
              << "\tExport last loaded file. Format is deducend from the extension.\n"
              << "-a, --antialias\n"
              << "\tEnable antialiasing for the following exports."
              << "-na, --no-antialias\n"
              << "\tDisable antialiasing for the following exports."
              << "-g, --graph\n"
              << "\tEnable graph output for the following exports."
              << "-ng, --no-graph\n"
              << "\tDisable graph output for the following exports."

              << "\n"
              << "Misc:\n"
              << "-b, --no-gui\n"
              << "\tDon't start the gui after parsing the command line.\n"
              << std::endl;
}

void Command_Line::version() const
{
    std::cout << resource_manager().program.name().toStdString() << " "
              << resource_manager().program.version().toStdString() << std::endl;
}
