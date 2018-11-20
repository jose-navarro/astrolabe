/** \file gp_ilist.cpp
  \brief Implementation file for gp_ilist.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_ilist.hpp"

bool
gp_ilist::  
exists_item
(int n)
const
{
  {
    return (item_ids_.find(n) != item_ids_.end());
  }
}

int
gp_ilist::
get_dim  (void)
{
  {
    return n_items_;
  }
}

string
gp_ilist::
get_item_id
(int n)
{
  set_precondition (exists_item(n));

  {
    return item_ids_[n];
  }
}

gp_ilist::
gp_ilist
(void)
{
  {
    n_items_ = 0;
  }
}


gp_ilist::
~gp_ilist
(void)
{
  {
    item_ids_.clear();
  }
}

void
gp_ilist::
set_dim
(int n_items)
{
  set_precondition (get_dim() == 0);
  set_precondition (n_items > 0);

  {
    n_items_ = n_items;
  }
}

void
gp_ilist::
set_item_id
(int n, string& id)
{
  set_precondition (!exists_item(n));
  set_precondition ((n >= 1) && (n <= get_dim()));
  set_precondition (!id.empty());

  {
    item_ids_.insert(make_pair(n, id));
  }
}
