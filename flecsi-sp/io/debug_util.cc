/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */

#include "debug_util.hh"

namespace fsp {
namespace io {

void
write_headers(int exoid) {
  auto status = ex_put_variable_param(exoid, EX_ELEM_BLOCK, 1);
  if(status)
    flog_fatal("Problem writing variable params to exodus file, "
               << "ex_put_variable_param() returned " << status);

  std::string var_name{"color"};
  char * vname = const_cast<char *>(var_name.c_str());
  status = ex_put_variable_names(exoid, EX_ELEM_BLOCK, 1, &vname);
  if(status)
    flog_fatal("Problem writing variable names to exodus file, "
               << "ex_put_variable_names() returned " << status);

  double time = 0;
  status = ex_put_time(exoid, 1, &time);
  if(status)
    flog_fatal("Problem writing time to exodus file, "
               << "ex_put_time() returned " << status);
}

} // namespace io
} // namespace fsp
