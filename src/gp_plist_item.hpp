/** \file gp_plist_item.hpp
  \brief Entity class to hold the \<item\> element included in a
         \<p_list\> element in ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_PLIST_ITEM_HPP_
#define _GP_PLIST_ITEM_HPP_

#include "astrolabe_exceptions.hpp"
#include <algorithm>
#include <string>
using namespace std;

/// \brief Entity class to hold the \<item\> element included in a \<p_list\> element in ASTROLABE metadata files.

class gp_plist_item
{
  public:
  
    /// \brief Retrieve the identifier (element \<id\>) of the gp_plist_item object.
    /**
      \return The identifier.

      Note that the type returned may be empty if never set!
    */

    string get_id         (void);

	
    /// \brief Retrieves the value of the "n" attribute present in
	  /// the \<item\> element.
    /**
      \return The value of the "n" attribute.
    */
	
     int   get_n          (void) const;

     /// \brief Retrieve the role (element \<role\>) of the gp_plist_item object.
    /**
      \return The role. It will always be either "free" or "constant" (lowercase).

      Note that the type returned may be empty if never set!
    */

    string get_role       (void);

    /// \brief Default constructor.

           gp_plist_item  (void);

    /// \brief Destructor.

           ~gp_plist_item (void);

    /// \brief Set the identifier (element \<id\>) of the gp_plist_item object.
    /**
      \pre (!identifier.empty())
      \param identifier The value of the identifier to set.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Note that the identifier may not be the empty string.

    */

    void   set_id         (string& identifier);
	
	/// \brief Set the value of the "n" attribute present in the
	/// \<item\> element.
	/**
    \pre (n > 0)
    \param n The value of the "n" attribute.
	  \throw astrolabe_precondition_violation One or more preconditions have
             been violated.
	  
	  Note that n must be greater than 0.
	*/
	
  void     set_n          (int n);

  /// \brief Set the role (element \<role\>) of the gp_plist_item object.
    /**
      \pre ((role == "FREE") || (role == "free") || (role == "CONSTANT") || (role == "constant"))
      \param role The value of the role to set.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

    */

  void     set_role      (string& role);

  protected:
  
    /// \brief Identifier (\<id\> element).

    string                identifier_;
	
    /// \brief The "n" attribute included in the \<item\> element.
	
    int                   n_;

    /// \brief Role (\<role\> element).

    string                role_;

};

#endif // _GP_PLIST_ITEM_HPP_
