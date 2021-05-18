/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */

#pragma once

#include <vector>

namespace fsp {
namespace io {
namespace detail {

template<class size>
struct csr {
  std::vector<size> rowptr;
  std::vector<size> colind;
};

template<class size>
struct block_cursor {
  block_cursor() : curr_block(0), entities_left(0), block_initialized(false) {}

  void init(size num_entities) {
    if(not block_initialized) {
      entities_left = num_entities;
      block_initialized = true;
    }
  }

  bool move(size num_new) {
    entities_left -= num_new;
    if(entities_left <= 0) {
      ++curr_block;
      block_initialized = false;
      return true;
    }
    return false;
  }

  size current() const {
    return curr_block;
  }

protected:
  size curr_block;
  size entities_left;
  bool block_initialized;
};

template<class size>
struct entity_cursor {
  entity_cursor() : base(0), block_base(0), new_block(true) {}

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
    return (curr.rowptr.size() == 0) ? 0 : base + curr.rowptr.size() - 1;
  }

  /**
   * Get id (within block) of the next entity to read.
   */
  size next_in_block() const {
    return (new_block or (curr.rowptr.size() == 0))
             ? 0
             : block_base + curr.rowptr.size() - 1;
  }

  /**
   * Move cursor (prepare for inserting another chunk of entities).
   */
  void move_next(size nrows, size nnz, bool finished_block) {
    base = next();
    new_block = finished_block;
    block_base = next_in_block();
    curr.rowptr.clear();
    curr.colind.clear();
    curr.rowptr.reserve(nrows + 1);
    curr.colind.reserve(nnz);
    curr.rowptr.emplace_back(0);
  }

  void get(size id, std::vector<size> & ret) {
    if(contains(id)) {
      size beg = curr.rowptr[id - base];
      size end = curr.rowptr[id - base + 1];
      ret.resize(end - beg);
      std::copy(
        curr.colind.begin() + beg, curr.colind.begin() + end, ret.begin());
    }
  }

  csr<size> & data() {
    return curr;
  }

protected:
  size base; /** global id of first cell in curr */
  size block_base; /** block id of first cell in curr */
  bool new_block;
  csr<size> curr;
};

} // namespace detail
} // namespace io
} // namespace fsp
