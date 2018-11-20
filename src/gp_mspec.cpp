/** \file gp_mspec.cpp
  \brief Implementation file for gp_mspec.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_mspec.hpp"

bool
gp_mspec::
get_active
(void)
{
  {
    return active_;
  }
}

bool
gp_mspec::
get_dynamic
(void)
{
  {
    return dynamic_;
  }
}

gp_ilist*
gp_mspec::
get_ilist_ptr
(void)
{
  {
    return &ilist_;
  }
}

lineage_data*
gp_mspec::
get_lineage_ptr
(void)
{
  {
    return &lineage_;
  }
}

gp_llist*
gp_mspec::
get_llist_ptr
(void)
{
  {
    return &llist_;
  }
}

gp_plist*
gp_mspec::
get_plist_ptr
(void)
{
  {
    return &plist_;
  }
}

gp_sub_m_list*
gp_mspec::
get_sub_m_list_ptr
(void)
{
  {
    return &sub_m_list_;
  }
}

string
gp_mspec::
get_type
(void)
{
  {
    return type_;
  }
}

string
gp_mspec::
get_toolbox
(void)
{
  {
    return toolbox_;
  }
}

gp_mspec::
gp_mspec
(void)
{
  {
    active_  = false;
    dynamic_ = false;
  }
}


gp_mspec::
~gp_mspec
(void)
{
  {
     // Intentionally left blank.
  }
}

void
gp_mspec::
set_active
(bool active)
{
  {
    active_ = active;
  }
}

void
gp_mspec::
set_dynamic
(bool dynamic)
{
  {
    dynamic_ = dynamic;
  }
}

void
gp_mspec::
set_toolbox
(string& toolbox)
{
  set_precondition (!toolbox.empty());

  {
    toolbox_ = toolbox;
  }
}

void
gp_mspec::
set_type    
(string& type)
{
  set_precondition (!type.empty());

  {
    type_ = type;
  }
}
