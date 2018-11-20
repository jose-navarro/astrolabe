/** \file astrolabe_precondition_violation.hpp
\brief ASTROLABE's precondition violation exception.
\ingroup ASTROLABE_exceptions_group
*/

#ifndef ASTROLABE_PRECONDITION_VIOLATION_HPP
#define ASTROLABE_PRECONDITION_VIOLATION_HPP

#include "astrolabe_contract_violation.hpp"

class astrolabe_time;

/**
  \brief ASTROLABE's precondition violation exception.
*/

class astrolabe_precondition_violation : public astrolabe_contract_violation
{

  public:

    /// \brief Default constructor.

                    astrolabe_precondition_violation (void);

    /// \brief Copy constructor.
    /**
      \param other The other exception from which the values
             will be copied.
    */

                    astrolabe_precondition_violation (const astrolabe_precondition_violation & other);

    /// \brief Destructor.

    virtual        ~astrolabe_precondition_violation (void);

  public:

    /// \brief Get the description of the exception.
    /**
      \return The description of the exception.
    */
    virtual string     _description (void) const;

    /// \brief Set the description of the exception.
    /**
      \param the_precondition The description of the exception.
    */

    virtual void    set_description (const string & the_precondition);

    /// \brief Print the exception.
    /**
      \param the_stream The stream to print the exception to.
    */

    virtual void    print           (ostream & the_stream) const;

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
};

ostream & operator << (ostream & result, const astrolabe_precondition_violation & the_exception);

#endif
