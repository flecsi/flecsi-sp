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

int
compute_coloring() {
  UNIT {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // io::exodus_definition<2, double> def("square_32x32.g");
    // io::exodus_definition<2, double> def("mixed.exo");
    io::exodus_definition<2, double> def("voro.g");

    const Color colors = size;
    auto [naive, c2v, v2c, c2c] = topo::unstructured_impl::make_dcrs(def, 1);
    auto raw = util::parmetis::color(naive, colors);

    auto [primaries, p2m, m2p] =
      topo::unstructured_impl::migrate(naive, colors, raw, c2v, v2c, c2c);

    topo::unstructured_impl::coloring_definition cd{colors, 0, 2, 1, 1, {{}}};
    auto colorings = topo::unstructured_impl::color(
      def, cd, raw, primaries, c2v, v2c, c2c, m2p, p2m);

    auto [c2e, e2v, v2e] =
      topo::unstructured_impl::build_intermediary<decltype(def)>(1, c2v);

#if 0
    { // debug
      if (rank == 1) {
        std::size_t lid{0};
        for (auto const & c: c2v) {
          std::cout << "cell: " << p2m[lid++] << ": ";
          for (const auto & v : c) {
            std::cout << v << " ";
          }
          std::cout << std::endl;
        }

        std::ofstream cfile("c2e.txt");
        std::ofstream efile("e2v.txt");
        std::ofstream vfile("v2e.txt");

        for (std::size_t row{0}; row < c2e.offsets.size() - 1; row++) {
          cfile << p2m[row] << " => ";
          for (std::size_t off{c2e.offsets[row]}; off < c2e.offsets[row+1]; off++) {
            cfile << c2e.indices[off] << " ";
          }
          cfile << '\n';
        }

        for (std::size_t row{0}; row < e2v.offsets.size() - 1; row++) {
          efile << row << " -> ";
          for (std::size_t off{e2v.offsets[row]}; off < e2v.offsets[row+1]; off++) {
            efile << e2v.indices[off] << ' ';
          }
          efile << '\n';
        }

        for (const auto & el : v2e) {
          vfile << el.first[0] << " " << el.first[1] << " => ";
          vfile << el.second << '\n';
        }
        cfile.close();
        efile.close();
        vfile.close();
      }
    }
#endif

    auto full_raw = gather_raw({raw.begin(), raw.end()});

    if(rank == 0)
      io::write_coloring("coloring.exo", def, full_raw);
  };
}
int
coloring_driver() {
  UNIT { execute<compute_coloring, mpi>(); };
}

flecsi::unit::driver<coloring_driver> coloring;
