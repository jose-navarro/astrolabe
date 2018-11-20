/** \file gp_metadata.hpp
  \brief Entity class to hold the metadata information
         found in ASTROLABE navigation metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_MEDATATA_HPP_
#define _GP_MEDATATA_HPP_

#include "astrolabe_exceptions.hpp"

#include "gp_ispec.hpp"
#include "gp_lspec.hpp"
#include "gp_mspec.hpp"
#include "gp_pspec.hpp"

#include <vector>
using namespace std;

/// \brief Entity class to hold metadata information for ASTROLABE metadata files.

class gp_metadata
{
  public:

    /// \brief Add a new gp_ispec object.
    /**
      \param ispec The gp_ispec object to add.
    */

    void         add_ispec        (gp_ispec& ispec);

    /// \brief Add a new gp_lspec object.
    /**
      \param lspec The gp_lspec object to add.
    */

    void         add_lspec        (gp_lspec& lspec);

    /// \brief Add a new gp_mspec object.
    /**
      \param mspec The gp_mspec object to add.
    */

    void         add_mspec        (gp_mspec& mspec);

    /// \brief Add a new gp_pspec object.
    /**
      \param pspec The gp_pspec object to add.
    */

    void         add_pspec        (gp_pspec& pspec);

    /// \brief Retrieve the number of available gp_ispec objects.

    int          get_dim_ispec    (void);

    /// \brief Retrieve the number of available gp_lspec objects.

    int          get_dim_lspec    (void);

    /// \brief Retrieve the number of available gp_mspec objects.

    int          get_dim_mspec    (void);

    /// \brief Retrieve the number of available gp_pspec objects.

    int          get_dim_pspec    (void);

    /// \brief Retrieve the gp_ispec object at a given position.
    /**
      \pre ((pos >= 0) && (pos < get_dim_ispec()))
      \param pos The ordinal (based at zero) of the gp_ispec object to retrieve.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    gp_ispec     get_ispec        (int pos);

    /// \brief Retrieve the gp_lspec object at a given position.
    /**
      \pre ((pos >= 0) && (pos < get_dim_lspec()))
      \param pos The ordinal (based at zero) of the gp_lspec object to retrieve.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    gp_lspec     get_lspec        (int pos);

    /// \brief Retrieve the gp_mspec object at a given position.
    /**
      \pre ((pos >= 0) && (pos < get_dim_mspec()))
      \param pos The ordinal (based at zero) of the gp_mspec object to retrieve.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    gp_mspec     get_mspec        (int pos);

    /// \brief Retrieve the gp_pspec object at a given position.
    /**
      \pre ((pos >= 0) && (pos < get_dim_pspec()))
      \param pos The ordinal (based at zero) of the gp_pspec object to retrieve.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    gp_pspec     get_pspec        (int pos);

    /// \brief Default constructor.

                 gp_metadata      (void);

    /// \brief Destructor.

                 ~gp_metadata     (void);

  protected:

    /// \brief The list of gp_ispec objects.

    vector<gp_ispec>    ispecs_;

    /// \brief The list of gp_lspec objects.

    vector<gp_lspec>    lspecs_;

    /// \brief The list of gp_mspec objects.

    vector<gp_mspec>    mspecs_;

    /// \brief The list of gp_pspec objects.

    vector<gp_pspec>    pspecs_;
  };

#endif // _GP_MEDATATA_HPP_
