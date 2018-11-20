/** \file gp_clist_item.cpp
  \brief Implementation file for gp_clist_item.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_clist_item.hpp"

gp_covariance*
gp_clist_item::
get_covariance_ptr
(void)
{
  {
    return &covariance_;
  }
}

int
gp_clist_item::
get_n
(void)
const
{
  {
    return n_;
  }
}

gp_ref*
gp_clist_item::
get_ref_ptr
(void)
{
  {
    return &ref_;
  }
}

gp_scale*
gp_clist_item::
get_scale_ptr
(void)
{
  {
    return &scale_;
  }
}

bool
gp_clist_item::
dimensions_are_valid
(vector<int>& dimensions)
const
{
  {
    size_t i;
    size_t total;

    total = dimensions.size();
    if (total == 0) return false;

    for (i = 0; i < total; i++)
    {
      if (dimensions[i] <= 1) return false;
    }

    return true;
  }
}

gp_clist_item_type*
gp_clist_item::
get_type_ptr
(void)
{
  {
    return &type_;
  }
}

gp_unit*
gp_clist_item::
get_unit_ptr
(void)
{
  {
    return &units_;
  }
}

gp_clist_item::
gp_clist_item
(void)
{
  {
    n_ = 0;
  }
}


gp_clist_item::
~gp_clist_item
(void)
{
  {
     // Intentionally left blank.
  }
}

void
gp_clist_item::  
set_n
(int n)
{
  set_precondition (n > 0);

  {
    n_ = n;
  }
}

void
gp_clist_item::
set_type_to_matrix
(vector<int>& dimensions)
{
  set_precondition(dimensions_are_valid(dimensions));

  {
    type_.set_type_to_matrix(dimensions);
  }
}
