/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */

#pragma once

#include <flecsi/topo/unstructured/coloring_utils.hh>
#include <flecsi/topo/unstructured/interface.hh>

namespace fsp {

template<std::size_t Dimension>
struct config;

template<>
struct config<1> {
  enum index_space { vertices, edges = vertices, faces = edges, cells };
};

template<std::size_t Dimension>
struct fvm : flecsi::topo::help,
                flecsi::topo::specialization<flecsi::topo::unstructured,
                  fvm<Dimension>> {

  /*--------------------------------------------------------------------------*
    Useful Types.
   *--------------------------------------------------------------------------*/

  using coloring =
    typename flecsi::topo::specialization<flecsi::topo::unstructured,
      fvm<Dimension>>::coloring;

  /*--------------------------------------------------------------------------*
    Policy Information.
   *--------------------------------------------------------------------------*/

  enum index_space { vertices, edges, faces, cells };

  using index_spaces = has<cells, vertices>;

  static constexpr std::size_t dimension = 2;

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

  static void initialize(flecsi::data::topology_slot<fvm> &) {}
};

} // namespace fsp
