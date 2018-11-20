/** \file astrolabe_contract_violation.hpp
\brief ASTROLABE's contract violation exception.
\ingroup ASTROLABE_exceptions_group
*/

#ifndef ASTROLABE_CONTRACT_VIOLATION_HPP
#define ASTROLABE_CONTRACT_VIOLATION_HPP

#include "astrolabe_exception.hpp"

#include <string>
using namespace std;

/**
  \brief ASTROLABE's contract violation exception.
  \ingroup ASTROLABE_exceptions_group
*/

class astrolabe_contract_violation : public astrolabe_exception
{

  public:

    /// \brief Default constructor.

                    astrolabe_contract_violation (void);

    /// \brief Copy constructor.
    /**
      \param other The other exception from which the values
             will be copied.
    */

                    astrolabe_contract_violation (const astrolabe_contract_violation & other);

    /// \brief Destructor.

    virtual        ~astrolabe_contract_violation (void);

  public:

    /// \brief Get the exception's contract.
    /**
      \return The exception's contract.
    */

    virtual string     _contract                 (void) const;

    /// \brief Set the exception's contract.
    /**
      \param the_description Description of the exception's contract.
    */
    virtual void    set_contract                 (const string & the_description);

    /// \brief Print the exception.
    /**
      \param the_stream The stream to print the exception to.
    */

    virtual void    print                        (ostream & the_stream) const;

    /// \brief Retrieve the exception's alphanumeric code.
    /**
      \return The exception's alphanumeric code.
    */

    virtual string _name_code                    (void) const;

    /// \brief Retrieve the exception's numeric code.
    /**
      \return The exception's numeric code.
    */

    virtual int    _num_code                     (void) const;

  protected:

    /// \brief The exception's contract.

    string contract_;
};

ostream & operator << (ostream & result,const astrolabe_contract_violation & the_exception);

#endif
