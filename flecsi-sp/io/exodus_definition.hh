/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */
/**
 * Exodus definition adapted from implementation by Marc Charest.
 */
#pragma once

#include <iostream>
#include <vector>

#include <exodusII.h>

#include <flecsi/flog.hh>
#include <flecsi/data.hh>

#include "flecsi-sp/io/stream_util.hh"

namespace fsp {
namespace io {

template<int D, class T>
class exodus_base
{
public:
  using size = std::size_t;
  using real = T;
  using index = std::size_t;
  using crs = flecsi::topo::unstructured_impl::crs;
  static constexpr int num_dims = D;
  static constexpr flecsi::Dimension dimension() { return D; }
  static constexpr size CHUNK_SIZE = 256;
  using point = std::array<real, D>;

  //! An enumeration to keep track of element types
  enum class block_t {
    tri,
    quad,
    polygon,
    tet,
    hex,
    polyhedron,
    unknown,
    empty,
    invalid
  };

  using block_cursor = detail::block_cursor<size>;
  using connect_cursor = detail::connect_cursor<size>;
  using vertex_cursor = detail::vertex_cursor<size, real, D>;

  template<typename U>
  struct block_stats_t {
    U num_elem_this_blk = 0;
    U num_faces_per_elem = 0;
    U num_edges_per_elem = 0;
    U num_nodes_per_elem = 0;
    U num_attr = 0;
    char elem_type[MAX_STR_LENGTH];
  };

  exodus_base(const std::string & filename) {
    exoid = open(filename, std::ios_base::in);
    if(exoid < 0)
      flog_fatal("Problem reading exodus file");

    exo_params = read_params(exoid);
    vert_cursor = std::make_unique<vertex_cursor>(exo_params.num_nodes);

    auto num_elem_blk = exo_params.num_elem_blk;
    blk_cursor = std::make_unique<block_cursor>( num_elem_blk );

    if(is_int64(exoid))
      elem_blk_ids =
        read_block_ids<long long>(exoid, EX_ELEM_BLOCK, num_elem_blk);
    else
      elem_blk_ids = read_block_ids<int>(exoid, EX_ELEM_BLOCK, num_elem_blk);
  }

  ~exodus_base() {
    close(this->exoid);
  }

  exodus_base(const exodus_base &) = delete;
  exodus_base & operator=(const exodus_base &) = delete;

  static int open(const std::string & name, std::ios_base::openmode mode) {
#ifdef DEBUG
    ex_opts(EX_ABORT | EX_VERBOSE);
#endif

    // size of floating point variables used in app.
    int app_word_size = sizeof(real);

    if((mode & std::ios_base::in) == std::ios_base::in) {
      // size of floating point stored in name.
      int exo_word_size = 0;
      // the version number
      float version;

      // open the file
      auto exo_id = ex_open(
        name.c_str(), EX_READ, &app_word_size, &exo_word_size, &version);
      if(exo_id < 0)
        flog_fatal(
          "Problem opening exodus file, ex_open() returned " << exo_id);

      // This sets the file to read IDs as 64 bit.  If the file does not have
      // 64 bit IDs, it should have no effect.
      ex_set_int64_status(exo_id, EX_ALL_INT64_API);

      return exo_id;
    }
    else if((mode & std::ios_base::out) == std::ios_base::out) {
      // size of floating point to be stored in file.
      // change to float to save space
      int exo_word_size = sizeof(real);

      // determine the file creation mode
      int cmode = (mode & std::ios_base::app) == std::ios_base::app
                    ? EX_NOCLOBBER
                    : EX_CLOBBER;

      // create file
      auto exo_id =
        ex_create(name.c_str(), cmode, &app_word_size, &exo_word_size);
      if(exo_id < 0)
        flog_fatal(
          "Problem writing exodus file, ex_create() returned " << exo_id);

      return exo_id;
    }
    else {

      flog_fatal("Unknown file mode");
      return -1;
    }
  }

  static void close(int exo_id) {
    auto status = ex_close(exo_id);
    if(status)
      flog_fatal("Problem closing exodus file, ex_close() returned " << exo_id);
  }

  static ex_init_params read_params(int exo_id) {
    ex_init_params exo_params;
    auto status = ex_get_init_ext(exo_id, &exo_params);
    if(status) {
      flog_fatal(
        "Problem getting exodus file parameters, ex_get_init_ext() returned "
        << status);
    }

    if(num_dims != exo_params.num_dim) {
      flog_fatal("Exodus dimension mismatch: Expected dimension ("
                 << num_dims << ") != Exodus dimension (" << exo_params.num_dim
                 << ")");
    }

    return exo_params;
  }

  static void write_params(int exo_id, const ex_init_params & exo_params) {
    if(num_dims != exo_params.num_dim)
      flog_fatal("Exodus dimension mismatch: Expected dimension ("
                 << num_dims << ") /= Exodus dimension (" << exo_params.num_dim
                 << ")");

    auto status = ex_put_init_ext(exo_id, &exo_params);
    if(status)
      flog_fatal(
        "Problem putting exodus file parameters, ex_put_init_ext() returned "
        << status);
  }

  static bool is_int64(int exo_id) {
    return (ex_int64_status(exo_id) & EX_IDS_INT64_API);
  }

  template<typename U>
  static void read_block_stats(int exoid,
    ex_entity_id blk_id,
    ex_entity_type entity_type,
    block_stats_t<U> & block_stats) {
    auto status = ex_get_block(exoid,
      entity_type,
      blk_id,
      block_stats.elem_type,
      &block_stats.num_elem_this_blk,
      &block_stats.num_nodes_per_elem,
      &block_stats.num_edges_per_elem,
      &block_stats.num_faces_per_elem,
      &block_stats.num_attr);
    if(status)
      flog_fatal("Problem reading block, ex_get_block() returned " << status);
  }

  template<class U>
  static std::vector<index>
  read_block_ids(int exoid, ex_entity_type obj_type, size num_blocks) {
    using ex_index_t = U;

    std::vector<index> ids(num_blocks);

    if(num_blocks > 0) {
      std::vector<ex_index_t> block_ids(num_blocks);
      auto status = ex_get_ids(exoid, obj_type, block_ids.data());
      if(status)
        flog_fatal(
          "Problem reading block ids, ex_get_ids() returned " << status);

      // now convert them
      std::transform(
        block_ids.begin(), block_ids.end(), ids.begin(), [](auto id) {
          return id;
        });
    }

    return ids;
  }

  template<class U>
  static block_t read_block(int exoid,
    ex_entity_id blkid,
    ex_entity_type entity_type,
    block_cursor & blk_cursor,
    connect_cursor & cell_cursor) {
    using stats_t = block_stats_t<U>;
    stats_t stats;
    read_block_stats(exoid, blkid, EX_ELEM_BLOCK, stats);

    if(not stats.num_elem_this_blk)
      return block_t::empty;

    blk_cursor.init(stats.num_elem_this_blk);

    using ex_index_t = U;

    std::vector<int> counts;
    std::vector<U> indices;

    block_t ret;

    // polygon data
    if(strcasecmp("nsided", stats.elem_type) == 0) {
      // the number of nodes per element is really the number of nodes
      // in the whole block
      auto num_nodes_this_blk = stats.num_nodes_per_elem;

      // get the number of nodes per element
      counts.resize(stats.num_elem_this_blk);
      auto status = ex_get_entity_count_per_polyhedra(
        exoid, entity_type, blkid, counts.data());
      if(status)
        flog_fatal("Problem getting element node numbers, "
                   << "ex_get_entity_count_per_polyhedra() returned "
                   << status);

      // read element definitions
      indices.resize(num_nodes_this_blk);
      status = ex_get_conn(
        exoid, entity_type, blkid, indices.data(), nullptr, nullptr);
      if(status)
        flog_fatal("Problem getting element connectivity, ex_get_elem_conn() "
                   << "returned " << status);

      ret = block_t::polygon;
    }
    // polygon data
    else if(strcasecmp("nfaced", stats.elem_type) == 0) {
      // the number of faces per element is really the number of
      // faces in the whole block ( includes duplicate / overlapping
      // nodes )
      auto num_face_this_blk = stats.num_faces_per_elem;

      // get the number of nodes per element
      counts.resize(stats.num_elem_this_blk);
      auto status = ex_get_entity_count_per_polyhedra(
        exoid, entity_type, blkid, counts.data());
      if(status)
        flog_fatal("Problem reading element node info, "
                   << "ex_get_entity_count_per_polyhedra() returned "
                   << status);

      // read element definitions
      indices.resize(num_face_this_blk);
      status = ex_get_conn(
        exoid, entity_type, blkid, nullptr, nullptr, indices.data());
      if(status)
        flog_fatal("Problem getting element connectivity, ex_get_conn() "
                   << "returned " << status);

      ret = block_t::polyhedron;
    }
    // fixed element size
    else {
      U start = cell_cursor.next_in_block();
      U end = start + CHUNK_SIZE;
      end = std::min(end, stats.num_elem_this_blk);
      auto num_elem_this_blk = end - start;
      auto finished = (end == stats.num_elem_this_blk);

      // set the counts
      counts.resize(num_elem_this_blk);
      std::fill(counts.begin(), counts.end(), stats.num_nodes_per_elem);

      // read element definitions
      indices.resize(num_elem_this_blk * stats.num_nodes_per_elem);
      auto status = ex_get_partial_conn(exoid,
        EX_ELEM_BLOCK,
        blkid,
        start + 1,
        end - start,
        indices.data(),
        0,
        0);
      if(status)
        flog_fatal("Problem getting element connectivity, ex_get_elem_conn() "
                   << "returned " << status);

      // return element type
      if(strcasecmp("tri", stats.elem_type) == 0 ||
         strcasecmp("tri3", stats.elem_type) == 0)
        ret = block_t::tri;
      else if(strcasecmp("quad", stats.elem_type) == 0 ||
              strcasecmp("quad4", stats.elem_type) == 0 ||
              strcasecmp("shell", stats.elem_type) == 0 ||
              strcasecmp("shell4", stats.elem_type) == 0)
        ret = block_t::quad;
      else if(strcasecmp("tet", stats.elem_type) == 0 ||
              strcasecmp("tetra", stats.elem_type) == 0 ||
              strcasecmp("tet4", stats.elem_type) == 0)
        ret = block_t::tet;
      else if(strcasecmp("hex", stats.elem_type) == 0 ||
              strcasecmp("hex8", stats.elem_type) == 0)
        ret = block_t::hex;
      else {
        flog_fatal("Unknown block type, " << stats.elem_type);
        ret = block_t::unknown;
      }
    } // element type

    { // filter block
      bool finished_block = blk_cursor.move(counts.size());
      // increment cell_cursor base cell
      cell_cursor.move_next(counts.size(), indices.size(), finished_block);
      auto & rowptr = cell_cursor.data().rowptr;
      auto & colind = cell_cursor.data().colind;

      // create cells in mesh
      size base = 0;
      for(size e = 0; e < counts.size(); e++) {
        // get the number of nodes
        auto cnt = counts[e];
        // copy local vertices into vector
        for(int v = 0; v < cnt; v++)
          colind.emplace_back(indices[base + v] - 1);
        // add the row
        rowptr.emplace_back(rowptr.back() + cnt);
        base += cnt;
      }

      if (finished_block and blk_cursor.current() == 0) {
        // went through all blocks and are restarting.
        cell_cursor.reset();
      }
    }

    return ret;
  }

  std::pair<index, block_t> read_next_block() const {
    if(blk_cursor->current() >= elem_blk_ids.size())
      return std::make_pair(blk_cursor->current(), block_t::invalid);
    block_t blktype;

    auto blkid = current_block();

    if(is_int64(exoid))
      blktype = read_block<long long>(
        exoid, blkid, EX_ELEM_BLOCK, *blk_cursor, cell_cursor);
    else
      blktype =
        read_block<int>(exoid, blkid, EX_ELEM_BLOCK, *blk_cursor, cell_cursor);

    return std::make_pair(blkid, blktype);
  }

  void stream_cell(index cellid, std::vector<size> & ret) const {
    while(not stream_contains(cellid)) {
      block_t blktype;
      std::tie(std::ignore, blktype) = read_next_block();
      if(blktype == block_t::invalid) {
        flog_fatal("Problem finding cell while streaming");
      }
    }

    cell_cursor.get(cellid, ret);
  }

  index current_block() const {
    return elem_blk_ids[blk_cursor->current()];
  }

  bool stream_contains(index cellid) const {
    return cell_cursor.contains(cellid);
  }

  static std::vector<real> read_point_coords(int exo_id, size num_nodes) {
    if(num_nodes <= 0)
      flog_fatal(
        "Exodus file has zero nodes, or parmeters haven't been read yet.");

    // read nodes
    std::vector<real> vertex_coord(num_dims * num_nodes);

    // exodus is kind enough to fetch the data in the real type we ask for
    auto status = ex_get_coord(exo_id,
      vertex_coord.data(),
      vertex_coord.data() + num_nodes,
      vertex_coord.data() + 2 * num_nodes);

    if(status)
      flog_fatal("Problem getting vertex coordinates from exodus file, "
                 << " ex_get_coord() returned " << status);

    return vertex_coord;
  }

  std::vector<real> read_point_coords(size num_nodes) {
    return read_point_coords(exoid, num_nodes);
  }

  static void write_point_coords(int exo_id,
    const std::vector<real> & vertex_coord) {
    if(vertex_coord.empty())
      return;

    auto num_nodes = vertex_coord.size() / num_dims;

    // exodus is kind enough to fetch the data in the real type we ask for
    auto status = ex_put_coord(exo_id,
      vertex_coord.data(),
      vertex_coord.data() + num_nodes,
      vertex_coord.data() + 2 * num_nodes);

    if(status)
      flog_fatal("Problem putting vertex coordinates to exodus file, "
                 << " ex_put_coord() returned " << status);
  }

  ex_init_params get_params() const {
    return exo_params;
  }

  void read_next_vertices() {
    vert_cursor->move_next(CHUNK_SIZE);
    auto [beg, end] = vert_cursor->interval();
    auto num_nodes = end - beg;
    auto status = ex_get_partial_coord(exoid,
      beg + 1,
      num_nodes,
      vert_cursor->data(),
      vert_cursor->data() + num_nodes,
      vert_cursor->data() + 2 * num_nodes);

    if(status)
      flog_fatal("Problem getting vertex coordinates from exodus file, "
                 << " ex_get_partial_coord() returned " << status);
  }

  point vertex(index vertexid) {
    while(not vert_cursor->contains(vertexid)) {
      read_next_vertices();
    }
    auto [beg, end] = vert_cursor->interval();
    auto num_nodes = end - beg;
    const auto & vertices = vert_cursor->current();
    point vert;
    for(int i = 0; i < num_dims; i++) {
      vert[i] = vertices[i * num_nodes + (vertexid - beg)];
    }

    return vert;
  }

protected:
  int exoid;
  ex_init_params exo_params;
  std::vector<index> elem_blk_ids;
  bool int64;
  mutable std::unique_ptr<block_cursor> blk_cursor;
  mutable connect_cursor cell_cursor;
  mutable std::unique_ptr<vertex_cursor> vert_cursor;
};

template<int D, class T>
class exodus_definition;

template<class T>
class exodus_definition<2, T> : public exodus_base<2, T>
{
public:
  using base = exodus_base<2, T>;
  using real = typename base::real;
  using size = typename base::size;
  using index = typename base::index;
  using crs = typename base::crs;
  using base::elem_blk_ids;
  using base::exo_params;
  using base::exoid;
  using base::num_dims;

  exodus_definition(const std::string & filename) : base(filename) {}

  size num_entities(int dim) const {
    if(dim > num_dims)
      flog_fatal("num_entities: " << dim << " > num_dims");

    if(dim == 2)
      return exo_params.num_elem;
    else if(dim == 0)
      return exo_params.num_nodes;
    else
      flog_fatal("num_entities: dim: " << dim << " not implemented.");
  }

  std::vector<size> entities(int from_dim, int to_dim, size from_id) const {
    std::vector<size> ret;
    if(from_dim == num_dims and to_dim == 0) {
      this->stream_cell(from_id, ret);
    }
    return ret;
  }

  static void build_intermediary_from_vertices(flecsi::Dimension dim,
                                               const std::vector<flecsi::Dimension> & verts,
                                               crs & inter)
  {
    flog_assert(dim == 1, "Invalid dimension: " << dim);
    for (auto v0 = verts.begin(), v1 = std::next(v0);
         v0 != verts.end();
         ++v0, ++ v1) {
      if (v1 == verts.end())
        v1 = verts.begin();
      inter.add_row({*v0, *v1});
    }
  }
};

template<class T>
class exodus_definition<3, T> : public exodus_base<3, T>
{
public:
  using base = exodus_base<3, T>;
  using real = typename base::real;
  using size = typename base::size;
  using index = typename base::index;
  using base::elem_blk_ids;
  using base::exo_params;
  using base::exoid;
  using base::num_dims;

  exodus_definition(const std::string & filename) : base(filename) {}

  size num_entities(int dim) const {
    if(dim > num_dims)
      flog_fatal("num_entities: " << dim << " > num_dims");

    if(dim == 3)
      return exo_params.num_elem;
    else if(dim == 0)
      return exo_params.num_nodes;
    else
      flog_fatal("num_entities: dim: " << dim << " not implemented.");
  }

  std::vector<size> entities(int from_dim, int to_dim, size from_id) const {
    std::vector<size> ret;
    if(from_dim == num_dims and to_dim == 0) {
      this->stream_cell(from_id, ret);
    }
    return ret;
  }
};

} // namespace io
} // namespace fsp
