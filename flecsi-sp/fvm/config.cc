/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */

#include "config.hh"
#include <flecsi/topo/unstructured/interface.hh>


namespace fsp {

using namespace flecsi;

template<>
struct config<1, false> : config_base<1, false>
{
  enum index_space { vertices, edges = vertices, faces = edges, cells };
  using index_spaces = has<vertices, cells>;
  using connectivities = fully_connected<index_spaces>::type;
};


template<>
struct config<1, true> : config_base<1, true>
{
  enum index_space { vertices, edges = vertices, faces = edges, cells,
    corners, wedges = corners, sides = corners };
  using index_spaces = has<vertices, cells, corners>;
  using connectivities = fully_connected<index_spaces>::type;
};


template<>
struct config<2, false> : config_base<2, false>
{
  enum index_space { vertices, edges, faces = edges, cells };
  using index_spaces = has<vertices, edges, cells>;
  using connectivities = fully_connected<index_spaces>::type;
};


template<>
struct config<2, true> : config_base<2, true>
{
  enum index_space { vertices, edges, faces = edges, cells, corners, wedges, sides };
  using index_spaces = has<vertices, edges, cells, corners, wedges, sides>;
  using connectivities = fully_connected<index_spaces>::type;
};


template<>
struct config<3, false> : config_base<3, false>
{
  enum index_space { vertices, edges, faces, cells };
  using index_spaces = has<vertices, edges, faces, cells>;
  using connectivities = fully_connected<index_spaces>::type;
};


template<>
struct config<3, true> : config_base<3, true>
{
  enum index_space { vertices, edges, faces, cells, corners, wedges, sides };
  using index_spaces = has<vertices, edges, faces, cells, corners, wedges, sides>;
  using connectivities = fully_connected<index_spaces>::type;
};


} // namespace fsp
