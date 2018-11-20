/** \file observation_socket_writer.cpp 
  \brief Implementation file for observation_socket_writer.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "observation_socket_writer.hpp"


observation_socket_writer::
observation_socket_writer
(void)
{
  {
    //
    // Guarantee that o-records may be written (sent),
    // since observation data include these kind of records.
    //

    o_records_allowed_ = true;

  }
}

observation_socket_writer::
~observation_socket_writer
(void)
{
  {
    // Intentionally left blank.
  }
}
