/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */

#pragma once

#include <flecsi/topo/unstructured/interface.hh>

namespace fsp {

template<std::size_t Dimension>
struct config;

template<>
struct config<1> {
  enum index_space {
    vertices,
    edges = vertices,
    faces = edges,
    cells
  };
};

template<std::size_t Dimension>
struct burton :
  flecsi::topo::specialization<flecsi::topo::unstructured, burton> {

  /*--------------------------------------------------------------------------*
    Policy Information.
   *--------------------------------------------------------------------------*/

  enum index_space {
    vertices,
    edges,
    faces,
    cells
  };

  using index_spaces = has<cells, vertices>;

  static constexpr std::size_t dimension = Dimension;

  template<auto>
  static constexpr std::size_t privilege_count = 2;

  /*--------------------------------------------------------------------------*
    Interface.
   *--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*
    Coloring.
   *--------------------------------------------------------------------------*/

  static coloring color(std::string const &) {
    return {};
  } // color

  /*--------------------------------------------------------------------------*
    Initialization.
   *--------------------------------------------------------------------------*/

  static void initialize(data::topology_slot<burton> &) {
  } // initialize

};

} // namespace fsp
