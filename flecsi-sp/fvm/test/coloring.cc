/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved.
 */

#include <flecsi/data.hh>
#include <flecsi/flog.hh>
#include <flecsi/util/parmetis.hh>
#include <flecsi/util/unit.hh>

#include "flecsi-sp/fvm/mesh.hh"
#include "flecsi-sp/io/debug_util.hh"
#include "flecsi-sp/io/exodus_definition.hh"

using namespace flecsi;
using namespace fsp;

static std::vector<std::size_t>
gather_raw(std::vector<std::size_t> raw) {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  auto displs = std::make_unique<int[]>(size);
  auto rcounts = std::make_unique<int[]>(size);
  [rank, size](int * displs, int * counts, int lsize) {
    MPI_Gather(&lsize, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    displs[0] = 0;
    for(int i = 1; i < size; i++) {
      displs[i] = displs[i - 1] + counts[i - 1];
    }
  }(displs.get(), rcounts.get(), raw.size());
  std::vector<std::size_t> full_raw(
    displs.get()[size - 1] + rcounts.get()[size - 1]);
  const auto typ = util::mpi::type<std::size_t>();
  MPI_Gatherv(raw.data(),
    raw.size(),
    typ,
    full_raw.data(),
    rcounts.get(),
    displs.get(),
    typ,
    0,
    MPI_COMM_WORLD);

  return full_raw;
}

template<unsigned short ND>
topo::unstructured_impl::coloring_definition
create_coloring_def(const Color colors) {
  if constexpr(ND == 2)
    return {colors, 0, 2, 1, 1, {{}}};
  else
    return {colors, 0, 3, 1, 1, {{}}};
}

template<unsigned short ND>
int
compute_coloring(const char * fname) {
  UNIT {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    io::exodus_definition<ND, double> def(fname);

    const Color colors = size;
    auto [naive, c2v, v2c, c2c] =
      topo::unstructured_impl::make_dcrs(def, ND - 1);
    auto raw = util::parmetis::color(naive, colors);

    auto [primaries, p2m, m2p] =
      topo::unstructured_impl::migrate(naive, colors, raw, c2v, v2c, c2c);

    auto cd = create_coloring_def<ND>(colors);
    auto colorings = topo::unstructured_impl::color(
      def, cd, raw, primaries, c2v, v2c, c2c, m2p, p2m);

    if constexpr(ND == 3) {

      auto [c2f, f2v, v2f] =
        topo::unstructured_impl::build_intermediary<3>(2, def, c2v, p2m);
      std::vector<std::vector<std::size_t>> f2v_vec;
      for(std::size_t row = 0; row < f2v.offsets.size() - 1; row++) {
        std::vector<std::size_t> verts;
        for(std::size_t i = f2v.offsets[row]; i < f2v.offsets[row + 1]; i++) {
          verts.push_back(f2v.indices[i]);
        }
        f2v_vec.push_back(std::move(verts));
      }

      auto [f2e, e2v, v2e] =
        topo::unstructured_impl::build_intermediary<2>(1, def, f2v_vec, p2m);

      auto c2e = topo::unstructured_impl::intersect_connectivity(c2f, f2e);
    }
    else { // ND == 2
      auto [c2e, e2v, v2e] =
        topo::unstructured_impl::build_intermediary<2>(1, def, c2v, p2m);
    }

    auto full_raw = gather_raw({raw.begin(), raw.end()});

    if(rank == 0) {
      std::string prefix{fname};
      std::string ofname =
        prefix.substr(0, prefix.find(".", 0)) + "-coloring.exo";
      io::write_coloring(ofname.c_str(), def, full_raw);
    }
  };
}
int
coloring_driver() {
  UNIT {
    execute<compute_coloring<3>, mpi>("box-hex.exo");
    execute<compute_coloring<2>, mpi>("voro.g");
  };
}

flecsi::unit::driver<coloring_driver> coloring;
