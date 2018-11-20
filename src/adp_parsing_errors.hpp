/** \file adp_parsing_errors.hpp
  \brief Implementation of a list of parsing errors, accesible by index.
  \ingroup adp_DOMparser_group
*/

#ifndef __ADP_PARSING_ERRORS_HPP__
#define __ADP_PARSING_ERRORS_HPP__

#include "astrolabe_exceptions.hpp"

#include <vector>
using namespace std;

/// \brief List of parsing errors
/**
   Implementation of a list of parsing errors, accesible by index,
   for the adp_DOMParser parser.
 */

class adp_parsing_errors
{
public:

  /// \brief Adds a new error, char* version.
  /**
    \param new_error The error to add to the list of errors.
    \throw e_mempager_not_enough_memory Not enough memory to
           instantiate a new error.
   */

  void             add_error           (const char* new_error);

  /// \brief Adds a new error, string version.
  /**
    \param new_error The error to add to the list of errors.
    \throw astrolabe_exception Not enough memory to
           instantiate a new error.
   */

  void             add_error           (const string new_error);

  /// \brief Retrieve the error at position i.
  /**
    \param i The index of the error to retrieve. It must be in the
             range 1 .. _size().
    \return The error stored at position i of the list of errors.
    \throw e_bs_precondition_violation One or more preconditions
           have been violated.
    \throw astrolabe_exception Not enough memory to
           instantiate the error to be returned.
   */

  astrolabe_parse_error* get_error           (const int i) const;

  /// \brief Copy constructor using a reference.
  /**
    \param original The reference to other parsing_errors object
           that has to be copied.
    \throw e_mempager_not_enough_memory Not enough memory to
           instantiate the new copied object.
  */

                   adp_parsing_errors  (const adp_parsing_errors& original);

  /// \brief Copy constructor using a pointer.
  /**
    \param original The pointer to other parsing_errors object
           that has to be copied.
    \throw e_mempager_not_enough_memory Not enough memory to
           instantiate the new copied object.
  */

                   adp_parsing_errors  (const adp_parsing_errors* original);

  /// \brief Default constructor

                   adp_parsing_errors  (void);

  /// \brief Destructor.

  virtual          ~adp_parsing_errors (void);

  /// \brief Retrieves the total number of errors present.

  int              _size               (void) const;


protected:

  /// \brief The list of errors.

  vector<astrolabe_parse_error*> errors_;

};

#endif // __PARSING_ERRORS_HPP__

