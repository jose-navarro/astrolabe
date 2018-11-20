/** \file gp_lspec.cpp
  \brief Implementation file for gp_lspec.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_lspec.hpp"

bool
gp_lspec::
get_active
(void)
{
  {
    return active_;
  }
}

gp_covariance*
gp_lspec::
get_covariance_ptr
(void)
{
  {
    return &covariance_;
  }
}

int
gp_lspec::
get_dimension
(void)
const
{
  {
    return dimension_;
  }
}

string
gp_lspec::
get_type
(void)
{
  {
    return type_;
  }
}

lineage_data*
gp_lspec::
get_lineage_ptr
(void)
{
  {
    return &lineage_;
  }
}

gp_ref*
gp_lspec::
get_ref_ptr
(void)
{
  {
    return &ref_;
  }
}

gp_scale*
gp_lspec::
get_scale_ptr
(void)
{
  {
    return &scale_;
  }
}

gp_timespec*
gp_lspec::
get_timespec_ptr
(void)
{
  {
    return &timespec_;
  }
}

string
gp_lspec::
get_toolbox
(void)
{
  {
    return toolbox_;
  }
}

gp_tspec*
gp_lspec::  
get_tspec_ptr
(void)
{
  {
    return &tspec_;
  }
}

gp_unit*
gp_lspec::
get_unit_ptr
(void)
{
  {
    return &units_;
  }
}

gp_lspec::
gp_lspec
(void)
{
  {
    active_    = false;
    dimension_ = 0;

    type_.clear();
    toolbox_.clear();
  }
}


gp_lspec::
~gp_lspec
(void)
{
  {
     // Intentionally left blank.
  }
}

void
gp_lspec::
set_active
(bool active)
{
  {
    active_ = active;
  }
}

void
gp_lspec::  
set_dimension
(int dimension)
{
  set_precondition (dimension > 0);

  {
    dimension_ = dimension;
  }
}

void
gp_lspec::
set_type    
(string& type)
{
  set_precondition (!type.empty());

  {
    type_ = type;
  }
}

void
gp_lspec::
set_toolbox
(string& toolbox)
{
  set_precondition (!toolbox.empty());

  {
    toolbox_ = toolbox;
  }
}

