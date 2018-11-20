/** \file astrolabe_parse_error.cpp
\brief Implementation file for astrolabe_parse_error.hpp.
\ingroup ASTROLABE_exceptions_group
*/

#include "astrolabe_parse_error.hpp"
#include "astrolabe_time.hpp"

astrolabe_parse_error::
astrolabe_parse_error
(void)
{
  {
    method_      = "";
    class_       = "";
    file_        = "";
    line_        = 0;
    device_      = "";
    severity_    = 2000;
    description_ = "parse error";
  }
}

astrolabe_parse_error::
astrolabe_parse_error
(const string & the_device)
{
  {
    method_      = "";
    class_       = "";
    file_        = "";
    line_        = 0;
    device_      = the_device;
    severity_    = 2000; 
    description_ = "parse error";
  }
}

astrolabe_parse_error::
astrolabe_parse_error
(const astrolabe_parse_error & other) : astrolabe_IO (other)
{
  {

    //
    // intentionally added
    //

  }
}

astrolabe_parse_error::
~astrolabe_parse_error
(void)
throw
()
{
  {
    // Intentionally left blank
  }
}

string
astrolabe_parse_error::
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
astrolabe_parse_error::
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
(ostream & result, const astrolabe_parse_error & the_exception)
{
  {
    result << " \'astrolabe_parse_error\' exception" << endl;
    result << "  error severity:  " << the_exception._severity() << endl;
    if (the_exception._description() != "")
      result << "  error description: " << the_exception._description() << endl;
    if (the_exception._device() != "")
      result << "  troublesome device   : " << the_exception._device() << endl;
    else
      result << "  troublesome device   : " << "unknown" << endl;
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
astrolabe_parse_error::
_name_code
(void)
const
{
  string result;
  {
    result = "astrolabe_parse_error";
  }
  return result;
}

int
astrolabe_parse_error::
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