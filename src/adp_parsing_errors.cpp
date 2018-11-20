/** \file adp_parsing_errors.cpp
  \brief Implementation file for adp_parsing_errors.hpp
  \ingroup adp_DOMparser_group
*/

#include "adp_parsing_errors.hpp"

void
adp_parsing_errors::
add_error
(const char* new_error)
{
  {
    string ne;

    ne = new_error;
    add_error(ne);
  }
}

void
adp_parsing_errors::
add_error
(const string new_error)
{
  {
    string ne;
    astrolabe_parse_error* pe;

    ne = new_error;
    try
    {
      pe = new astrolabe_parse_error();
    }
    catch(...)
    {
      throw astrolabe_exception();
    }

    pe->set_description(ne);

    errors_.push_back(pe);
  }
}

astrolabe_parse_error*
adp_parsing_errors::
get_error
(const int i)
const
{
  set_precondition ((i>=1) & (i <=_size()));
  {
    try
    {
      return new astrolabe_parse_error(*errors_[i-1]);
    }
    catch(...)
    {
      throw astrolabe_exception();
    }
  }
}

adp_parsing_errors::
adp_parsing_errors
(const adp_parsing_errors & original)
{
  {
    int i;

    for (i=1; i<= original._size(); i++)
    {
      errors_.push_back(original.get_error(i));
    }
  }
}

adp_parsing_errors::
adp_parsing_errors
(const adp_parsing_errors * original)
{
  {
    int i;

    for (i=1; i<= original->_size(); i++)
    {
      errors_.push_back(original->get_error(i));
    }
  }
}

adp_parsing_errors::
adp_parsing_errors
(void)
{
  {
    // Intentionally left blank.
  }
}

adp_parsing_errors::
~adp_parsing_errors
(void)
{
  {
    errors_.clear();
  }
}


int
adp_parsing_errors::
_size
(void)
const
{
  {
    return (int) errors_.size();
  }
}

