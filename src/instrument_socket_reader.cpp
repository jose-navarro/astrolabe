/** \file instrument_socket_reader.cpp 
  \brief Implementation file for instrument_socket_reader.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "instrument_socket_reader.hpp"


instrument_socket_reader::
instrument_socket_reader
(void)
{
  {
    //
    // Guarantee that the transmission of o-records is NOT allowed.
    // Doing this, this class becomes a partial implementation
    // of the obs-e specification.
    //

    o_records_allowed_ = false;
  }
}

instrument_socket_reader::
~instrument_socket_reader
(void)
{
  {
    // Intentionally left blank.
  }
}

