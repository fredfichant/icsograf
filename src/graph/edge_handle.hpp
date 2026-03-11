/**
 * \file src/graph/edge_handle.hpp
 * \brief API declarations for edge handle graph data structures and operations.
 */

#ifndef EDGE_HANDLE_HPP
#define EDGE_HANDLE_HPP

#include <QFlags>
#include <QMetaType>
#include <cstdint>

namespace Edge_Handle_Namespace {
typedef uint16_t Handle;

const Handle NO_HANDLE = 0x0000;
const Handle TOP_LEFT = 0x0001;
const Handle TOP_RIGHT = 0x0002;
const Handle BOTTOM_LEFT = 0x0004;
const Handle BOTTOM_RIGHT = 0x0008;

const Handle MID_TOP_LEFT = 0x0010;
const Handle MID_TOP_RIGHT = 0x0020;
const Handle MID_BOTTOM_LEFT = 0x0040;
const Handle MID_BOTTOM_RIGHT = 0x0080;

const Handle CENTER_TOP_LEFT = 0x0100;
const Handle CENTER_TOP_RIGHT = 0x0200;
const Handle CENTER_BOTTOM_LEFT = 0x0400;
const Handle CENTER_BOTTOM_RIGHT = 0x0800;

const Handle HANDLE_MASK = 0x0FFF;

const Handle STRAND_0 = 0x0000;
const Handle STRAND_1 = 0x1000;
const Handle STRAND_2 = 0x2000;
const Handle STRAND_MASK = 0xF000;

typedef uint32_t Handle_Flags;
}  // namespace Edge_Handle_Namespace

typedef Edge_Handle_Namespace::Handle Edge_Handle;
Q_DECLARE_METATYPE(Edge_Handle)

#endif  // EDGE_HANDLE_HPP
