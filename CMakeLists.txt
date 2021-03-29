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
      FleCSI-SP
    LINK_PUBLIC
      FleCSI::FleCSI
  )

else()

  summary_header()
  summary_option("FORMAT_ONLY" ${FORMAT_ONLY} "")

  message(STATUS ${_summary})

endif()
