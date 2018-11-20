/** \file astrolabe_IO.hpp
\brief ASTROLABE's input / output exception.
\ingroup ASTROLABE_exceptions_group
*/

#ifndef ASTROLABE_IO_HPP
#define ASTROLABE_IO_HPP

#include "astrolabe_exception.hpp"

class astrolabe_time;

/**
  \brief ASTROLABE's input / output exception.
  \ingroup ASTROLABE_exceptions_group
*/

class astrolabe_IO : public astrolabe_exception
{
  public:

    /// \brief Default constructor.

             astrolabe_IO           (void);

    /// \brief Constructor, setting the device causing the exception.
    /**
      \param the_device String defining the device causing the exception.
    */

             astrolabe_IO           (const string & the_device);

             /// \brief Copy constructor.
    /**
      \param other The other exception from which the values
             will be copied.
    */

             astrolabe_IO           (const astrolabe_IO & other);

    /// \brief Destructor.

    virtual ~astrolabe_IO           (void) throw();

  public:

    /// \brief Get the description of the exception.
    /**
      \return The description of the exception.
    */

    virtual  string    _description (void) const;

    /// \brief Get the device causing the exception.
    /**
      \return The device causing the exception.
    */

    virtual  string    _device      (void) const;

    /// \brief Set the device causing the exception.
    /**
      \param the_device The device causing the exception.
    */
    virtual  void   set_device      (const string & the_device);

  public:

    /// \brief Print the exception.
    /**
      \param the_stream The stream to print the exception to.
    */

    virtual void    print           (ostream & the_stream) const;

    /// \brief Retrieve the exception's alphanumeric code.
    /**
      \return The exception's alphanumeric code.
    */
    virtual string _name_code       (void) const;

    /// \brief Retrieve the exception's numeric code.
    /**
      \return The exception's numeric code.
    */

    virtual int    _num_code        (void) const;

  protected:

    /// \brief The device causing the exception.

    string device_;
};

ostream & operator << (ostream & result,const astrolabe_IO & the_exception);

#endif
