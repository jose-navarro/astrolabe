/** \file gp_ref.cpp
  \brief Implementation file for gp_ref.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_ref.hpp"

//
// Statit constants
//

const int gp_ref::GP_REF_TYPE_NOT_SET = 0;
const int gp_ref::GP_REF_TYPE_CRF     = 1;
const int gp_ref::GP_REF_TYPE_CS_RF   = 2;

void
gp_ref::
add_CRF
(gp_ref_code& crf)
{
  set_precondition (get_type() == GP_REF_TYPE_CRF);

  {
    gp_ref_code the_crf;

    the_crf = crf;

    coordinate_reference_frame_.push_back(the_crf);
  }
}

void
gp_ref::
add_CS
(gp_ref_code& cs)
{
  set_precondition (get_type() == GP_REF_TYPE_CS_RF);

  {
    gp_ref_code the_cs;

    the_cs = cs;
    coordinate_system_.push_back(the_cs);

    has_CS_ = true;
  }
}

void
gp_ref::
add_CS_RF
(gp_ref_code& cs, gp_ref_code& rf)
{
  set_precondition (get_type() == GP_REF_TYPE_CS_RF);

  {
    gp_ref_code the_cs, the_rf;

    the_cs = cs;
    the_rf = rf;

    coordinate_system_.push_back(the_cs);
    reference_frame_.push_back(the_rf);

    has_CS_ = true;
  }
}

void
gp_ref::
add_RF
(gp_ref_code& rf)
{
  set_precondition (get_type() == GP_REF_TYPE_CS_RF);

  {
    gp_ref_code the_rf;

    the_rf = rf;
    reference_frame_.push_back(the_rf);
  }
}

gp_ref_code
gp_ref::
get_CRF
(int pos)
{
  set_precondition (get_type() == GP_REF_TYPE_CRF);
  set_precondition ((pos >= 0) & (pos < get_dim()));

  gp_ref_code* result;

  {
    gp_ref_code tmp;

    tmp = coordinate_reference_frame_.at(pos);
    result = new gp_ref_code(tmp);
  }

  return *result;
}

gp_ref_code
gp_ref::
get_CS
(int pos)
{
  set_precondition (get_type() == GP_REF_TYPE_CS_RF);
  set_precondition (has_CS());
  set_precondition ((pos >= 0) & (pos < get_dim()));

  gp_ref_code* result;

  {
    gp_ref_code tmp;

    tmp = coordinate_system_.at(pos);
    result = new gp_ref_code(tmp);
  }

  return *result;
}

int
gp_ref::
get_dim
(void)
{
  {
    if (reference_type_ == GP_REF_TYPE_NOT_SET)
    {
      return 0;
    }
    else if (reference_type_ == GP_REF_TYPE_CRF)
    {
      return (int)coordinate_reference_frame_.size();
    }
    else
    {
      //
      // Type is GP_REF_TYPE_CS_RF. The dimension
      // must be retrieved from the array with the
      // reference frames, since the coordinate systems
      // are optional.
      //

      return (int)reference_frame_.size();
    }
  }
}

int
gp_ref::
get_dim_CS
(void)
{
  {
    if (reference_type_ == GP_REF_TYPE_CS_RF) return (int)coordinate_system_.size();
    return 0;
  }
}

int
gp_ref::
get_dim_RF
(void)
{
  {
    if (reference_type_ == GP_REF_TYPE_CS_RF) return (int)reference_frame_.size();
    return 0;
  }
}

gp_ref_code
gp_ref::
get_RF
(int pos)
{
  set_precondition (get_type() == GP_REF_TYPE_CS_RF);
  set_precondition ((pos >= 0) & (pos < get_dim()));

  gp_ref_code* result;

  {
    gp_ref_code tmp;

    tmp = reference_frame_.at(pos);
    result = new gp_ref_code(tmp);
  }

  return *result;
}

int
gp_ref::
get_type
(void)
{
  {
    return reference_type_;
  }
}


gp_ref::
gp_ref
(void)
{
  {
    // Set the gp_ref object's type to not set.

    reference_type_ = GP_REF_TYPE_NOT_SET;

    // There is no list of CS codes.

    has_CS_ = false;
  }
}


gp_ref::
~gp_ref
(void)
{
  {
    //
    // Clear the data structures used, depending on the type of
    // gp_ref object being used.
    //

    if (reference_type_ == GP_REF_TYPE_CRF)
    {
      // We have a list of coordinate reference frames.

      coordinate_reference_frame_.clear();
    }
    else if (reference_type_ == GP_REF_TYPE_CS_RF)
    {
      // Here, we have a list of coordinate systems and reference frames.

      coordinate_system_.clear();
      reference_frame_.clear();
    }

    // Reset the type.

    reference_type_ = GP_REF_TYPE_NOT_SET;
  }
}

bool
gp_ref::
has_CS
(void)
{
  {
    return has_CS_;
  }
}

void
gp_ref::
set_type
(int reference_type)
{
  set_precondition (reference_type_ == GP_REF_TYPE_NOT_SET);
  set_precondition ((reference_type == GP_REF_TYPE_CRF) |
           (reference_type == GP_REF_TYPE_CS_RF));

  {
    reference_type_ = reference_type;
  }
}

