/** \file gp_lspec.hpp
  \brief Entity class to hold the \<l_spec\> element in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_LSPEC_HPP_
#define _GP_LSPEC_HPP_

#include "astrolabe_exceptions.hpp"

#include "gp_covariance.hpp"
#include "gp_timespec.hpp"
#include "gp_ref.hpp"
#include "gp_scale.hpp"
#include "gp_tspec.hpp"
#include "gp_unit.hpp"
#include "lineage_data.hpp"

#include <string>
using namespace std;

/// \brief Entity class to hold the \<l_spec\> element in ASTROLABE metadata files.
/**
  A gp_lspec object is composed of several members. Some of these (gp_covariance,
  gp_timespec, gp_ref, gp_scale, gp_unit and gp_tspec) are also objects on their own that
  are AUTOMATICALLY created when instantiating a gp_lspec object.

  To avoid an unnecessary complicated API for this class, methods have been
  provided to retrieve direct pointers to these members. Therefore, methods
  adhering to the following naming pattern:

  get_xxx_ptr()

  may be used to obtain such pointers. The methods of the respective members
  may thus be accessed using the pointers retrieved in this way.

  So simplified an interface implies that changing the values of the sub-objects
  included in the gp_lspec object must be done (a) getting a pointer
  to the sub-object that must be changed and (b) calling the specific methods
  provided by the sub-object to do so.

  For instance, to modify the gp_unit member of a gp_lspec object, the
  calling module should do the following;

  \code
  ...
  gp_lspec* my_lspec;
  gp_unit*  my_unit;
  ...
  // Get a pointer to my_lspec's internal units object.
  my_unit = my_lspec->get_unit_ptr();

  // Use the method provided by the gp_unit class to modify
  // the contents of the unit object.

  my_unit->set_dim(1);
  my_unit->set_unit(1,"m");

  // Note that the changes made have been done to the units
  // sub-object included in my_lspec.
  ...
  \endcode

  The gp_unit included in the gp_lspec object is directly modified in
  this way, since it was automatically created when instantiating the
  gp_lspec object.

  Note that calling modules using the get_xxx_ptr() methods MUST NEVER DELETE
  the object thus retrieved. Doing so, would corrupt the gp_lspec object
  that contains these, which would produce a crash afterwards.
*/

class gp_lspec
{
  public:

    /// \brief Check if the \<l_spec\> element is active (s attribute).
    /**
      \return True if the \<l_spec\> is active (s attribute value "a")
              or false otherwise (s attribute value "r").
    */

    bool           get_active         (void);

    /// \brief Retrieve a pointer to the gp_covariance object included
    /// in the gp_lspec object.
    /**
      This method provides with an easy way to access the methods of
      the covariance object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_lspec object and this could crash the application.
    */

    gp_covariance* get_covariance_ptr (void);

    /// \brief Retrieve the dimension (element \<dimension\>) of the gp_lspec object.
    /**

      Note that the dimension returned may be 0 if never set!
    */

    int            get_dimension      (void) const;


    /// \brief Retrieve a pointer to the lineage_data object included
    /// in the gp_lspec object.
    /**
      This method provides with an easy way to access the methods of
      the lineage_data object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_lspec object and this could crash the application.
    */

    lineage_data*  get_lineage_ptr     (void);

    /// \brief Retrieve a pointer to the gp_ref object included
    /// in the gp_lspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_ref object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_lspec object and this could crash the application.
    */

    gp_ref*        get_ref_ptr        (void);

    /// \brief Retrieve a pointer to the gp_scale object included
    /// in the gp_lspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_scale object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_lspec object and this could crash the application.
    */

    gp_scale*      get_scale_ptr      (void);

    /// \brief Retrieve a pointer to the gp_timespec object included
    /// in the gp_lspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_timespec object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_lspec object and this could crash the application.
    */

    gp_timespec*   get_timespec_ptr   (void);

    /// \brief Retrieve the toolbox (element \<toolbox\>) of the gp_lspec object.
    /**

      Note that the toolbox returned may be empty if never set!
    */

    string         get_toolbox        (void);

    /// \brief Retrieve a pointer to the gp_tspec object included
    /// in the gp_lspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_tspec object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_lspec object and this could crash the application.
    */

    gp_tspec*      get_tspec_ptr      (void);

    /// \brief Retrieve the type (element \<type\>) of the gp_lspec object.
    /**

      Note that the type returned may be empty if never set!
    */

    string         get_type           (void);

    /// \brief Retrieve a pointer to the gp_unit object included
    /// in the gp_lspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_unit object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_lspec object and this could crash the application.
    */

    gp_unit*       get_unit_ptr       (void);

    /// \brief Default constructor.

                   gp_lspec           (void);

    /// \brief Destructor.

                   ~gp_lspec          (void);

    /// \brief Set the active flag (s attribute) of the \<l_spec\> element.
    /**
      \param active The active flag. Set it to true to state that the
             \<l_spec\> element is active, false otherwise.
    */

    void           set_active         (bool active);

    /// \brief Set the dimension (element \<dimension\>) of the gp_lspec object.
    /**
      \pre (dimension > 0)
      \throw astrolabe_precondition_violation One or more preconditions have
           been violated.

      Note that the dimension must be greater than or equal to 0.
    */

    void           set_dimension      (int dimension);


    /// \brief Set the toolbox (element \<toolbox\>) of the gp_lspec object.
    /**
      \pre (!toolbox.empty())
      \throw astrolabe_precondition_violation One or more preconditions have
           been violated.

      Note that the toolbox may not be an empty string.
    */

    void           set_toolbox        (string& toolbox);

    /// \brief Set the type (element \<type\>) of the gp_lspec object.
    /**
      \pre (!type.empty())
      \throw astrolabe_precondition_violation One or more preconditions have
           been violated.

      Note that the type may not be an empty string.
    */

    void           set_type           (string& type);

  protected:

    /// \brief The "s" attribute (active / removed).

    bool          active_;

    /// \brief The \<c\> element (covariance matrix).

    gp_covariance covariance_;

    /// \brief The \<dimension\> element.

    int           dimension_;

    /// \brief The \<lineage\> element.

    lineage_data  lineage_;

    /// \brief The \<ref\> element (coordinate reference frame / reference frame plus
    /// coordinate system).

    gp_ref        ref_;

    /// \brief The \<s\> element (scale).

    gp_scale      scale_;

    /// \brief The \<time_spec\> element.

    gp_timespec   timespec_;

    /// \brief The \<toolbox\> element.

    string        toolbox_;

        /// \brief The \<t_spec\> element.

    gp_tspec      tspec_;

    /// \brief The \<type\> element.

    string        type_;

    /// \brief The \<units\> element (units).

    gp_unit       units_;


};

#endif // _GP_LSPEC_HPP_
