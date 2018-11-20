/** \file astrolabe_precondition_violation.cpp
\brief Implementation file for astrolabe_precondition_violation.hpp.
\ingroup ASTROLABE_exceptions_group
*/

#include "astrolabe_precondition_violation.hpp"
#include "astrolabe_time.hpp"

astrolabe_precondition_violation::
astrolabe_precondition_violation
(void)
{
  {
    method_      = "";
    class_       = "";
    file_        = "";
    line_        = 0;
    contract_    = "";
    description_ = "precondition violated";
    severity_    = 2000;
  }
}

astrolabe_precondition_violation::
astrolabe_precondition_violation
(const astrolabe_precondition_violation & other) : astrolabe_contract_violation (other)
{
  {

    //
    // intentionally added
    //

  }
}

astrolabe_precondition_violation::
~astrolabe_precondition_violation
(void)
{
  {
    // intentionally left blank
  }
}

string
astrolabe_precondition_violation::
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
astrolabe_precondition_violation::
set_description
(const string & the_precondition)
{
  {
    if (the_precondition != "")
      description_ = the_precondition;
  }
}

void
astrolabe_precondition_violation::
print
(ostream & the_stream)
const
{
  {
    the_stream << (*this);
  }
}

ostream &
operator <<
(ostream & result,const astrolabe_precondition_violation & the_exception)
{
  {
    result << " \'astrolabe_precondition_violation\' exception" << endl;
    result << "  error severity:  " << the_exception._severity() << endl;
    if (the_exception._description() != "")
      result << "  error description: " << the_exception._description() << endl;
    if (the_exception._contract() != "")
      result << "  precondition    : " << "set_precondition(" << the_exception._contract() << ")" << endl;
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

string
astrolabe_precondition_violation::
_name_code
(void)
const
{
  string result;
  {
    result = "astrolabe_precondition_violation";
  }
  return result;
}

int
astrolabe_precondition_violation::
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
