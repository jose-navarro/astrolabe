/** \file gp_clist.cpp
  \brief Implementation file for gp_clist.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_clist.hpp"

void
gp_clist::
add_item
(gp_clist_item& item)
{
  set_precondition (!exists_item(item.get_n()));
  set_precondition ((item.get_n() > 0) & (item.get_n() <= get_dim()));
  {
    items_.insert(make_pair(item.get_n(), item));
  }
}

bool
gp_clist::  
exists_item
(int n)
const
{
  {
    return (items_.find(n) != items_.end());
  }
}

gp_clist_item
gp_clist::  
get_item
(int n)
{
  set_precondition (exists_item(n));

  gp_clist_item* result;

  {
    result = new gp_clist_item(items_[n]);
  }

  return *result;
}

int
gp_clist::
get_dim
(void)
{
  {
    return dimension_;
  }
}

int
gp_clist::
get_size
(void)
{
  {
    return (int)items_.size();
  }
}

gp_clist::
gp_clist
(void)
{
  {
    dimension_ = 0;
    items_.clear();
  }
}


gp_clist::
~gp_clist
(void)
{
  {
    dimension_ = 0;
    items_.clear();
  }
}

void
gp_clist::  
set_dim
(int dimension)
{
  set_precondition (dimension > 0);
  set_precondition (get_dim() == 0);

  {
    dimension_ = dimension;
  }
}
