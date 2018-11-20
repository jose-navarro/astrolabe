/** \file gp_units_manager.hpp
\brief Units validator for ASTROLABE metadata
\ingroup ASTROLABE_metadata
*/

#ifndef _GP_UNITS_MANAGER_HPP_
#define _GP_UNITS_MANAGER_HPP_

#include <string>
#include "udunits2.h"

using namespace std;

/// \brief Units validator for ASTROLABE metadata
/**
  This class provides the necessary services to check the validity of the
  units written in ASTROLABE metadata files.

  It relies in the UDUNITS2 library (and therefore, in the EXPAT library too).
*/

class gp_units_manager
{
  public:

    /// \brief Default constructor.

         gp_units_manager        (void);

    /// \brief Destructor

         ~gp_units_manager       (void);

    /// \brief Check if the gp_units_manager object is valid and therefore
    /// ready to be used. Used mainly in preconditions.

    bool is_valid                (void) const;

    /// \brief Set the path and file name of the UDUNITS2 file
    /// containing the units database. Load immediately the
    /// units database into memory.
    /**
      \pre (!database_path.empty())
      \param database_path Full path and filename of the UDUNITS2 file
             containing the units database.
      \post (is_valid())
      \return An error code. 0 stands for successful completion, 1 means
             that one or more preconditions have been violated and 2
             means that the units database could not be loaded.

      Since this class uses the UDUNITS2 package to validate units,
      such package must be initialized indicating the absolute location
      of the file defining the units database (a.k.a. the units system).

      Use this method to provide such filename. 

      Note that this method also tries to load the units database into
      memory so other methods may work properly. This may fail (see the
      error codes).

      When the method ends successfully, the gp_units_manager becomes
      valid automatically, so other methods requiring such condition
      may be used. If this method fails, then the gp_units_manager
      remains invalid.
    */
    int  set_units_database_path (string& database_path);

    /// \brief Check that a unit in string format adheres to the
    /// required syntax.
    /**
      \pre (!unit_to_validate.empty())
      \pre (is_valid())
      \param unit_to_validate A string containing a unit representation according to
             the syntax accepted by the UDUNITS2 package. Leading or trailing whitespace
             may not exist (it would lead to an erroneous validation of the unit).
      \return An error code. 0 means successful completion, so the input unit is valid.
             1 means that one or more preconditions have been violated and 2 that the
             unit is not valid (either because it does not adhere to the UDUNITS2 syntax
             or because there are leading or trailing whitespace in the input string).
    */

    int  validate_unit           (string& unit_to_validate);

  protected:

    /// \brief Path and filename of the UDUNITS2 file containing the
    /// units database.

    string     path_to_units_system_;

    /// \brief The UDUNITS2 database loaded as an object (the
    /// "units system").

    ut_system* units_system_;

    /// \brief Flag stating whether the gp_units_manager object is
    /// valid, that is, ready to validate units.

    bool       valid_;


};

#endif // _GP_UNITS_MANAGER_HPP_
