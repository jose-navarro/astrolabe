/** \file gp_astrolabe_header_file_data.cpp
  \brief Implementation file for gp_astrolabe_header_file_data.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_astrolabe_header_file_data.hpp"

//
// Statit constants
//

const int gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_NOT_SET     = 0;
const int gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE   = 1;
const int gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE = 2;
const int gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET      = 3;

string
gp_astrolabe_header_file_data::
get_device_file_name
(void)
const
{
  set_precondition ((get_device_format()  == GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE) |
                    (get_device_format()  == GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE));

  set_precondition (is_set_device_file_name());

  {
    return file_name_;
  }
}

int
gp_astrolabe_header_file_data::
get_device_format
(void)
const
{
  {
    return device_format_;
  }
}

int
gp_astrolabe_header_file_data::
get_device_port
(void)
const
{
  set_precondition (get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET);
  set_precondition (is_set_device_port());
  {
    return socket_port_;
  }
}

string
gp_astrolabe_header_file_data::
get_device_server
(void)
const
{
  set_precondition (get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET);
  set_precondition (is_set_device_server());
  {
    return socket_server_;
  }
}

string
gp_astrolabe_header_file_data::
get_device_type
(void)
const
{
  set_precondition (is_set_device_type());
  {
    return device_type_;
  }
}

lineage_data*
gp_astrolabe_header_file_data::
get_lineage_ptr
(void)
{
  {
    return &lineage_;
  }
}

gp_astrolabe_header_file_data::
gp_astrolabe_header_file_data
(void)
{
  {
    device_format_is_set_ = false;
    device_type_is_set_   = false;
    file_name_is_set_     = false;
    socket_port_is_set_   = false;
    socket_server_is_set_ = false;

    device_format_        = GP_ASTROLABE_HEADER_FORMAT_IS_NOT_SET;
    device_type_          = "";
    file_name_            = "";
    socket_port_          = -1;
    socket_server_        = "";
  }
}


gp_astrolabe_header_file_data::
~gp_astrolabe_header_file_data
(void)
{
  {
    // Intentionally left blank.
  }
}

bool
gp_astrolabe_header_file_data::
is_set_device_file_name
(void)
const
{
  {
    return file_name_is_set_;
  }
}

bool
gp_astrolabe_header_file_data::
is_set_device_format
(void)
const
{
  {
    return device_format_is_set_;
  }
}

bool
gp_astrolabe_header_file_data::
is_set_device_port
(void)
const
{
  {
    return socket_port_is_set_;
  }
}

bool
gp_astrolabe_header_file_data::
is_set_device_type
(void)
const
{
  {
    return device_type_is_set_;
  }
}

bool
gp_astrolabe_header_file_data::
is_set_device_server
(void)
const
{
  {
    return socket_server_is_set_;
  }
}

void
gp_astrolabe_header_file_data::
set_device_file_name
(string& device_file_name)
{
  set_precondition ((get_device_format()  == GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE) |
                    (get_device_format()  == GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE));
  {
    file_name_is_set_ = true;
    file_name_        = device_file_name;
  }
}

void
gp_astrolabe_header_file_data::
set_device_format
(int device_format)
{
  set_precondition ((device_format == GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE) |
                    (device_format == GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE) |
                    (device_format == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET));

  set_precondition (!is_set_device_format());

  {
    device_format_is_set_ = true;
    device_format_        = device_format;
  }
}


void
gp_astrolabe_header_file_data::
set_device_port
(int device_port)
{
  set_precondition (get_device_format()  == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET);
  {
    socket_port_is_set_ = true;
    socket_port_        = device_port;
  }
}

void
gp_astrolabe_header_file_data::
set_device_server
(string& device_server)
{
  set_precondition (get_device_format()  == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET);
  {
    socket_server_is_set_ = true;
    socket_server_        = device_server;
  }
}

void
gp_astrolabe_header_file_data::
set_device_type
(string& device_type)
{
  {
    device_type_is_set_ = true;
    device_type_        = device_type;
  }
}
