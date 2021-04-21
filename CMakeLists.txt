#------------------------------------------------------------------------------#
# Copyright (c) 2021, Tiad National Security, LLC
# All rights reserved
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.12...3.19)

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
# Add flecsi module path.
#------------------------------------------------------------------------------#

find_package(FleCSICMake REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${FleCSICMake_PREFIX})

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

  include(documentation)

  #----------------------------------------------------------------------------#
  # Find FleCSI installationn.
  #----------------------------------------------------------------------------#

  find_package(FleCSI REQUIRED)

  #----------------------------------------------------------------------------#
  # Unit tests and logging.
  #----------------------------------------------------------------------------#

  set(ENABLE_FLOG ${FleCSI_ENABLE_FLOG})
  set(FLECSI_RUNTIME_MODEL ${FleCSI_RUNTIME_MODEL})

  include(flog)
  set(FLECSI_UNIT_MAIN ${FleCSICMake_PREFIX}/unit-main.cc)
  include(unit)

  configure_file(${PROJECT_SOURCE_DIR}/config/flecsi-sp-config.h.in
    ${CMAKE_BINARY_DIR}/flecsi-sp-config.h @ONLY)

  install(
    FILES ${CMAKE_BINARY_DIR}/flecsi-sp-config.h
    DESTINATION include
  )

  #----------------------------------------------------------------------------#
  # FleCSI-SP library.
  #----------------------------------------------------------------------------#

  include(library)

  add_library_target(FleCSI-SP flecsi-sp
    EXPORT_TARGET
      FleCSI-SPTargets
    VERSION
      ${FleCSI_SP_VERSION}
    NAMESPACE
      FleCSI
    INCLUDE_PRIVATE
      ${CMAKE_BINARY_DIR}
    LINK_PUBLIC
      FleCSI::FleCSI
  )

  #----------------------------------------------------------------------------#
  # Config file.
  #----------------------------------------------------------------------------#

  configure_file(${PROJECT_SOURCE_DIR}/config/FleCSI-SPConfig.cmake.in
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FleCSI-SPConfig.cmake @ONLY)

  install(
    FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FleCSI-SPConfig.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR}/cmake/FleCSI-SP
  )

  #----------------------------------------------------------------------------#
  # Output configuration summary.
  #----------------------------------------------------------------------------#

  summary_header()
  summary_info("CMAKE_BUILD_TYPE" "${CMAKE_BUILD_TYPE}" TRUE)
  summary_info("CMAKE_INSTALL_PREFIX" "${CMAKE_INSTALL_PREFIX}" TRUE)
  string(APPEND _summary "\n")
  summary_info("CMAKE_CXX_COMPILER" "${CMAKE_CXX_COMPILER}" TRUE)
  summary_info("CMAKE_CXX_FLAGS" "${CMAKE_CXX_FLAGS}" TRUE)
  summary_info("CMAKE_C_COMPILER" "${CMAKE_C_COMPILER}" TRUE)
  summary_info("CMAKE_C_FLAGS" "${CMAKE_C_FLAGS}" TRUE)
  if(ClangFormat_FOUND)
    summary_info("ClangFormat_EXECUTABLE" "${ClangFormat_EXECUTABLE}" TRUE)
  endif()
  string(APPEND _summary "\n")
  summary_info("FleCSI_SP_VERSION" "${FleCSI_SP_VERSION}" TRUE)
  summary_info("FleCSI_SP_DOCUMENTATION_VERSION"
    "${FleCSI-SP_DOCUMENTATION_VERSION}" FALSE)
  if(${PROJECT_NAME}_COMMITS)
    summary_info("Repository state" "${${PROJECT_NAME}_COMMITS}" FALSE)
  endif()
  summary_option("ENABLE_FLOG" ${ENABLE_FLOG}
    " (FLOG_STRIP_LEVEL ${FLOG_STRIP_LEVEL})")
  summary_option("ENABLE_UNIT_TESTS" ${ENABLE_UNIT_TESTS} "")

  message(STATUS ${_summary})

else()

  summary_header()
  summary_option("FORMAT_ONLY" ${FORMAT_ONLY} "")

  message(STATUS ${_summary})

endif()
