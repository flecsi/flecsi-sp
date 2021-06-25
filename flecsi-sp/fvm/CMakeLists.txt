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

if(ENABLE_EXODUSII)
add_unit(coloring
  SOURCES
    test/coloring.cc
  LIBRARIES
    FleCSI-SP
  INPUTS
    ${PROJECT_SOURCE_DIR}/data/meshes/2d/mixed/mixed.exo
    ${PROJECT_SOURCE_DIR}/data/meshes/2d/voro/voro.g
    ${PROJECT_SOURCE_DIR}/data/meshes/2d/square/square_32x32.g
  THREADS 4)
endif()
