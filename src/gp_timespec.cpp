/** \file gp_timespec.cpp
  \brief Implementation file for gp_timespec.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_timespec.hpp"

gp_ref*
gp_timespec::
get_ref_ptr
(void)
{
  {
    return &ref_;
  }
}

gp_unit*
gp_timespec::
get_unit_ptr
(void)
{
  {
    return &units_;
  }
}

gp_timespec::
gp_timespec
(void)
{
  {
  }
}


gp_timespec::
~gp_timespec
(void)
{
  {
     // Intentionally left blank.
  }
}
