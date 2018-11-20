/** \file astrolabe_postcondition_violation.hpp
\brief ASTROLABE's postcondition violation exception.
\ingroup ASTROLABE_exceptions_group
*/

#ifndef ASTROLABE_POSTCONDITION_VIOLATION_HPP
#define ASTROLABE_POSTCONDITION_VIOLATION_HPP

#include "astrolabe_contract_violation.hpp"

class astrolabe_time;

/**
  \brief ASTROLABE's postcondition violation exception.
  \ingroup ASTROLABE_exceptions_group
*/

class astrolabe_postcondition_violation : public astrolabe_contract_violation
{

  public:

    /// \brief Default constructor.

    astrolabe_postcondition_violation                 (void);

    /// \brief Copy constructor.
    /**
      \param other The other exception from which the values
             will be copied.
    */

    astrolabe_postcondition_violation                 (const astrolabe_postcondition_violation & other);

    /// \brief Destructor.

    virtual        ~astrolabe_postcondition_violation (void);

  public:

    /// \brief Get the description of the exception.
    /**
      \return The description of the exception.
    */
    virtual string     _description                   (void) const;

    /// \brief Set the description of the exception.
    /**
      \param the_postcondition The description of the exception.
    */

    virtual void    set_description                   (const string & the_postcondition);

  public:

    /// \brief Print the exception.
    /**
      \param the_stream The stream to print the exception to.
    */

    virtual  void   print                             (ostream & the_stream) const;

    /// \brief Retrieve the exception's alphanumeric code.
    /**
      \return The exception's alphanumeric code.
    */

    virtual string _name_code                         (void) const;

    /// \brief Retrieve the exception's numeric code.
    /**
      \return The exception's numeric code.
    */

    virtual int    _num_code                          (void) const;
};

ostream & operator << (ostream & result,const astrolabe_postcondition_violation & the_exception);

#endif
