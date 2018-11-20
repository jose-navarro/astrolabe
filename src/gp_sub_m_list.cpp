/** \file gp_sub_m_list.cpp
  \brief Implementation file for gp_sub_m_list.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_sub_m_list.hpp"

bool
gp_sub_m_list::
exists_item
(int n)
const
{
  {
    return (item_ids_.find(n) != item_ids_.end());
  }
}

int
gp_sub_m_list::
get_dim  (void)
{
  {
    return n_items_;
  }
}

string
gp_sub_m_list::
get_item_id
(int n)
{
  set_precondition ((n >= 1) && (n <= get_dim()));
  set_precondition (exists_item(n));

  {
    return item_ids_[n];
  }
}

gp_sub_m_list::
gp_sub_m_list
(void)
{
  {
    n_items_ = 0;
  }
}


gp_sub_m_list::
~gp_sub_m_list
(void)
{
  {
    item_ids_.clear();
  }
}

void
gp_sub_m_list::
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
gp_sub_m_list::
set_item_id
(int n, string& id)
{
  set_precondition (!exists_item(n));
  set_precondition ((n >= 1) && (n <= get_dim()));
  set_precondition (!id.empty());

  {
    item_ids_[n] = id;
  }
}
