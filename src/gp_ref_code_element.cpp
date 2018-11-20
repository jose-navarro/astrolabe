/** \file gp_ref_code_element.cpp
  \brief Implementation file for gp_ref_code_element.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_ref_code_element.hpp"

void
gp_ref_code_element::
add_parameters
(const string & parameter)
{
  {
    if (parameter == "") return;
    parameters_.push_back(parameter);
  }
}

string &
gp_ref_code_element::
get_code
(void)
{
  {
    return code_;
  }
}

int
gp_ref_code_element::
get_n_of_parameters
(void)
const
{
  {
    return (int)parameters_.size();
  }
}

string &
gp_ref_code_element::
get_parameter
(int pos)
{
  string* result;
  {
    result = new string("");
    if ((pos >= 0) & (pos < (int)parameters_.size())) *result = parameters_.at(pos);
    return *result;
  }
}

gp_ref_code_element::
gp_ref_code_element
(void)
{
  {
    code_ = "";
    parameters_.clear();
  }
}


gp_ref_code_element::
~gp_ref_code_element
(void)
{
  {
    code_ = "";
    parameters_.clear();
  }
}

void
gp_ref_code_element::
set_code
(const string & code)
{
  {
    code_ = code;
  }
}

string &
gp_ref_code_element::
to_string
(void)
{
  string* result;

  {
    int i;
    int size;

    result = new string("");

    // Return the empty string if no code was ever set.

    if (code_ == "") return *result;

    // We've got a code. Copy it to the output result.

    *result += code_;

    // If we have no parameters, we may return now.

    size = (int)parameters_.size();

    if (size == 0) return *result;

    // We've got some parameters. Concatenate these to the code.

    *result += "(";

    for (i = 0; i < size; i++)
    {
      *result += parameters_.at(i);
      if (i != size - 1) *result += ",";
    }

    *result += ")";

    // That's all.

    return *result;
  }
}
