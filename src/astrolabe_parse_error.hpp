/** \file astrolabe_parse_error.hpp
\brief ASTROLABE's parsing error exception.
\ingroup ASTROLABE_exceptions_group
*/

#ifndef ASTROLABE_PARSE_ERROR_HPP
#define ASTROLABE_PARSE_ERROR_HPP

#include "astrolabe_IO.hpp"
class astrolabe_time;

/**
  \brief ASTROLABE's parsing error exception.
  \ingroup ASTROLABE_exceptions_group
*/

class astrolabe_parse_error : public astrolabe_IO
{

  public:

    /// \brief Default constructor.

    astrolabe_parse_error       (void);

    /// \brief Constructor, setting the device causing the exception.
    /**
      \param the_device String defining the device causing the exception.
    */

    astrolabe_parse_error       (const string & the_device);

    /// \brief Copy constructor.
    /**
      \param other The other exception from which the values
      will be copied.
    */

    astrolabe_parse_error       (const astrolabe_parse_error & other);

    /// \brief Destructor.

    virtual ~astrolabe_parse_error    (void) throw ();

  public: 

    /// \brief Get the description of the exception.
    /**
      \return The description of the exception.
    */
    virtual string _description (void) const; // short description

  public:

    /// \brief Print the exception.
    /**
      \param the_stream The stream to print the exception to.
    */

    virtual void   print        (ostream & the_stream) const;

    /// \brief Retrieve the exception's alphanumeric code.
    /**
      \return The exception's alphanumeric code.
    */

    virtual string _name_code   (void) const;

    /// \brief Retrieve the exception's numeric code.
    /**
      \return The exception's numeric code.
    */
    virtual int    _num_code    (void) const;
};

ostream & operator << (ostream & result,const astrolabe_parse_error & the_exception);

#endif
