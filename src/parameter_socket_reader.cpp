/** \file parameter_socket_reader.cpp 
  \brief Implementation file for parameter_socket_reader.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "parameter_socket_reader.hpp"


parameter_socket_reader::
parameter_socket_reader
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

parameter_socket_reader::
~parameter_socket_reader
(void)
{
  {
    // Intentionally left blank.
  }
}

