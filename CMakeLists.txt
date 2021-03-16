#------------------------------------------------------------------------------#
# Copyright (c) 2021, Tiad National Security, LLC
# All rights reserved
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.12)

#------------------------------------------------------------------------------#
# Add local module path.
#------------------------------------------------------------------------------#

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#------------------------------------------------------------------------------#
# Project.
#------------------------------------------------------------------------------#

project(FleCSI-SP LANGUAGES CXX C)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

#------------------------------------------------------------------------------#
# Compiler version requirements.
#------------------------------------------------------------------------------#

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    message(FATAL_ERROR "Version 9.0 of gnu compilers required!")
  endif()
endif()

#------------------------------------------------------------------------------#
# Version.
#------------------------------------------------------------------------------#

include(version)

set(FleCSI_SP_VERSION ${${PROJECT_NAME}_VERSION})

#------------------------------------------------------------------------------#
# Add summary macros.
#------------------------------------------------------------------------------#

include(summary)

#------------------------------------------------------------------------------#
# Add format target.
#------------------------------------------------------------------------------#

include(format)

if(NOT FORMAT_ONLY)

  #----------------------------------------------------------------------------#
  # FleCSI-SP library.
  #----------------------------------------------------------------------------#

  include(library)

  add_library_target(FleCSI-SP flecsi-sp EXPORT_TARGET FleCSI-SPTargets)

  #----------------------------------------------------------------------------#
  #----------------------------------------------------------------------------#

  set(FLECSI_SP_CMAKE_DIR ${CMAKE_INSTALL_PREFIX}/${LIBDIR}/cmake/FleCSI-SP)

  #----------------------------------------------------------------------------#
  # Export targets and package.
  #----------------------------------------------------------------------------#

  export(
    TARGETS FleCSI-SP
    FILE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FleCSI-SPTargets.cmake
  )

  export(PACKAGE FleCSI-SP)

  #----------------------------------------------------------------------------#
  # CMake config file: This should be the last thing to happen.
  #----------------------------------------------------------------------------#

  configure_file(${PROJECT_SOURCE_DIR}/config/FleCSI-SPConfig.cmake.in
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FleCSI-SPConfig.cmake @ONLY)

  install(
    FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FleCSI-SPConfig.cmake
    DESTINATION ${FLECSI_SP_CMAKE_DIR}
  )

  install(
    EXPORT FleCSI-SPTargets
    DESTINATION ${FLECSI_SP_CMAKE_DIR}
    COMPONENT dev
  )

else()

  summary_header()
  summary_option("FORMAT_ONLY" ${FORMAT_ONLY} "")

  message(STATUS ${_summary})

endif()
