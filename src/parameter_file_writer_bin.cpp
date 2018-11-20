/** \file parameter_file_writer_bin.cpp 
  \brief Implementation file for parameter_file_writer_bin.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "parameter_file_writer_bin.hpp"


parameter_file_writer_bin::
parameter_file_writer_bin
(void)
{
  {
    // Intentionally left blank.
  }
}

parameter_file_writer_bin::
~parameter_file_writer_bin
(void)
{
  {
    // Intentionally left blank.
  }
}

int
parameter_file_writer_bin::
write_o
(bool         active,
 char*        identifier,
 double       time,
 int          n_parameter_iids,
 int*         the_parameter_iids,
 int          n_observation_iids,
 int*         the_observation_iids,
 int          n_instrument_iids,
 int*         the_instrument_iids)
{
  {
    //
    // There exist no o-records in parameter files. Therefore, it is not
    // legal to try to write one.
    //

    return 3;
  }
}
