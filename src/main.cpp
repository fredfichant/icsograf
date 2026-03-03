#include <QApplication>

#include "QDebug"
#include "command_line.hpp"
#include "edge_2strand.hpp"
#include "edge_3strand.hpp"
#include "edge_normal.hpp"
#include "main_window.hpp"
#include "resource_manager.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    qDebug() << "=== KNOTTER START ===";

    resource_manager().register_cusp_shape(new Cusp_Rounded);

    resource_manager().register_edge_type(new Edge_Normal);
    resource_manager().register_edge_type(new Edge_Inverted);
    resource_manager().register_edge_type(new Edge_2Strand);
    resource_manager().register_edge_type(new Edge_2Strand_Inverted);
    resource_manager().register_edge_type(new Edge_3Strand);
    resource_manager().register_edge_type(new Edge_3Strand_Inverted);

    resource_manager().initialize();

    Command_Line cmd(argc, argv);

    if (!cmd.load_ui()) return 0;

    Main_Window mw;
    mw.show();

    const auto files = cmd.files();  // garde une copie locale
    for (const QString& s : files) mw.create_new_tab(s);

    return a.exec();
}
