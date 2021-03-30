/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved.
 */

#include <flecsi/flog.hh>
#include <flecsi/util/unit.hh>

int
sanity_driver() {
  UNIT { flog(info) << "Hello World" << std::endl; };
}

flecsi::unit::driver<sanity_driver> sanity;
