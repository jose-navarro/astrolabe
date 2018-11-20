/** \file gp_plist.cpp
  \brief Implementation file for gp_plist.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_plist.hpp"

void
gp_plist::
add_item
(gp_plist_item& item)
{
  set_precondition (!exists_item(item.get_n()));
  set_precondition ((item.get_n() > 0) & (item.get_n() <= get_dim()));
  {
    items_.insert(make_pair(item.get_n(), item));
  }
}

bool
gp_plist::  
exists_item
(int n)
const
{
  {
    return (items_.find(n) != items_.end());
  }
}

gp_plist_item
gp_plist::  
get_item
(int n)
{
  set_precondition (exists_item(n));

  gp_plist_item* result;

  {
    result = new gp_plist_item(items_[n]);
  }

  return *result;
}

int
gp_plist::
get_dim
(void)
{
  {
    return dimension_;
  }
}

int
gp_plist::
get_size
(void)
{
  {
    return (int)items_.size();
  }
}

gp_plist::
gp_plist
(void)
{
  {
    dimension_ = 0;
    items_.clear();
  }
}


gp_plist::
~gp_plist
(void)
{
  {
    dimension_ = 0;
    items_.clear();
  }
}

void
gp_plist::  
set_dim
(int dimension)
{
  set_precondition (dimension > 0);
  set_precondition (get_dim() == 0);

  {
    dimension_ = dimension;
  }
}
