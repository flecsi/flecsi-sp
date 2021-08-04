/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */

#pragma once
#include <exodusII.h>

#include "flecsi-sp/io/exodus_definition.hh"

namespace fsp {
namespace io {

void write_headers(int exoid);

template<class U, class Def>
void
write_block(int exoid,
  Def & def,
  typename Def::index blkid,
  typename Def::block_t blktype,
  std::vector<U> & entity_nodes,
  std::vector<int> & entity_node_counts,
  std::vector<typename Def::real> block_coloring) {
  using ex_index_t = U;
  std::string type_str;
  switch(blktype) {
  case(Def::block_t::quad):
    type_str = "quad4";
    break;
  case(Def::block_t::tri):
    type_str = "tri3";
    break;
  case(Def::block_t::hex):
    type_str = "hex8";
    break;
  case(Def::block_t::tet):
    type_str = "tet4";
    break;
  default:
    type_str = "nsided";
  };

  ex_index_t num_nodes_this_blk = entity_nodes.size();

  ex_index_t num_entries_this_blk =
    (type_str == "nsided") ? entity_node_counts.size() : block_coloring.size();
  ex_index_t num_nodes_per_entry =
    (type_str == "nsided") ? num_nodes_this_blk
                           : num_nodes_this_blk / num_entries_this_blk;
  ex_index_t num_edges_per_entry = 0;
  ex_index_t num_faces_per_entry = 0;
  ex_index_t num_attr_per_entry = 0;

  auto status = ex_put_block(exoid,
    EX_ELEM_BLOCK,
    blkid,
    type_str.c_str(),
    num_entries_this_blk,
    num_nodes_per_entry,
    num_edges_per_entry,
    num_faces_per_entry,
    num_attr_per_entry);
  if(status)
    flog_fatal("Problem writing block header to exodus file, "
               << " ex_put_block() returned " << status);

  std::string blkname = "cell-block-" + std::to_string(blkid);
  status = ex_put_name(exoid, EX_ELEM_BLOCK, blkid, blkname.c_str());
  if(status)
    flog_fatal("Problem writing block name to exodus file, "
               << " ex_put_name() returned " << status);

  status = ex_put_conn(
    exoid, EX_ELEM_BLOCK, blkid, entity_nodes.data(), nullptr, nullptr);
  if(status)
    flog_fatal("Problem writing block connectivity to exodus file, "
               << " ex_put_conn() returned " << status);

  if(type_str == "nsided") {
    status = ex_put_entity_count_per_polyhedra(
      exoid, EX_ELEM_BLOCK, blkid, entity_node_counts.data());
    if(status)
      flog_fatal("Problem writing block counts to exodus file, "
                 << " ex_put_entity_count_per_polyhedra() returned " << status);
  }

  status = ex_put_var(exoid,
    1,
    EX_ELEM_BLOCK,
    1,
    blkid,
    block_coloring.size(),
    block_coloring.data());
  if(status)
    flog_fatal("Problem writing vars to exodus file, "
               << "ex_put_var() returned " << status);
}

template<class U, class Def>
void
write_blocks(int exoid, Def & def, const std::vector<std::size_t> & colors) {
  using ex_index_t = U;
  std::vector<ex_index_t> entity_nodes;
  std::vector<typename Def::real> block_coloring;
  std::vector<int> entity_node_counts;

  int current_block = 0;
  for(typename Def::size cellid{0}; cellid < def.num_entities(Def::num_dims);
      cellid++) {
    std::vector<typename Def::index> nodes;
    def.stream(cellid, nodes);
    const auto & curr_loc = def.get_block_cursor().current_location();
    auto blktype = def.block_type(curr_loc.block);
    auto blkid = def.block_id(curr_loc.block);
    if(current_block != curr_loc.block) {
      write_block<U>(exoid,
        def,
        def.block_id(current_block),
        def.block_type(current_block),
        entity_nodes,
        entity_node_counts,
        block_coloring);
      entity_nodes.clear();
      block_coloring.clear();
      entity_node_counts.clear();
    }
    if(blktype == Def::block_t::polygon)
      entity_node_counts.emplace_back(nodes.size());
    for(auto nodeid : nodes) {
      entity_nodes.emplace_back(nodeid + 1);
    }
    block_coloring.emplace_back(colors[cellid]);
    current_block = curr_loc.block;
  }
  write_block<U>(exoid,
    def,
    def.block_id(current_block),
    def.block_type(current_block),
    entity_nodes,
    entity_node_counts,
    block_coloring);
}

template<class Def>
void
write_coloring(const char * name,
  Def & def,
  const std::vector<std::size_t> & colors) {
  auto exoid = Def::open(name, std::ios_base::out);
  auto params = def.get_params();
  Def::write_params(exoid, params);

  write_headers(exoid);
  if(Def::is_int64(exoid))
    write_blocks<long long>(exoid, def, colors);
  else
    write_blocks<int>(exoid, def, colors);

  auto verts = def.read_point_coords(params.num_nodes);
  Def::write_point_coords(exoid, verts);

  Def::close(exoid);
}

} // namespace io
} // namespace fsp
