/** \file gp_pspec.cpp
  \brief Implementation file for gp_pspec.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_pspec.hpp"

bool
gp_pspec::
get_active
(void)
{
  {
    return active_;
  }
}

gp_covariance*
gp_pspec::
get_covariance_ptr
(void)
{
  {
    return &covariance_;
  }
}

int
gp_pspec::
get_dimension
(void)
const
{
  {
    return dimension_;
  }
}

lineage_data*
gp_pspec::
get_lineage_ptr
(void)
{
  {
    return &lineage_;
  }
}

gp_ref*
gp_pspec::
get_ref_ptr
(void)
{
  {
    return &ref_;
  }
}

gp_scale*
gp_pspec::
get_scale_ptr
(void)
{
  {
    return &scale_;
  }
}

gp_timespec*
gp_pspec::
get_timespec_ptr
(void)
{
  {
    return &timespec_;
  }
}

string
gp_pspec::
get_toolbox
(void)
{
  {
    return toolbox_;
  }
}

gp_tspec*
gp_pspec::  
get_tspec_ptr
(void)
{
  {
    return &tspec_;
  }
}

string
gp_pspec::
get_type
(void)
{
  {
    return type_;
  }
}

gp_unit*
gp_pspec::
get_unit_ptr
(void)
{
  {
    return &units_;
  }
}

gp_pspec::
gp_pspec
(void)
{
  {
    active_    = false;
    dimension_ = 0;

    type_.clear();
    toolbox_.clear();
  }
}


gp_pspec::
~gp_pspec
(void)
{
  {
     // Intentionally left blank.
  }
}

void
gp_pspec::
set_active
(bool active)
{
  {
    active_ = active;
  }
}

void
gp_pspec::  
set_dimension
(int dimension)
{
  set_precondition (dimension > 0);

  {
    dimension_ = dimension;
  }
}

void
gp_pspec::
set_toolbox
(string& toolbox)
{
  set_precondition (!toolbox.empty());

  {
    toolbox_ = toolbox;
  }
}

void
gp_pspec::
set_type    
(string& type)
{
  set_precondition (!type.empty());

  {
    type_ = type;
  }
}
