#------------------------------------------------------------------------------#
#  @@@@@@@@  @@           @@@@@@   @@@@@@@@ @@
# /@@/////  /@@          @@////@@ @@////// /@@
# /@@       /@@  @@@@@  @@    // /@@       /@@
# /@@@@@@@  /@@ @@///@@/@@       /@@@@@@@@@/@@
# /@@////   /@@/@@@@@@@/@@       ////////@@/@@
# /@@       /@@/@@//// //@@    @@       /@@/@@
# /@@       @@@//@@@@@@ //@@@@@@  @@@@@@@@ /@@
# //       ///  //////   //////  ////////  //
#
# Copyright (c) 2016, Triad National Security, LLC
# All rights reserved
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(engine)
include(image)

project(FleCSI-CI LANGUAGES NONE)

#------------------------------------------------------------------------------#
# Options
#------------------------------------------------------------------------------#

set(SPACK_REPOSITORY "https://github.com/spack/spack.git" CACHE STRING
  "Set the Spack repository")
set(SPACK_BRANCH "develop" CACHE STRING "Set the Spack branch")
set(SPACK_HASH "HEAD" CACHE STRING "Set the Spack hash")

set(FLeCSI_REPOSITORY "https://gitlab.lanl.gov/flecsi/flecsi.git" CACHE STRING
	"Set the FLeCSI repository")

set(TARGET_BRANCH "devel" CACHE STRING
  "Set the target branch (branch subdir used for configuration)")

set(SOURCE_REPOSITORY "https://gitlab.lanl.gov/flecsi/flecsi-sp.git" CACHE STRING
	"Set the FleCSI-SP source repository")
set(SOURCE_BRANCH "devel" CACHE STRING
  "Set the source repository branch")
set(SOURCE_VERSION "devel" CACHE STRING
  "Set the source repository version")

set(REGISTRY "gitlab.lanl.gov:5050/flecsi/flecsi-sp" CACHE STRING
  "Set the registry")

#------------------------------------------------------------------------------#
# Images
#------------------------------------------------------------------------------#

list(APPEND BASE_IMAGES
  centos-8
  fedora-30
  clearlinux
)

foreach(image ${BASE_IMAGES})

  add_image(${image}
    ${REGISTRY}:${image}
    docker/os/${image}
    ARGS
      SPACK_REPO=${SPACK_REPOSITORY}
      SPACK_BRANCH=${SPACK_BRANCH}
      SPACK_HASH=${SPACK_HASH}
  )

  add_image(${TARGET_BRANCH}-${image}
    ${REGISTRY}:${TARGET_BRANCH}-${image}
    docker/targets/ci
    TARGET_BRANCH
      ${TARGET_BRANCH}
    ARGS
      SOURCE_REPO=${SOURCE_REPOSITORY}
      SOURCE_BRANCH=${SOURCE_BRANCH}
      SOURCE_REPOSITORY=${SOURCE_REPOSITORY}
      SOURCE_VERSION=${SOURCE_VERSION}
      FLeCSI_REPOSITORY=${FLeCSI_REPOSITORY}
      OS=${REGISTRY}:${image}
    DEPENDS
      push-${image}
  )

  add_image(util-${image}
    ${REGISTRY}:util-${image}
    docker/util/ci
    TARGET_BRANCH
      util
    ARGS
      SPACK_REPO=${SPACK_REPOSITORY}
      SPACK_BRANCH=${SPACK_BRANCH}
      FLeCSI_REPOSITORY=${FLeCSI_REPOSITORY}
      OS=${REGISTRY}:${image}
    DEPENDS
      push-${image}
  )

  add_image(flecsi-sp-${image}
    ${REGISTRY}:flecsi-sp-${image}
    docker/targets/flecsi-sp
    ARGS
      SOURCE_VERSION=2.0
      OS=${REGISTRY}:2.0-${image}
    DEPENDS
      push-2.0-${image}
  )

endforeach()

#------------------------------------------------------------------------------#
# Legion unit test image
#------------------------------------------------------------------------------#

add_image(legion-unit ${REGISTRY}:legion-unit
  devel/legion-unit
  ARGS
    OS=${REGISTRY}:devel-ubuntu-20.04
  DEPENDS
    push-devel-ubuntu-20.04
)

#------------------------------------------------------------------------------#
# Summary
#------------------------------------------------------------------------------#

include(summary)

summary_header()
summary_info("TARGET_BRANCH" "${TARGET_BRANCH}" TRUE)
summary_info("SPACK_REPOSITORY" "${SPACK_REPOSITORY}" TRUE)
summary_info("SPACK_BRANCH" "${SPACK_BRANCH}" TRUE)
summary_info("SPACK_HASH" "${SPACK_HASH}" TRUE)
summary_info("FLeCSI_REPOSITORY" "${FLeCSI_REPOSITORY}" TRUE)
summary_info("FLeCSI_BRANCH" "${FLeCSI_BRANCH}" TRUE)
summary_info("SOURCE_REPOSITORY" "${SOURCE_REPOSITORY}" TRUE)
summary_info("SOURCE_BRANCH" "${SOURCE_BRANCH}" TRUE)
summary_info("SOURCE_VERSION" "${SOURCE_VERSION}" TRUE)

message(STATUS ${_summary})
