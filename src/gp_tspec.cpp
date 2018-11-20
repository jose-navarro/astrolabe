/** \file gp_tspec.cpp
  \brief Implementation file for gp_tspec.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_tspec.hpp"

int
gp_tspec::
get_dimension
(void)
const
{
  {
    return dimension_;
  }
}

gp_ref*
gp_tspec::
get_ref_ptr
(void)
{
  {
    return &ref_;
  }
}

gp_unit*
gp_tspec::
get_unit_ptr
(void)
{
  {
    return &units_;
  }
}

gp_tspec::
gp_tspec
(void)
{
  {
    dimension_ = 0;
  }
}


gp_tspec::
~gp_tspec
(void)
{
  {
     // Intentionally left blank.
  }
}

void
gp_tspec::  
set_dimension
(int dimension)
{
  set_precondition (dimension > 0);

  {
    dimension_ = dimension;
  }
}
