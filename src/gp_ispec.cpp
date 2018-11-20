/** \file gp_ispec.cpp
  \brief Implementation file for gp_ispec.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_ispec.hpp"

bool
gp_ispec::
get_active
(void)
{
  {
    return active_;
  }
}

gp_clist*
gp_ispec::
get_clist_ptr
(void)
{
  {
    return &clist_;
  }
}

lineage_data*
gp_ispec::
get_lineage_ptr
(void)
{
  {
    return &lineage_;
  }
}

gp_timespec*
gp_ispec::
get_timespec_ptr
(void)
{
  {
    return &timespec_;
  }
}

string
gp_ispec::
get_toolbox
(void)
{
  {
    return toolbox_;
  }
}

gp_tspec*
gp_ispec::
get_tspec_ptr
(void)
{
  {
    return &tspec_;
  }
}

string
gp_ispec::
get_type
(void)
{
  {
    return type_;
  }
}

gp_ispec::
gp_ispec
(void)
{
  {
    active_ = false;
    type_.clear();
  }
}

gp_ispec::
~gp_ispec
(void)
{
  {
     // Intentionally left blank.
  }
}

void
gp_ispec::
set_active
(bool active)
{
  {
    active_ = active;
  }
}

void
gp_ispec::
set_toolbox
(string& toolbox)
{
  set_precondition (!toolbox.empty());

  {
    toolbox_ = toolbox;
  }
}

void
gp_ispec::
set_type    
(string& type)
{
  set_precondition (!type.empty());

  {
    type_ = type;
  }
}
