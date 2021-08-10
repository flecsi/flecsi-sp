/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved.
 */

#include <flecsi/flog.hh>
#include <flecsi/util/unit.hh>
#include <flecsi/util/parmetis.hh>


#include "flecsi-sp/io/exodus_definition.hh"

using namespace flecsi;
using namespace fsp;
using flecsi::topo::unstructured_impl::crs;


static int check_cell(std::size_t cellid, std::map<std::size_t, std::vector<std::size_t>> bless,
                      const std::vector<std::size_t> & p2m,
                      const crs & c2e, const crs & e2v, const std::map<std::vector<std::size_t>, std::size_t> & v2e)
{
  UNIT {
    bool owns_cell = false;
    for (std::size_t row{0}; row < c2e.offsets.size() - 1; row++) {
      if (p2m[row] == cellid) {
        owns_cell = true;
        std::vector<std::size_t> edges;
        for (std::size_t off{c2e.offsets[row]}; off < c2e.offsets[row+1]; off++) {
          edges.push_back(c2e.indices[off]);
        }
        std::vector<std::size_t> bless_edges;
        for (const auto & [key, _] : bless) bless_edges.push_back(key);
        std::sort(edges.begin(), edges.end());
        EXPECT_EQ(edges, bless_edges);
      }
    }

    if (owns_cell) {
      // check e2v and v2e
      for (std::size_t row{0}; row < e2v.offsets.size() - 1; row++) {
        auto edge = bless.find(row);
        if (edge != bless.end()) {
          std::vector<std::size_t> verts;
          for (std::size_t off{e2v.offsets[row]}; off < e2v.offsets[row+1]; off++) {
            verts.push_back(e2v.indices[off]);
          }
          std::sort(verts.begin(), verts.end());
          EXPECT_EQ(verts, edge->second);
          EXPECT_EQ(v2e.at(verts), edge->first);
        }
      }
    }
  };
}


static int inter2d()
{
  UNIT {
    // voro
    {
      io::exodus_definition<2, double> def("voro.g");
      const auto colors = processes();
      auto [naive, c2v, v2c, c2c] = topo::unstructured_impl::make_dcrs(def, 1);
      auto raw = util::parmetis::color(naive, colors);

      auto [primaries, p2m, m2p] =
        topo::unstructured_impl::migrate(naive, colors, raw, c2v, v2c, c2c);

      topo::unstructured_impl::coloring_definition cd{colors, 0, 2, 1, 1, {{}}};
      auto colorings = topo::unstructured_impl::color(
        def, cd, raw, primaries, c2v, v2c, c2c, m2p, p2m);

      auto [c2e, e2v, v2e] =
        topo::unstructured_impl::build_intermediary<2>(1, def, c2v, p2m);

      EXPECT_EQ(check_cell(145, {
            {207, {306, 307}},
            {209, {307, 308}},
            {212, {308, 310}},
            {227, {328, 329}},
            {230, {328, 330}},
            {232, {306, 330}},
            {235, {310, 329}}
          }, p2m, c2e, e2v, v2e),
        0);

      EXPECT_EQ(check_cell(240, {
            {81,  {437, 438}},
            {106, {438, 466}},
            {179, {466, 545}},
            {181, {545, 552}},
            {182, {437, 552}}
          }, p2m, c2e, e2v, v2e),
        0);
    }
    // mixed
    {
      io::exodus_definition<2, double> def("mixed.exo");
      const auto colors = processes();
      auto [naive, c2v, v2c, c2c] = topo::unstructured_impl::make_dcrs(def, 1);
      auto raw = util::parmetis::color(naive, colors);

      auto [primaries, p2m, m2p] =
        topo::unstructured_impl::migrate(naive, colors, raw, c2v, v2c, c2c);

      topo::unstructured_impl::coloring_definition cd{colors, 0, 2, 1, 1, {{}}};
      auto colorings = topo::unstructured_impl::color(
        def, cd, raw, primaries, c2v, v2c, c2c, m2p, p2m);

      auto [c2e, e2v, v2e] =
        topo::unstructured_impl::build_intermediary<2>(1, def, c2v, p2m);

      EXPECT_EQ(check_cell(166, {
            {39, {212, 213}},
            {40, {213, 214}},
            {41, {212, 214}}
          }, p2m, c2e, e2v, v2e),
        0);

      EXPECT_EQ(check_cell(136, {
            {122, {156, 157}},
            {149, {156, 176}},
            {151, {157, 177}},
            {152, {176, 177}}
          }, p2m, c2e, e2v, v2e),
        0);
    }
  };
}


static int inter3d()
{
  UNIT {
    io::exodus_definition<3, double> def("box-hex.exo");
    const auto colors = processes();
    auto [naive, c2v, v2c, c2c] = topo::unstructured_impl::make_dcrs(def, 2);
    auto raw = util::parmetis::color(naive, colors);

    auto [primaries, p2m, m2p] =
      topo::unstructured_impl::migrate(naive, colors, raw, c2v, v2c, c2c);

    topo::unstructured_impl::coloring_definition cd{colors, 0, 3, 1, 1, {{}}};
    auto colorings = topo::unstructured_impl::color(
      def, cd, raw, primaries, c2v, v2c, c2c, m2p, p2m);

    auto [c2f, f2v, v2f] =
      topo::unstructured_impl::build_intermediary<3>(2, def, c2v, p2m);
    std::vector<std::vector<std::size_t>> f2v_vec;
    for (std::size_t row = 0; row < f2v.offsets.size() - 1; row++) {
      std::vector<std::size_t> verts;
      for (std::size_t i = f2v.offsets[row]; i < f2v.offsets[row+1]; i++) {
        verts.push_back(f2v.indices[i]);
      }
      f2v_vec.push_back(std::move(verts));
    }

    auto [f2e, e2v, v2e] =
      topo::unstructured_impl::build_intermediary<2>(1, def, f2v_vec, p2m);

    auto c2e = topo::unstructured_impl::intersect_connectivity(c2f, f2e);

    // check faces
    EXPECT_EQ(check_cell(29, {
          {29, {32, 35, 42, 45}},
          {45, {32, 35, 65, 67}},
          {55, {32, 42, 65, 70}},
          {59, {35, 45, 67, 72}},
          {60, {42, 45, 70, 72}},
          {61, {65, 67, 70, 72}}
        }, p2m, c2f, f2v, v2f),
      0);
  };
}

int driver() {
  UNIT {
    // EXPECT_EQ((test<inter2d, mpi>()), 0);
    EXPECT_EQ((test<inter3d, mpi>()), 0);
  };
}

flecsi::unit::driver<driver> inter_driver;
