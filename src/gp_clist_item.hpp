/** \file gp_clist_item.hpp
  \brief Entity class to hold the \<item\> element included in the
         \<c_list\> element found in ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_CLIST_ITEM_HPP_
#define _GP_CLIST_ITEM_HPP_

#include "astrolabe_exceptions.hpp"

#include "gp_covariance.hpp"
#include "gp_ref.hpp"
#include "gp_scale.hpp"
#include "gp_unit.hpp"
#include "gp_clist_item_type.hpp"

#include <string>
using namespace std;

/// \brief Entity class to hold the \<item\> element included in the \<c_list\> element
///        found in ASTROLABE metadata files.
/**
  A gp_clist_item object is composed of several members. Some of these (gp_covariance,
  gp_ref, gp_scale, gp_unit and gp_clist_item_type) are also objects on their own that
  are AUTOMATICALLY created when instantiating a gp_clist_item object.

  To avoid an unnecessary complicated API for this class, methods have been
  provided to retrieve direct pointers to these members. Therefore, methods
  adhering to the following naming pattern:

  get_xxx_ptr()

  may be used to obtain such pointers. The methods of the respective members
  may thus be accessed using the pointers retrieved in this way.

  So simplified an interface implies that changing the values of the sub-objects
  included in the gp_clist_item object must be done (a) getting a pointer
  to the sub-object that must be changed and (b) calling the specific methods
  provided by the sub-object to do so.

  For instance, to modify the gp_unit member of a gp_clist_item object, the
  calling module should do the following;

  \code
  ...
  gp_clist_item* my_citem;
  gp_unit*       my_unit;
  ...
  // Get a pointer to my_citem's internal units object.
  my_unit = my_citem->get_unit_ptr();

  // Use the method provided by the gp_unit class to modify
  // the contents of the unit object.

  my_unit->set_dim(1);
  my_unit->set_unit(1,"m");

  // Note that the changes made have been done to the units
  // sub-object included in my_citem.
  ...
  \endcode

  The gp_unit included in the gp_clist_item object is directly modified in
  this way, since it was automatically created when instantiating the
  gp_clist_item object.

  Note that calling modules using the get_xxx_ptr() methods MUST NEVER DELETE
  the object thus retrieved. Doing so, would corrupt the gp_clist_item object
  that contains these, which would produce a crash afterwards.
*/

class gp_clist_item
{
  public:

    /// \brief Check that all the dimensions in a MATRIX type are valid.
    /**
      \param dimensions The array with the dimensions to check.
      \return True if all the dimensions in the dimensions array are valid,
      that is, all of them are > 1.

      The MATRIX type is accompanied by a set of dimension values. All these
      must be strictly greater than one. This method checks this condition.

      It is used mainly in preconditions.
    */

    bool                dimensions_are_valid (vector<int>& dimensions) const;
  
    /// \brief Retrieve a pointer to the type (element \<type\>) of the
    ///        gp_clist_item object.
    /**

      Note that the type returned may be empty if never set!
    */

    gp_clist_item_type* get_type_ptr         (void);

    /// \brief Retrieve a pointer to the gp_covariance object included
    /// in the gp_clist_item object.
    /**
      This method provides with an easy way to access the methods of
      the covariance object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_clist_item object and this could crash the application.
    */

    gp_covariance*      get_covariance_ptr   (void);
	
    /// \brief Retrieves the value of the "n" attribute present in
	  /// the \<item\> element.
	
     int                get_n                (void) const;

    /// \brief Retrieve a pointer to the gp_ref object included
    /// in the gp_clist_item object.
    /**
      This method provides with an easy way to access the methods of
      the gp_ref object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_clist_item object and this could crash the application.
    */

    gp_ref*             get_ref_ptr          (void);

    /// \brief Retrieve a pointer to the gp_scale object included
    /// in the gp_clist_item object.
    /**
      This method provides with an easy way to access the methods of
      the gp_scale object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_clist_item object and this could crash the application.
    */

    gp_scale*           get_scale_ptr        (void);

    /// \brief Retrieve a pointer to the gp_unit object included
    /// in the gp_clist_item object.
    /**
      This method provides with an easy way to access the methods of
      the gp_unit object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_clist_item object and this could crash the application.
    */

    gp_unit*            get_unit_ptr         (void);

    /// \brief Default constructor.

                        gp_clist_item        (void);

    /// \brief Destructor.

                        ~gp_clist_item       (void);
	
	/// \brief Set the value of the "n" attribute present in the
	/// \<item\> element.
	/**
	  \pre (n > 0)
	  \throw astrolabe_precondition_violation One or more preconditions have
	         been violated.
	  
	  Note that n must be greater than 0.
	*/
	
	void                  set_n                 (int n);

  /// \brief Set the type (element \<type\>) of the gp_clist_item object
  /// to "MATRIX", including its dimensions.
  /**
    \pre (dimensions_are_valid(dimensions))
    \param dimensions The different dimensions defining the MATRIX type.
    \throw astrolabe_precondition_violation One or more preconditions have
           been violated.

    Note that the dimensions array must have at least one element, and that
    all its elements must contain values > 1.
  */

  void                  set_type_to_matrix (vector<int>& dimensions);

  protected:

    /// \brief The \<c\> element (covariance matrix).

    gp_covariance      covariance_;
	
    /// \brief The "n" attribute included in the \<item\> element.
	
    int                n_;

    /// \brief The \<ref\> element (coordinate reference frame / reference frame plus
    /// coordinate system).

    gp_ref             ref_;

    /// \brief The \<s\> element (scale).

    gp_scale           scale_;

    /// \brief The \<type\> element.

    gp_clist_item_type type_;

    /// \brief The \<units\> element (units).

    gp_unit            units_;

};

#endif // _GP_CLIST_ITEM_HPP_
