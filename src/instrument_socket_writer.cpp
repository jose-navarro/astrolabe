/** \file instrument_socket_writer.cpp 
  \brief Implementation file for instrument_socket_writer.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "instrument_socket_writer.hpp"


instrument_socket_writer::
instrument_socket_writer
(void)
{
  {
    //
    // Guarantee that o-records may NOT be written (sent),
    // since instrument data DOES NOT include these kind of records.
    //

    o_records_allowed_ = false;

  }
}

instrument_socket_writer::
~instrument_socket_writer
(void)
{
  {
    // Intentionally left blank.
  }
}
