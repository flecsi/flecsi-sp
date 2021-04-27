#------------------------------------------------------------------------------#
# Copyright (c) 2021, Tiad National Security, LLC
# All rights reserved
#------------------------------------------------------------------------------#

set(fvm_HEADERS
  config.hh
  mesh.hh
  PARENT_SCOPE
)

set(fvm_SOURCES
  config.cc
  PARENT_SCOPE
)

#------------------------------------------------------------------------------#
# Unit tests.
#------------------------------------------------------------------------------#

add_unit(sanity
  SOURCES
    test/sanity.cc
  LIBRARIES
    FleCSI-SP
)
