/** \file gp_covariance.cpp
  \brief Implementation file for gp_covariance.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_covariance.hpp"

int
gp_covariance::
get_dim
(void)
{
  {
    return dim_;
  }
}

double
gp_covariance::
get_value
(int pos)
{
  set_precondition ((pos >= 0) && (pos < get_dim()));
  {
    return values_[pos];
  }
}

gp_covariance::
gp_covariance
(void)
{
  {
    dim_   = 0;
  }
}


gp_covariance::
~gp_covariance
(void)
{
  {
    if (dim_ > 0)
    {
      values_.clear();
      dim_ = 0;
    }
  }
}

void
gp_covariance::  
set_dim
(int dim)
{
  set_precondition (dim > 0);
  set_precondition (get_dim() == 0);
  {
    int i;

    dim_ = dim;

    values_.resize(dim_);

    for (i = 0; i < dim_; i++) values_[i] = 0.0;
  }
}

void
gp_covariance::
set_value
(int pos, double value)
{
  set_precondition ((pos >= 0) && (pos < get_dim()));
  {
    values_[pos] = value;
  }
}
