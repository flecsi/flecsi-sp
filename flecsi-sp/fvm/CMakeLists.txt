#------------------------------------------------------------------------------#
# Copyright (c) 2021, Tiad National Security, LLC
# All rights reserved
#------------------------------------------------------------------------------#

set(fvm_HEADERS
  dummy.hh
  mesh.hh
  PARENT_SCOPE
)

set(fvm_SOURCES
  dummy.cc
  PARENT_SCOPE
)

#------------------------------------------------------------------------------#
# Unit tests.
#------------------------------------------------------------------------------#

add_unit(sanity
  SOURCES
    test/sanity.cc
)
