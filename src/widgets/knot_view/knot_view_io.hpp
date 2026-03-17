/**
 * \file src/widgets/knot_view/knot_view_io.hpp
 * \brief Internal I/O and clipboard helper for Knot_View.
 */

#ifndef KNOT_VIEW_IO_HPP
#define KNOT_VIEW_IO_HPP

#include <QString>

class QIODevice;
class Graph;
class Knot_View;

class Knot_View_IO
{
   public:
    explicit Knot_View_IO(Knot_View& view) : m_view(view) {}

    bool load_file(QIODevice& device, QString action_name);
    bool load_file(QString file_name);
    bool save_file(QString file_name);
    void copy_selection();
    void paste_selection();
    void cut_selection();
    bool insert(const Graph& graph, QString macro_name);

   private:
    Knot_View& m_view;
};

#endif  // KNOT_VIEW_IO_HPP
