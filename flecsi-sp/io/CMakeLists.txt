#------------------------------------------------------------------------------#
# Copyright (c) 2021, Tiad National Security, LLC
# All rights reserved
#------------------------------------------------------------------------------#

if(ENABLE_EXODUSII)
set(io_HEADERS
  stream_util.hh
  debug_util.hh
  exodus_definition.hh
  PARENT_SCOPE)

set(io_SOURCES
  debug_util.cc
  exodus_definition.cc
  PARENT_SCOPE)

#------------------------------------------------------------------------------#
# Unit tests.
#------------------------------------------------------------------------------#
add_unit(exodus
  SOURCES
    test/exodus.cc
  LIBRARIES
    FleCSI-SP
  INPUTS
   ${PROJECT_SOURCE_DIR}/data/meshes/2d/mixed/mixed.exo
   ${PROJECT_SOURCE_DIR}/data/meshes/2d/voro/voro.g
   ${PROJECT_SOURCE_DIR}/data/meshes/2d/square/square_32x32.g
   ${PROJECT_SOURCE_DIR}/data/meshes/3d/box/box-hex.exo
   ${PROJECT_SOURCE_DIR}/data/meshes/3d/box/box-tet.exo
)

add_unit(exodus_intermediary
  SOURCES
    test/exodus_inter.cc
  LIBRARIES
    FleCSI-SP
  INPUTS
    ${PROJECT_SOURCE_DIR}/data/meshes/2d/voro/voro.g
    ${PROJECT_SOURCE_DIR}/data/meshes/2d/mixed/mixed.exo
  THREADS 4
)

endif()
