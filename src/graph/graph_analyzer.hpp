/**
 * \file graph_analyzer.hpp
 * \brief Internal graph analysis helper used by Graph.
 */

#ifndef GRAPH_ANALYZER_HPP
#define GRAPH_ANALYZER_HPP

class Graph;

class Graph_Analyzer
{
   public:
    static void update(const Graph& graph);
};

#endif  // GRAPH_ANALYZER_HPP
