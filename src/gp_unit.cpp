/** \file gp_unit.cpp
  \brief Implementation file for gp_unit.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_unit.hpp"

int
gp_unit::
get_dim  (void)
{
  {
    return n_units_;
  }
}

string
gp_unit::
get_unit
(int pos)
{
  set_precondition ((pos >= 0) && (pos < get_dim()));

  {
    return units_.at(pos);
  }
}

gp_unit::
gp_unit
(void)
{
  {
    n_units_ = 0;
  }
}


gp_unit::
~gp_unit
(void)
{
  {
    if (n_units_ > 0) units_.clear();
  }
}

void
gp_unit::
set_dim
(int n_units)
{
  set_precondition (get_dim() == 0);
  set_precondition (n_units > 0);

  {
    n_units_ = n_units;
    units_.resize(n_units_);
  }
}

void
gp_unit::
set_unit
(int pos, string& unit_spec)
{
  set_precondition ((pos >= 0) && (pos < get_dim()));
  set_precondition (!unit_spec.empty());

  {
    units_[pos] = unit_spec;
  }
}
