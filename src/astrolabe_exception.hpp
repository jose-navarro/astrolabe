/** \file astrolabe_exception.hpp
\brief Definition of the topmost, general ASTROLABE exception.
\ingroup ASTROLABE_exceptions_group
*/

#ifndef ASTROLABE_EXCEPTION_HPP
#define ASTROLABE_EXCEPTION_HPP

#include <stdexcept>
#include <string>
using namespace std;

class astrolabe_time;

/**
  \brief ASTROLABE's topmost, general exception.
  \ingroup ASTROLABE_exceptions_group
*/

class astrolabe_exception : public exception
{

  public:

    /// \brief Default constructor.

                    astrolabe_exception (void);

    /// \brief Copy constructor.
    /**
      \param other The other exception from which the values
             will be copied.
    */

                    astrolabe_exception (const astrolabe_exception & other);

    /// \brief Destructor.

    virtual        ~astrolabe_exception (void);


  public:

    /// \brief Print the exception.
    /**
      \param the_stream The stream to print the exception to.
    */

    virtual void    print           (ostream & the_stream) const;

    /// \brief Set the description of the exception.
    /**
      \param the_description The description of the exception.
    */

    virtual void    set_description (const string & the_description);

    /// \brief Get the description of the exception.
    /**
      \return The description of the exception.
    */

    virtual string     _description (void) const;

    /// \brief Set the name of the source file where the exception takes place.
    /**
      \param the_file The name of the file to set.
    */

    virtual void    set_file        (const string & the_file);

    /// \brief Get the name of the file where the exception takes place.
    /**
      \return The name of the file where the exception takes place.
    */

    virtual string     _file        (void) const;

    /// \brief Set the line number (in the source file) where the exception takes place.
    /**
      \param the_line The line number to set.
    */

    virtual void    set_line        (int the_line);

    /// \brief Ge the line number (in the source file) where the exception takes place.
    /**
      \return The line number (in the source file) where the exception takes place.
    */

    virtual int        _line        (void) const;

    /// \brief Set the name of the class where the exception takes place.
    /**
      \param the_class The name of the class to set.
    */

    virtual void    set_class_in    (const string & the_class);

    /// \brief Retrieve the name of the class where the exception takes place.
    /**
      \return The name of the class where the exception takes place.
    */

    virtual string     _class_in    (void) const;

    /// \brief Set the name of the method where the exception takes place.
    /**
      \param the_method The name of the method to set.
    */

    virtual void    set_method_in   (const string & the_method);

    /// \brief Retrieve the name of the method where the exception takes place.
    /**
      \return The name of the method where the exception takes place.
    */

    virtual string     _method_in   (void) const;

    /// \brief Set the severity level of the exception.
    /**
      \param The severity level to set. It must be coded as follows:

      - Severity =  000 -> INFORMATIONAL (simply information to user)
      - Severity = 1000 -> WARNING (it makes sense to continue with the execution)
      - Severity = 2000 -> FATAL ERROR (execution was broken)
    */

    virtual void    set_severity    (int the_sever);

    /// \brief Retrieve the severity level of the exception.
    /**
      \return The severity level of the exception. The meaning of the
              numerical code returned is:

              - Severity =  000 -> INFORMATIONAL (simply information to user)
              - Severity = 1000 -> WARNING (it makes sense to continue with the execution)
              - Severity = 2000 -> FATAL ERROR (execution was broken)
    */

    virtual string     _severity    (void) const;

    /// \brief Set the time when the exception takes place.
    /**
      \param Time The time when the exception takes place.
    */

    virtual void    set_time        (const astrolabe_time   Time);

    /// \brief Retrieve the time when the exception takes place.
    /**
      \param Time The object to which the time when the exception
             takes placw will be copied.
    */
    virtual void       _time        (      astrolabe_time & Time) const;

    /// \brief Retrieve the exception's alphanumeric code.
    /**
      \return The exception's alphanumeric code.
    */

    virtual string  _name_code      (void) const;

    /// \brief Retrieve the exception's numeric code.
    /**
      \return The exception's numeric code.
    */

    virtual int     _num_code       (void) const;

  protected:

    /// \brief Source file where the exception took place.

    string file_;

    /// \brief Line number within the source file where the exception took place.

    int    line_;

    /// \brief Class of the object throwing the exception.

    string class_;

    /// \brief Method of the object throwing the exception.

    string method_;

    /// \brief Short description.

    string description_;

    /// \brief Error degree.
    /**
       Three severity levels are allowed:

       - Severity =  000 -> INFORMATIONAL (simply information to user)
       - Severity = 1000 -> WARNING (it makes sense to continue with the execution)
       - Severity = 2000 -> FATAL ERROR (execution was broken)
     */

    int    severity_;

    /// \brief Time of event: year.

    int    yy_;

    /// \brief Time of event: month.

    int    mm_;

    /// \brief Time of event: day.

    int    dd_;

    /// \brief Time of event: hour.

    int    ho_;

    /// \brief Time of event: minute.

    int    mi_;

    /// \brief Time of event: second.

    long double se_;
};

ostream & operator << (ostream & result, const astrolabe_exception & the_exception);

#endif
