/** \file parameter_socket_writer.cpp 
  \brief Implementation file for parameter_socket_writer.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "parameter_socket_writer.hpp"


parameter_socket_writer::
parameter_socket_writer
(void)
{
  {
    //
    // Guarantee that o-records may NOT be written (sent),
    // since parameter data DOES NOT include these kind of records.
    //

    o_records_allowed_ = false;

  }
}

parameter_socket_writer::
~parameter_socket_writer
(void)
{
  {
    // Intentionally left blank.
  }
}
