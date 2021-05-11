/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved.
 */

#include <flecsi/flog.hh>
#include <flecsi/util/unit.hh>

#include "flecsi-sp/io/exodus_definition.hh"

using namespace flecsi;

int
def2d() {
  UNIT {
    {
      fsp::io::exodus_definition<2, double> def("square_32x32.g");
      ASSERT_EQ(def.num_entities(2), 1024);
      ASSERT_EQ(def.num_entities(0), 1089);
      ASSERT_EQ(def.entities(2, 0, 1), std::vector<std::size_t>({1, 4, 5, 2}));
      ASSERT_EQ(def.entities(2, 0, 914),
        std::vector<std::size_t>({942, 943, 976, 975}));
      ASSERT_EQ(def.entities(2, 0, 1023),
        std::vector<std::size_t>({1054, 1055, 1088, 1087}));
    }
    {
      fsp::io::exodus_definition<2, double> def("voro.g");
      ASSERT_EQ(def.num_entities(2), 305);
      ASSERT_EQ(def.num_entities(0), 612);
      ASSERT_EQ(
        def.entities(2, 0, 0), std::vector<std::size_t>({4, 5, 6, 7, 8, 9}));
      ASSERT_EQ(def.entities(2, 0, 76),
        std::vector<std::size_t>({104, 221, 222, 223, 224, 219, 105}));
      ASSERT_EQ(def.entities(2, 0, 287),
        std::vector<std::size_t>({602, 603, 580, 579}));
    }
    {
      fsp::io::exodus_definition<2, double> def("mixed.exo");
      ASSERT_EQ(def.num_entities(2), 301);
      ASSERT_EQ(def.num_entities(0), 255);
      ASSERT_EQ(
        def.entities(2, 0, 13), std::vector<std::size_t>({26, 28, 29, 27}));
      ASSERT_EQ(def.entities(2, 0, 132),
        std::vector<std::size_t>({152, 153, 173, 164}));
      ASSERT_EQ(
        def.entities(2, 0, 263), std::vector<std::size_t>({221, 251, 252}));
      ASSERT_EQ(
        def.entities(2, 0, 211), std::vector<std::size_t>({28, 26, 222}));
    }
  };
}

int
def3d() {
  UNIT {
    {
      fsp::io::exodus_definition<3, double> def("box-hex.exo");
      ASSERT_EQ(def.num_entities(3), 64);
      ASSERT_EQ(def.num_entities(0), 125);
      ASSERT_EQ(def.entities(3, 0, 3),
        std::vector<std::size_t>({12, 16, 17, 13, 14, 18, 19, 15}));
      ASSERT_EQ(def.entities(3, 0, 7),
        std::vector<std::size_t>({13, 17, 28, 26, 15, 19, 29, 27}));
      ASSERT_EQ(def.entities(3, 0, 31),
        std::vector<std::size_t>({37, 39, 49, 47, 68, 69, 74, 73}));
    }
    {
      fsp::io::exodus_definition<3, double> def("box-tet.exo");
      ASSERT_EQ(def.num_entities(3), 531);
      ASSERT_EQ(def.num_entities(0), 156);
      ASSERT_EQ(
        def.entities(3, 0, 92), std::vector<std::size_t>({72, 19, 15, 16}));
      ASSERT_EQ(
        def.entities(3, 0, 380), std::vector<std::size_t>({107, 135, 96, 100}));
      ASSERT_EQ(def.entities(3, 0, 508),
        std::vector<std::size_t>({152, 138, 150, 151}));
    }
  };
}

int
driver() {
  UNIT {
    EXPECT_EQ(def2d(), 0);
    EXPECT_EQ(def3d(), 0);
  };
}

unit::driver<driver> drv;
