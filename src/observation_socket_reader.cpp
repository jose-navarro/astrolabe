/** \file observation_socket_reader.cpp 
  \brief Implementation file for observation_socket_reader.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "observation_socket_reader.hpp"


observation_socket_reader::
observation_socket_reader
(void)
{
  {
    //
    // Guarantee that the transmission of o-records is allowed.
    // Doing this, this class is fully compliant with the
    // obs-e specification.
    //

    o_records_allowed_ = true;
  }
}

observation_socket_reader::
~observation_socket_reader
(void)
{
  {
    // Intentionally left blank.
  }
}

