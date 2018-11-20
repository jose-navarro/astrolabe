/** \file obs_residuals_file_writer_txt.cpp 
  \brief  Implementation file for obs_residuals_file_writer_txt.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "obs_residuals_file_writer_txt.hpp"

obs_residuals_file_writer_txt::
obs_residuals_file_writer_txt
(void)
{
  {
    // Intentionally left blank.
  }
}

obs_residuals_file_writer_txt::
~obs_residuals_file_writer_txt
(void)
{
  {
    // Intentionally left blank.
  }
}

int
obs_residuals_file_writer_txt::
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
    // There exist no o-records in observation residuals files. Therefore, it is not
    // legal to try to write one.
    //

    return 3;
  }
}
