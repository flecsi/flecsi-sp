/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved.
 */

#include <flecsi/util/unit.hh>
#include <flecsi/flog.hh>

int
sanity_driver() {
  UNIT {
    flog(info) << "Hello World" << std::endl;
  };
}

flecsi::unit::driver<sanity_driver> sanity;
