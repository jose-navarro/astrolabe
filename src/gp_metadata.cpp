/** \file gp_metadata.cpp
  \brief Implementation file for gp_metadata.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_metadata.hpp"

void
gp_metadata::
add_ispec
(gp_ispec& ispec)
{
  {
    ispecs_.push_back(ispec);
  }
}

void
gp_metadata::
add_lspec
(gp_lspec& lspec)
{
  {
    lspecs_.push_back(lspec);
  }
}

void
gp_metadata::
add_mspec
(gp_mspec & mspec)
{
  {
    mspecs_.push_back(mspec);
  }
}

void
gp_metadata::
add_pspec
(gp_pspec& pspec)
{
  {
    pspecs_.push_back(pspec);
  }
}

int
gp_metadata::
get_dim_ispec
(void)
{
  {
    return (int)ispecs_.size();
  }
}

int
gp_metadata::
get_dim_lspec
(void)
{
  {
    return (int)lspecs_.size();
  }
}

int
gp_metadata::
get_dim_mspec
(void)
{
  {
    return (int)mspecs_.size();
  }
}

int
gp_metadata::
get_dim_pspec
(void)
{
  {
    return (int)pspecs_.size();
  }
}

gp_ispec
gp_metadata::
get_ispec
(int pos)
{
  set_precondition ((pos >= 0) && (pos < get_dim_ispec()));

  {
    return ispecs_.at(pos);
  }
}

gp_lspec
gp_metadata::
get_lspec
(int pos)
{
  set_precondition ((pos >= 0) && (pos < get_dim_lspec()));

  {
    return lspecs_.at(pos);
  }
}

gp_mspec
gp_metadata::
get_mspec
(int pos)
{
  set_precondition ((pos >= 0) && (pos < get_dim_mspec()));

  {
    return mspecs_.at(pos);
  }
}

gp_pspec
gp_metadata::
get_pspec
(int pos)
{
  set_precondition ((pos >= 0) && (pos < get_dim_pspec()));

  {
    return pspecs_.at(pos);
  }
}

gp_metadata::
gp_metadata
(void)
{
  {
    ispecs_.clear();
    lspecs_.clear();
    mspecs_.clear();
    pspecs_.clear();
  }
}


gp_metadata::
~gp_metadata
(void)
{
  {
    ispecs_.clear();
    lspecs_.clear();
    mspecs_.clear();
    pspecs_.clear();
  }
}
