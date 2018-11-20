/** \file gp_unit.hpp
  \brief Entity class implementing the \<unit\> element in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_UNIT_HPP_
#define _GP_UNIT_HPP_

#include "astrolabe_exceptions.hpp"

#include <vector>
#include <string>

using namespace std;

/// \brief Entity class implementing the \<unit\> element in ASTROLABE metadata files.
/**
  The gp_unit object contains a list of single unit specifications.
*/

class gp_unit
{
  public:

    /// \brief Retrieve the number of single unit specifications that may
    /// be stored in the gp_unit object.

    int    get_dim  (void);

    /// \brief Retrieve the single unit specification at a given position.
    /**
      \pre ((pos >= 0) && (pos < get_dim()))
      \param pos The number of the single unit specification to retrieve. This
             is a zero based index, whose maximum value is get_dim() - 1.
      \return The single unit specification at position pos.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    string get_unit (int pos);

    /// \brief Default constructor.

           gp_unit  (void);

    /// \brief Destructor.

           ~gp_unit (void);

    /// \brief Set the number of individual units composing the gp_unit object.
    /**
      \pre (get_dim() == 0)
      \pre (n_units > 0)
      \param n_units Number of single unit specs to be stored in the gp_unit object.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.

      Use this method to indicate how many single unit specifications will be
      stored in the gp_unit object.

      It may be called only once (typically, after the gp_unit object has been
      instantiated); that is why the precondition states that the current
      dimension (number of single unit specs) must be zero.

      Obviously, parameter n_units must be strictly positive, so at least
      one single unit spec may be stored.
    */

    void   set_dim  (int n_units);

    /// \brief Set the single unit specification for a given position.
    /**
      \pre ((pos >= 0) && (pos < get_dim()))
      \pre (!unit_spec.empty())
      \param pos The number of the single unit specification to set. This
             is a zero based index, whose maximum value is get_dim() - 1.
      \param unit_spec The single unit specification to set. May not be
             empty.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    void   set_unit (int pos, string& unit_spec);

  protected:

    /// \brief Number of units included in the gp_unit object.

    int n_units_;

    /// \brief The list of single units making the gp_unit object.

    vector<string> units_;
  };

#endif // _GP_UNIT_HPP_

