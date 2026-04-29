
#include <cassert>
#include <iostream>
#include "graph.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "edge_normal.hpp"
#include "edge_2strand.hpp"
#include "edges_mark.hpp"
#include "faces.hpp"

int main() {
    // Topology that find_faces likes: a square
    Node n1(QPointF(0, 0));
    Node n2(QPointF(100, 0));
    Node n3(QPointF(100, 100));
    Node n4(QPointF(0, 100));

    Edge_Normal normal_type;
    Edge_2Strand two_strand_type;

    Graph g;
    g.add_node(&n1);
    g.add_node(&n2);
    g.add_node(&n3);
    g.add_node(&n4);

    // 4-cycle
    Edge e1(&n1, &n2, &normal_type);
    Edge e2(&n2, &n3, &normal_type);
    Edge e3(&n3, &n4, &two_strand_type);
    Edge e4(&n4, &n1, &two_strand_type);

    g.add_edge(&e1);
    g.add_edge(&e2);
    g.add_edge(&e3);
    g.add_edge(&e4);

    // Set styles
    e1.set_style(Edge_Style(24, 10, 0.5, &normal_type, Edge_Style::EVERYTHING, 10, 1));
    e2.set_style(Edge_Style(24, 10, 0.5, &normal_type, Edge_Style::EVERYTHING, 10, 1));
    e3.set_style(Edge_Style(24, 10, 0.5, &two_strand_type, Edge_Style::EVERYTHING, 10, 2));
    e4.set_style(Edge_Style(24, 10, 0.5, &two_strand_type, Edge_Style::EVERYTHING, 10, 2));

    std::vector<std::vector<size_t>> faces = find_faces(g);
    std::cout << "Number of faces: " << faces.size() << std::endl;

    GraphMarker marker;
    std::vector<EdgeDistributionTable> tables = marker.edge_distribution_tables(g, faces);
    std::cout << "Number of solutions: " << tables.size() << std::endl;

    bool found_balanced = false;
    for (const auto& table : tables) {
        std::cout << "Table: wa=" << table.wa << ", w0=" << table.w0 << ", label=" << table.label << std::endl;
        // Total weight is 1+1+2+2 = 6. Balanced is 3/3?
        // But user said for record 24 (total weight 4) it should be 2/2.
        // My test has total weight 6. Balanced should be 3/3.
    }

    // Reproduction of user's 'listing' case (total weight 4, 1 red 2-strand edge, 2 blue normal edges)
    // We expect wa=2, w0=2.
    // Let's make a 3-edge loop (triangle) with 1 normal, 1 normal, 1 2-strand.
    Graph g2;
    g2.add_node(&n1);
    g2.add_node(&n2);
    g2.add_node(&n3);
    Edge f1(&n1, &n2, &normal_type);
    Edge f2(&n2, &n3, &normal_type);
    Edge f3(&n3, &n1, &two_strand_type);
    g2.add_edge(&f1);
    g2.add_edge(&f2);
    g2.add_edge(&f3);
    f1.set_style(Edge_Style(24, 10, 0.5, &normal_type, Edge_Style::EVERYTHING, 10, 1));
    f2.set_style(Edge_Style(24, 10, 0.5, &normal_type, Edge_Style::EVERYTHING, 10, 1));
    f3.set_style(Edge_Style(24, 10, 0.5, &two_strand_type, Edge_Style::EVERYTHING, 10, 2));

    std::vector<std::vector<size_t>> faces2 = find_faces(g2);
    std::vector<EdgeDistributionTable> tables2 = marker.edge_distribution_tables(g2, faces2);
    std::cout << "G2 (Listing repro) - Solutions: " << tables2.size() << std::endl;
    bool found_2_2 = false;
    for (const auto& table : tables2) {
        std::cout << "G2 Table: wa=" << table.wa << ", w0=" << table.w0 << std::endl;
        if (table.wa == 2 && table.w0 == 2) found_2_2 = true;
    }
    assert(found_2_2);

    std::cout << "Test passed!" << std::endl;
    return 0;
}
