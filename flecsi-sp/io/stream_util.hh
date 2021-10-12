/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */

#pragma once

#include <functional>
#include <vector>

#include "flecsi/topo/unstructured/types.hh"

namespace fsp {
namespace io {
namespace detail {

using crs = flecsi::topo::unstructured_impl::crs;

template<class size, class block_t, auto is_fixed_block>
struct block_cursor {
  struct location {
    int block;
    size offset;
  };

  block_cursor(std::vector<std::pair<size, block_t>> && entities_per_block,
    size chunk_size)
    : base(0), block_counts(std::move(entities_per_block)),
      chunk_size(chunk_size) {}

  /**
   * check whether entity (id) is contained in current chunk.
   */
  bool contains(size id) const {
    return id >= base and id < next();
  }

  /**
   * Get id of the next entity to read.
   */
  size next() const {
    return (curr.offsets.size() == 0) ? 0 : base + curr.offsets.size() - 1;
  }

  void move(int block, size offset) {
    curr_loc = {block, offset};
    base = 0;
    for(int i = 0; i < block; i++) {
      base += block_counts[i].first;
    }
    base += offset;

    curr.offsets.clear();
    curr.indices.clear();
  }

  void alloc(size nrows, size nnz) {
    curr.offsets.reserve(nrows + 1);
    curr.indices.reserve(nnz);
    curr.offsets.emplace_back(0);
  }

  location find_entity(size eid) {
    size curr = 0;
    location ret{0, 0};
    for(const auto & blkinfo : block_counts) {
      if(not is_fixed_block(blkinfo.second)) { // polyhedra
        curr += blkinfo.first;
        if(eid < curr)
          return ret;
        ++ret.block;
      }
      else { // fixed element so read by chunks
        if(eid < curr + blkinfo.first) { // first check if in block
          long long entities_remaining = blkinfo.first;
          ret.offset = 0;
          while(entities_remaining > 0) {
            entities_remaining -= chunk_size;
            if(eid < curr + ret.offset + chunk_size)
              return ret;
            ret.offset += chunk_size;
          }
        }
        curr += blkinfo.first;
        ++ret.block;
      }
    }

    // did not find entity (eid)
    ret.block = -1;
    ret.offset = -1;
    return ret;
  }

  void get(size id, std::vector<size> & ret) {
    if(contains(id)) {
      size beg = curr.offsets[id - base];
      size end = curr.offsets[id - base + 1];
      ret.resize(end - beg);
      std::copy(
        curr.indices.begin() + beg, curr.indices.begin() + end, ret.begin());
    }
  }

  crs & data() {
    return curr;
  }

  const location & current_location() const {
    return curr_loc;
  }

  block_t get_block_type(size id) const {
    auto & binfo = block_counts.at(id);
    return binfo.second;
  }

  size get_base() const {
    return base;
  }

protected:
  size base;
  std::vector<std::pair<size, block_t>> block_counts;
  size chunk_size;
  crs curr;
  location curr_loc;
};

template<class size_t, class real, int num_dims>
class vertex_cursor
{
public:
  /**
   * \param num_verts global number of vertices to stream
   */
  vertex_cursor(size_t num_verts) : base(0), num_verts(num_verts) {}

  /**
   * check whether entity (id) is contained in current chunk.
   */
  bool contains(size_t id) const {
    return id >= base and id < next();
  }

  /**
   * Get id of the next entity to read.
   */
  size_t next() const {
    return (size() == 0) ? 0 : base + size();
  }

  size_t size() const {
    return curr.size() / num_dims;
  }

  real * data() {
    return curr.data();
  }

  const std::vector<real> & current() const {
    return curr;
  }

  /**
   * get interval [begin, end) of entities in curr.
   */
  std::pair<size_t, size_t> interval() const {
    return std::make_pair(base, base + size());
  }

  /**
   * Move cursor (prepare for inserting another chunk of entities).
   */
  void move_next(size_t chunksize) {
    base = next();
    if(base >= num_verts)
      base = 0;
    curr.clear();
    chunksize = std::min(chunksize, num_verts - base);
    curr.resize(chunksize * num_dims);
  }

protected:
  size_t base;
  size_t num_verts;
  std::vector<real> curr;
};

} // namespace detail
} // namespace io
} // namespace fsp
