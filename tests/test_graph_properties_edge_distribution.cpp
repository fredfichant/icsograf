/**
 * \file tests/test_graph_properties_edge_distribution.cpp
 * \brief Unit tests for graph properties edge distribution.
 */

#include <cassert>

#include "edge_2strand.hpp"
#include "edge_normal.hpp"
#include "edge_3strand.hpp"
#include "graph.hpp"

static void assert_table_totals_match_edge_count(const Graph& graph)
{
    const std::vector<EdgeDistributionTable>& tables = graph.properties()->edge_distribution_tables();
    assert(!tables.empty());
    for (const EdgeDistributionTable& table : tables) {
        assert(table.wa + table.w0 + table.p0 + table.pa == graph.properties()->edge_count());
    }
}

int main()
{
    Node n1(QPointF(0, 0));
    Node n2(QPointF(100, 0));
    Edge_Normal regular_type;

    Graph g;
    g.add_node(&n1);
    g.add_node(&n2);

    Edge e(&n1, &n2, &regular_type);
    g.add_edge(&e);
    e.set_style(Edge_Style(24, 10, 0.5, &regular_type, Edge_Style::EVERYTHING, 10, 1));

    g.render_knot();

    const int total = g.properties()->wa() + g.properties()->w0() + g.properties()->p0() +
                      g.properties()->pa();
    assert(total > 0);
    assert(g.properties()->edge_distribution_tables().size() == 1);
    assert_table_totals_match_edge_count(g);

    Node m1(QPointF(-200, 0));
    Node m2(QPointF(-50, 80));
    Node m3(QPointF(120, 60));
    Node m4(QPointF(80, -120));
    Edge_2Strand two_strand_type;

    Graph multi;
    multi.add_node(&m1);
    multi.add_node(&m2);
    multi.add_node(&m3);
    multi.add_node(&m4);

    Edge f1(&m1, &m2, &regular_type);
    Edge f2(&m2, &m3, &regular_type);
    Edge f3(&m3, &m4, &two_strand_type);
    multi.add_edge(&f1);
    multi.add_edge(&f2);
    multi.add_edge(&f3);
    multi.render_knot();

    const std::vector<EdgeDistributionTable>& tables = multi.properties()->edge_distribution_tables();
    assert(multi.properties()->group_count() == 2);
    assert(tables.size() == 2);

    bool found_all_zero = false;
    bool found_split = false;
    for (const EdgeDistributionTable& table : tables) {
        assert(table.wa + table.w0 + table.p0 + table.pa == multi.properties()->edge_count());
        if (table.wa == 0 && table.w0 == 4 && table.p0 == 0 && table.pa == 0) {
            found_all_zero = true;
        }
        if (table.wa == 1 && table.w0 == 3 && table.p0 == 0 && table.pa == 0) {
            found_split = true;
        }
    }
    assert(found_all_zero);
    assert(found_split);

    Node i1(QPointF(-200, 0));
    Node i2(QPointF(-50, 80));
    Node i3(QPointF(120, 60));
    Node i4(QPointF(80, -120));
    Edge_2Strand_Inverted two_strand_inverted_type;

    Graph inverted;
    inverted.add_node(&i1);
    inverted.add_node(&i2);
    inverted.add_node(&i3);
    inverted.add_node(&i4);

    Edge g1(&i1, &i2, &regular_type);
    Edge g2(&i2, &i3, &regular_type);
    Edge g3(&i3, &i4, &two_strand_inverted_type);
    inverted.add_edge(&g1);
    inverted.add_edge(&g2);
    inverted.add_edge(&g3);
    inverted.render_knot();
    assert_table_totals_match_edge_count(inverted);

    Node t1(QPointF(-200, 0));
    Node t2(QPointF(-50, 80));
    Node t3(QPointF(120, 60));
    Node t4(QPointF(80, -120));
    Edge_3Strand three_strand_type;

    Graph three;
    three.add_node(&t1);
    three.add_node(&t2);
    three.add_node(&t3);
    three.add_node(&t4);

    Edge h1(&t1, &t2, &regular_type);
    Edge h2(&t2, &t3, &regular_type);
    Edge h3(&t3, &t4, &three_strand_type);
    three.add_edge(&h1);
    three.add_edge(&h2);
    three.add_edge(&h3);
    three.render_knot();
    assert_table_totals_match_edge_count(three);

    return 0;
}
