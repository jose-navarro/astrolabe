/** \file astrolabe_IO.cpp
\brief Implementation file for astrolabe_IO.hpp.
\ingroup ASTROLABE_exceptions_group
*/

#include "astrolabe_IO.hpp"
#include "astrolabe_time.hpp"

astrolabe_IO::
astrolabe_IO
(void)
{
  {
    method_      = "";
    class_       = "";
    file_        = "";
    line_        = 0;
    device_      = "";
    severity_    = 2000;
    description_ = "Input/Output error";
  }
}

astrolabe_IO::
astrolabe_IO
(const string & the_device)
{
  {
    method_      = "";
    class_       = "";
    file_        = "";
    line_        = 0;
    device_      = the_device;
    severity_    = 2000;
    description_ = "Input/Output error";
  }
}

astrolabe_IO::
astrolabe_IO
(const astrolabe_IO & other) : astrolabe_exception (other)
{
  {
    device_ = other.device_;
  }
}

astrolabe_IO::
~astrolabe_IO
(void)
throw
()
{
  {
    // intentionally left blank
  }
}

string
astrolabe_IO::
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


string
astrolabe_IO::
_device
(void)
const
{
  string result;
  {
    result = device_;
  }
  return result;
}

void
astrolabe_IO::
set_device
(const string & the_device)
{
  {
    device_ = the_device;
  }
}

void
astrolabe_IO::
print
(ostream & the_stream)
const
{
  {
    the_stream << (*this);
  }
}

ostream &
operator << (ostream & result,const astrolabe_IO & the_exception)
{
  {
    result << " \'astrolabe_IO\' exception" << endl;
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
astrolabe_IO::
_name_code
(void)
const
{
  string result;
  {
    result = "astrolabe_IO";
  }
  return result;
}

int
astrolabe_IO::
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
