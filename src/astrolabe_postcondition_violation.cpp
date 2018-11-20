/** \file astrolabe_postcondition_violation.cpp
\brief Implementation file for astrolabe_postcondition_violation.hpp.
\ingroup ASTROLABE_exceptions_group
*/

#include "astrolabe_postcondition_violation.hpp"
#include "astrolabe_time.hpp"

astrolabe_postcondition_violation::
astrolabe_postcondition_violation
(void)
{
  {
    method_      = "";
    class_       = "";
    file_        = "";
    line_        = 0;
    contract_    = "";
    description_ = "postcondition violated";
    severity_    = 2000;
  }
}

astrolabe_postcondition_violation::
astrolabe_postcondition_violation
(const astrolabe_postcondition_violation & other) : astrolabe_contract_violation (other)
{
  {

    //
    // intentionally added
    //

  }
}

astrolabe_postcondition_violation::
~astrolabe_postcondition_violation
(void)
{
  {
    // intentionally left blank
  }
}

string
astrolabe_postcondition_violation::
_description
(void)
const
{
  string result;
  {
    result = description_;
  }
  return result;
}

void
astrolabe_postcondition_violation::
print
(ostream & the_stream)
const
{
  {
    the_stream << (*this);
  }
}

ostream &
operator << (ostream & result,const astrolabe_postcondition_violation & the_exception)
{
  {
    result << " \'astrolabe_postcondition_violation\' exception" << endl;
    result << "  error severity:  " << the_exception._severity() << endl;
    if (the_exception._description() != "")
      result << "  error description: " << the_exception._description() << endl;
    if (the_exception._contract() != "")
      result << "  postcondition    : " << "set_postcondition(" << the_exception._contract() << ")" << endl;
    astrolabe_time Time;
    the_exception._time(Time);
    result << "  time of throwing: ";
    result << Time <<endl;
    #ifndef NDEBUG
      result << "  debug information:" << endl;
      if (the_exception._class_in() != "")
        result << "    in class    : " << the_exception._class_in() << endl;
      if (the_exception._method_in() != "")
        result << "    in method    : " << the_exception._method_in() << endl;
      if (the_exception._file() != "")
        result << "    in src.file : " << the_exception._file() << endl;
      if (the_exception._line() != 0)
        result << "    at line     : " << the_exception._line() << endl;
    #endif
  }
  return result;
}

void
astrolabe_postcondition_violation::
set_description
(const string & the_postcondition)
{
  {
    if (the_postcondition != "")
      description_ = the_postcondition;
  }
}

string
astrolabe_postcondition_violation::
_name_code
(void)
const
{
  string result;
  {
    result = "astrolabe_postcondition_violation";
  }
  return result;
}

int
astrolabe_postcondition_violation::
_num_code
(void)
const
{
  int result;
  {
    result = 0;
  }
  return result;
}
