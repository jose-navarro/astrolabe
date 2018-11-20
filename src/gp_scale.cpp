/** \file gp_scale.cpp
  \brief Implementation file for gp_scale.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_scale.hpp"


void
gp_scale::
add_scale
(double value)
{
  {
    scale_.push_back(value);
  }
}

int
gp_scale::
get_dim
(void)
{
  {
    return (int)scale_.size();
  }
}

double
gp_scale::
get_value
(int pos)
{
  set_precondition ((pos >= 0) & (pos < get_dim()));

  {
    return (scale_.at(pos));
  }
}

gp_scale::
gp_scale
(void)
{
  {
  }
}


gp_scale::
~gp_scale
(void)
{
  {
    scale_.clear();
  }
}
