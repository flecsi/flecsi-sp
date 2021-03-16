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
# Copyright (c) 2016 Los Alamos National Laboratory, LLC
# All rights reserved
#------------------------------------------------------------------------------#

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(FLECSI_SP_ColorReset  "${Esc}[m")
  set(FLECSI_SP_ColorBold   "${Esc}[1m")
  set(FLECSI_SP_Red         "${Esc}[31m")
  set(FLECSI_SP_Green       "${Esc}[32m")
  set(FLECSI_SP_Yellow      "${Esc}[33m")
  set(FLECSI_SP_Brown       "${Esc}[0;33m")
  set(FLECSI_SP_Blue        "${Esc}[34m")
  set(FLECSI_SP_Magenta     "${Esc}[35m")
  set(FLECSI_SP_Cyan        "${Esc}[36m")
  set(FLECSI_SP_White       "${Esc}[37m")
  set(FLECSI_SP_BoldGrey    "${Esc}[1;30m")
  set(FLECSI_SP_BoldRed     "${Esc}[1;31m")
  set(FLECSI_SP_BoldGreen   "${Esc}[1;32m")
  set(FLECSI_SP_BoldYellow  "${Esc}[1;33m")
  set(FLECSI_SP_BoldBlue    "${Esc}[1;34m")
  set(FLECSI_SP_BoldMagenta "${Esc}[1;35m")
  set(FLECSI_SP_BoldCyan    "${Esc}[1;36m")
  set(FLECSI_SP_BoldWhite   "${Esc}[1;37m")
endif()
