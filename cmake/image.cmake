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

function(add_image target tag dockerfile)

  set(options)
  set(one_value_args)
  set(multi_value_args TARGET_BRANCH ARGS DEPENDS)

  cmake_parse_arguments(image "${options}" "${one_value_args}"
    "${multi_value_args}" ${ARGN})

  foreach(arg ${image_ARGS})
    list(APPEND args "--build-arg;${arg}")
  endforeach()

  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/targets)

  message(STATUS "Adding target: ${target}")
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/targets/${target})

  configure_file(${CMAKE_SOURCE_DIR}/config/dockerignore
    ${CMAKE_BINARY_DIR}/targets/${target}/.dockerignore)

  if(EXISTS ${CMAKE_SOURCE_DIR}/aux/gnu-${target}.xz)
    file(COPY ${CMAKE_SOURCE_DIR}/aux/gnu-${target}.xz
      DESTINATION ${CMAKE_BINARY_DIR}/targets/${target}
    )
  endif()

  file(COPY ${CMAKE_SOURCE_DIR}/config/clang_fortran.py
    DESTINATION ${CMAKE_BINARY_DIR}/targets/${target}
    FILE_PERMISSIONS
      OWNER_READ
      OWNER_WRITE
      OWNER_EXECUTE
      GROUP_READ
      GROUP_EXECUTE
      WORLD_READ
      WORLD_EXECUTE
  )

  file(COPY ${CMAKE_SOURCE_DIR}/config/dircolors
    DESTINATION ${CMAKE_BINARY_DIR}/targets/${target}
  )

  file(COPY ${CMAKE_SOURCE_DIR}/config/bashrc
    DESTINATION ${CMAKE_BINARY_DIR}/targets/${target}
  )

  file(COPY ${CMAKE_SOURCE_DIR}/config/vimrc
    DESTINATION ${CMAKE_BINARY_DIR}/targets/${target}
  )

  if(image_TARGET_BRANCH)
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/branches/${image_TARGET_BRANCH}/spack)
      message(FATAL_ERROR "Branch spack directory "
        "${CMAKE_SOURCE_DIR}/branches/${image_TARGET_BRANCH}/spack "
        "does not exist")
    endif()

    file(COPY ${CMAKE_SOURCE_DIR}/branches/${image_TARGET_BRANCH}/spack
      DESTINATION ${CMAKE_BINARY_DIR}/targets/${target})
  endif()

  add_custom_target(${target} ALL
    ${ENGINE_EXECUTABLE} build \${BUILD_EXTRA} ${args} -t ${tag}
      -f ${CMAKE_SOURCE_DIR}/${dockerfile}
      ${CMAKE_BINARY_DIR}/targets/${target}
    DEPENDS ${dockerfile} ${image_DEPENDS})

  add_custom_target(push-${target} ALL
    ${ENGINE_EXECUTABLE} push \${PUSH_EXTRA} ${tag}
    DEPENDS ${target})

  add_custom_target(pull-${target}
    ${ENGINE_EXECUTABLE} pull \${PULL_EXTRA} ${tag})

  add_custom_target(clean-${target}
    ${ENGINE_EXECUTABLE} rmi \${CLEAN_EXTRA} ${tag})

endfunction()
