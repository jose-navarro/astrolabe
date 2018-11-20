/** \file astrolabe_exception.cpp
\brief Implementation file for astrolabe_exception.hpp.
\ingroup ASTROLABE_exceptions_group
*/

#include "astrolabe_exception.hpp"
#include "astrolabe_time.hpp"

astrolabe_exception::
astrolabe_exception
(void)
{
  //
  // root class for the exception class hierarchy.
  //
  {
    method_ = "";
    class_  = "";
    file_   = "";
    line_   = 0;
    description_ = " general error, sorry not to be more specific";
    severity_ = 2000;

    // set time

    astrolabe_time Time;
    Time.set_to_now();
    set_time(Time);

    yy_ = Time.year();
    mm_ = Time.month();
    dd_ = Time.day();
    ho_ = Time.hour();
    mi_ = Time.minute();
    se_ = Time.second();
  }
}

astrolabe_exception::
astrolabe_exception
(const astrolabe_exception & other) : exception (other)
{
  {
    file_        = other.file_;
    line_        = other.line_;
    class_       = other.class_;
    method_      = other.method_;
    description_ = other.description_;
    severity_    = other.severity_;
    yy_          = other.yy_;
    mm_          = other.mm_;
    dd_          = other.dd_;
    ho_          = other.ho_;
    mi_          = other.mi_;
    se_          = other.se_;
  }
}

astrolabe_exception::
~astrolabe_exception
(void)
{
  {
    // intentionally left blank
  }
}

void
astrolabe_exception::
print
(ostream & the_stream)
const
{
  {
    the_stream << (*this);
  }
}


ostream &
operator << (ostream & result, const astrolabe_exception & the_exception)
{
  {
    result << "- \'astrolabe_exception\' exception" << endl;
    result << "  error severity:  " << the_exception._severity() << endl;
    result << "  error description: " << the_exception._description() << endl;
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
astrolabe_exception::
_class_in
(void)
const
{
  string result;
  //
  // get the class which triggered the exception
  //
  {
    result = class_;
  }
  return result;
}

void
astrolabe_exception::
set_class_in
(const string & the_class)
{
  //
  // set the class which triggered the exception.
  //
  {
    class_ = the_class;
  }
}

string
astrolabe_exception::
_method_in
(void)
const
{
  string result;
  //
  // get the class which triggered the exception
  //
  {
    result = method_;
  }
  return result;
}

void
astrolabe_exception::
set_method_in
(const string & the_method)
{
  //
  // set the class which triggered the exception.
  //
  {
    method_ = the_method;
  }
}

void
astrolabe_exception::
set_description
(const string & the_description)
{
  //
  // set the exception description.
  //
  {
    description_ = the_description;
  }
}

string
astrolabe_exception::
_description
(void)
const
{
  string result;
  //
  // get the exception description.
  //
  {
    result = description_;
  }
  return result;
}

string
astrolabe_exception::
_file
(void)
const
{
  string result;
  //
  // get the source code file in which the exception was triggered
  //
  {
    result = file_;
  }
  return result;
}

void
astrolabe_exception::
set_file
(const string & the_file)
{
  //
  // set the source code file in which the exception was triggered
  //
  {
    file_ = the_file;
  }
}

int
astrolabe_exception::
_line
(void)
const
{
  int result;
  //
  // get the [approx.] line number of the source code file in which the exception was triggered
  //
  {
    result = line_;
  }
  return result;
}

void
astrolabe_exception::
set_line
(int the_line)
{
  //
  // set the [approx.] line number of the source code file in which the exception was triggered
  //
  {
    line_ = the_line;
  }
}

string
astrolabe_exception::
_severity
(void)
const
{
  string result;
  {
    if(severity_ == 0)
      result = "INFORMATIONAL";
    else
    {
      if(severity_ == 1000)
        result = "WARNING";
      else
      {
        if(severity_ == 2000)
          result = "FATAL ERROR";
        else
          result = "No specific severity has been given";
      }
    }
  }
  return result;
}

void
astrolabe_exception::
set_severity
(int the_sever)
{
  {
    severity_ = the_sever;
  }
}

void
astrolabe_exception::
set_time
(const astrolabe_time Time)
{
  {
    yy_ = Time.year();
    mm_ = Time.month();
    dd_ = Time.day();
    ho_ = Time.hour();
    mi_ = Time.minute();
    se_ = Time.second();
  }
}

void
astrolabe_exception::
_time
(astrolabe_time & Time)
const
{
  {
    Time.set(yy_,mm_,dd_,ho_,mi_,se_);
  }
}

string
astrolabe_exception::
_name_code
(void)
const
{
  string result;
  {
    result = "astrolabe_exception";
  }
  return result;
}

int
astrolabe_exception::
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



