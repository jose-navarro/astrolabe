/** \file gp_llist.cpp
  \brief Implementation file for gp_llist.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_llist.hpp"

bool
gp_llist::  
exists_item
(int n)
const
{
  {
    return (item_ids_.find(n) != item_ids_.end());
  }
}

int
gp_llist::
get_dim  (void)
{
  {
    return n_items_;
  }
}

string
gp_llist::
get_item_id
(int n)
{
  set_precondition (exists_item(n));

  {
    return item_ids_[n];
  }
}

gp_llist::
gp_llist
(void)
{
  {
    n_items_ = 0;
  }
}


gp_llist::
~gp_llist
(void)
{
  {
    item_ids_.clear();
  }
}

void
gp_llist::
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
gp_llist::
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
