
#ifndef COMMAND_LINE_HPP
#define COMMAND_LINE_HPP

#include <QStringList>

class Command_Line
{
    QStringList args;

    QStringList m_files;
    bool ui;
    bool antialias;
    bool include_graph;

   public:
    Command_Line(int argc, char* argv[]);

    QStringList files() const { return m_files; }
    bool load_ui() const { return ui; }

   private:
    void license() const;
    void help() const;
    void version() const;
};

#endif  // COMMAND_LINE_HPP
