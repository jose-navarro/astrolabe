/** \file gp_ref_code.cpp
\brief Implementation file for gp_ref_code.hpp
\ingroup ASTROLABE_metadata
*/

#include "gp_ref_code.hpp"

void
gp_ref_code::
add_element
(const gp_ref_code_element & element)
{
  {
    the_code_.push_back(element);
  }
}

int gp_ref_code::
get_n_elements
(void)
const
{
  {
    return (int)the_code_.size();
  }
}

gp_ref_code_element &
gp_ref_code::
get_ref_code_element
(int pos,
 int & error)
{
  gp_ref_code_element* result;

  {
    int size;

    error = 0;
    result = new gp_ref_code_element();
    size = (int)the_code_.size();

    if ((pos >= 0) & (pos < size)) *result = the_code_[pos];
    else error = 1;

    return *result;
  }
}

gp_ref_code::
gp_ref_code
(void)
{
  {
    the_code_.clear();
  }
}


gp_ref_code::
~gp_ref_code
(void)
{
  {
    the_code_.clear();
  }
}

string &
gp_ref_code::
to_string
(void)
{
  string* result;

  {
    int i;
    int size;

    result = new string("");

    size = (int)the_code_.size();

    for (i = 0; i < size; i++)
    {
      *result += the_code_[i].to_string();
      if (i != (size - 1)) *result += "+";
    }

    return *result;
  }
}
