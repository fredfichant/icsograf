/**
 * \file src/graph/faces.hpp
 * \brief API declarations for faces graph data structures and operations.
 */

#ifndef FACES_HPP
#define FACES_HPP

#include <cstddef>  // for size_t
#include <vector>

class Graph;

// Returns a vector of faces, where each face is a vector of vertex indices.
std::vector<std::vector<std::size_t>> find_faces(const Graph& graph);

#endif  // FACES_HPP
