#------------------------------------------------------------------------------#
# Copyright (c) 2021, Tiad National Security, LLC
# All rights reserved
#------------------------------------------------------------------------------#

set(burton_HEADERS
  dummy.hh
  mesh.hh
  PARENT_SCOPE
)

set(burton_SOURCES
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
