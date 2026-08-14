#ifndef GRAPH_SYMMETRY_ANALYZER_HPP
#define GRAPH_SYMMETRY_ANALYZER_HPP

#include <QVector>
#include <QLineF>

class Graph;

struct Graph_Symmetry_Result
{
    QVector<QVector<int>> generators;
    QVector<QVector<int>> orbits;
    QVector<QLineF> reflection_axes;
    int group_order = 1;
    bool complete = true;
};

class Graph_Symmetry_Analyzer
{
   public:
    static Graph_Symmetry_Result find_automorphisms(const Graph& graph,
                                                     int permutation_limit = 10000);
};

#endif  // GRAPH_SYMMETRY_ANALYZER_HPP
