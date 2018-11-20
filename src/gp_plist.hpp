/** \file gp_plist.hpp
  \brief Entity class to hold the \<p_list\> element in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_PLIST_HPP_
#define _GP_PLIST_HPP_

#include "astrolabe_exceptions.hpp"
#include "gp_plist_item.hpp"
#include <map>

using namespace std;

/// \brief Entity class to hold the \<p_list\> element in ASTROLABE metadata files.

class gp_plist
{
  public:

    /// \brief Add a new gp_plist_item object.
    /**
      \pre(!exists_item(item.get_n()))
      \pre ((item.get_n() > 0) & (item.get_n() <= get_dim()))
      \param item The gp_plist_item object to add.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.
             
      Add a new gp_plist_item object. Note that there may not exist other
      gp_plist_item objects already inserted whose "n" attribute has the
      same value than the one about to be inserted.
      
      Moreover, the value of the "n" attribute of the gp_plist_item object
      to be inserted must be in the range 1 to the dimension of the
      gp_plist.
    */

    void           add_item         (gp_plist_item& item);
    
    /// \brief Check if a gp_plist_item with a given "n" attribute exist.
    /**
      \param n The value of the n attribute.
      \return True if a gp_plist_item with an "n" attribute whose value
             is equal to the input parameter n exists, false otherwise.
    */
    
    bool           exists_item      (int n) const;

    /// \brief Retrieve the gp_plist_item object whose "n" attribute is equal
    /// to the input parameter n.
    /**
      \pre (exists_item(n))
      \param n The value of the "n" attribute.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    gp_plist_item  get_item         (int n);

    /// \brief Retrieve the stated dimension of the gp_plist.
    /**
      \return The dimension of the gp_plist as stated by method set_dim().

      This dimension may not be the same than the actual number of
      gp_plist_item objects stored in the gp_plist, which is returned by
      method get_size().
      
    */

    int            get_dim          (void);

    /// \brief Retrieve the actual number of gp_plist_item objects stored
    ///  in the gp_plist.
    /**
      \return The actual number of gp_plist_item objects stored.

      The number of gp_plist_item objects stored in the gp_plist may not
      be the same than the dimension stated by get_dim(). This is so
      because the gp_plist is loaded gradually with gp_plist_item objects.

      For the gp_plist to be complete (and correct), these two
      amounts must coincide once the load process (see add_item())
      is over.
    */

    int            get_size         (void);

    ///

    /// \brief Default constructor.

                   gp_plist         (void);

    /// \brief Destructor.

                   ~gp_plist        (void);
              
    /// \brief Set the actual dimension of the gp_plist.
    /**
      \pre (dimension > 0)
      \pre (get_dim() == 0)
      \param dimension The dimension of the gp_plist.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
             
     Set the dimension (number of gp_plist_item elements) of the gp_plist.
     This method may be invoked only once. That is, once the dimension
     is set, it can not be set again.
     
    */
    
    void           set_dim          (int dimension);

  protected:

    /// \brief The declared dimension of the gp_plist.

    int                     dimension_;
  
    /// \brief The map containing the different gp_plist_item objects.
    
    map<int, gp_plist_item> items_;
};

#endif // _GP_PLIST_HPP_
