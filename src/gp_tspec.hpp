/** \file gp_tspec.hpp
  \brief Entity class to hold the \<t_spec\> element in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_TSPEC_HPP_
#define _GP_TSPEC_HPP_

#include "astrolabe_exceptions.hpp"

#include "gp_ref.hpp"
#include "gp_unit.hpp"

#include <string>
using namespace std;

/// \brief Entity class to hold the \<t_spec\> element in ASTROLABE metadata files.
/**
  A gp_tspec object is composed of several members. Some of these (gp_ref
  and gp_unit) are also objects on their own that are AUTOMATICALLY created
  when instantiating a gp_tspec object.

  To avoid an unnecessary complicated API for this class, methods have been
  provided to retrieve direct pointers to these members. Therefore, methods
  adhering to the following naming pattern:

  get_xxx_ptr()

  may be used to obtain such pointers. The methods of the respective members
  may thus be accessed using the pointers retrieved in this way.

  So simplified an interface implies that changing the values of the sub-objects
  included in the gp_tspec object must be done (a) getting a pointer
  to the sub-object that must be changed and (b) calling the specific methods
  provided by the sub-object to do so.

  For instance, to modify the gp_unit member of a gp_tspec object, the
  calling module should do the following;

  \code
  ...
  gp_tspec* my_tspec;
  gp_unit*  my_unit;
  ...
  // Get a pointer to my_tspec's internal units object.
  my_unit = my_tspec->get_unit_ptr();

  // Use the method provided by the gp_unit class to modify
  // the contents of the unit object.

  my_unit->set_dim(1);
  my_unit->set_unit(1,"m");

  // Note that the changes made have been done to the units
  // sub-object included in my_tspec.
  ...
  \endcode

  The gp_unit included in the gp_tspec object is directly modified in
  this way, since it was automatically created when instantiating the
  gp_tspec object.

  Note that calling modules using the get_xxx_ptr() methods MUST NEVER DELETE
  the object thus retrieved. Doing so, would corrupt the gp_tspec object
  that contains these, which would produce a crash afterwards.
*/

class gp_tspec
{
  public:

    /// \brief Retrieve the dimension (element \<dimension\>) of the gp_tspec object.
    /**

      Note that the dimension returned may be 0 if never set!
    */

    int            get_dimension      (void) const;

    /// \brief Retrieve a pointer to the gp_ref object included
    /// in the gp_tspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_ref object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_tspec object and this could crash the application.
    */

    gp_ref*        get_ref_ptr        (void);

    /// \brief Retrieve a pointer to the gp_unit object included
    /// in the gp_tspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_unit object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_tspec object and this could crash the application.
    */

    gp_unit*       get_unit_ptr       (void);

    /// \brief Default constructor.

                   gp_tspec           (void);

    /// \brief Destructor.

                   ~gp_tspec          (void);

    /// \brief Set the dimension (element \<dimension\>) of the gp_tspec object.
    /**
      \pre (dimension > 0)
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Note that the dimension must be greater than or equal to 0.
    */

    void           set_dimension      (int dimension);

  protected:

    /// \brief The \<dimension\> element.

    int           dimension_;

    /// \brief The \<ref\> element (coordinate reference frame / reference frame plus
    /// coordinate system).

    gp_ref        ref_;

    /// \brief The \<units\> element (units).

    gp_unit       units_;


};

#endif // _GP_TSPEC_HPP_
