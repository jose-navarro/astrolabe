/** \file gp_mspec.hpp
  \brief Entity class to hold the \<m_spec\> element in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_MSPEC_HPP_
#define _GP_MSPEC_HPP_

#include "astrolabe_exceptions.hpp"

#include "gp_ilist.hpp"
#include "gp_llist.hpp"
#include "gp_plist.hpp"
#include "gp_sub_m_list.hpp"

#include "lineage_data.hpp"

#include <string>
using namespace std;

/// \brief Entity class to hold the \<m_spec\> element in ASTROLABE metadata files.
/**
  A gp_mspec object is composed of several members. Some of these (gp_ilist,
  gp_llist, gp_plist, gp_sub_m_list and lineage_data) are also objects on
  their own.

  To avoid an unnecessary complicated API for this class, methods have been
  provided to retrieve direct pointers to these members. Therefore, methods
  adhering to the following naming pattern:

  get_xxx_ptr

  may be used to obtain such pointers. The methods of the respective members
  may thus be accessed using the pointers retrieved in this way.

  Note that calling modules using the get_xxx_ptr methods MUST NEVER DELETE
  the object thus retrieved. Doing so, would corrupt the gp_mspec object
  that contains these, which would produce a crash afterwards.
*/

class gp_mspec
{
  public:

    /// \brief Check if the \<m_spec\> element is active (s attribute).
    /**
      \return True if the \<m_spec\> is active (s attribute value "a")
              or false otherwise (s attribute value "r").
    */

    bool           get_active         (void);

    /// \brief Retrieve the value of the  \<dynamic\> element.
    /**
      \return True of false, depending on the value of the \<dynamic\>
              attribute.
    */

    bool           get_dynamic        (void);

    /// \brief Retrieve a pointer to the gp_ilist object included
    /// in the gp_mspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_ilist object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_mspec object and this could crash the application.
    */

    gp_ilist*      get_ilist_ptr      (void);

    /// \brief Retrieve a pointer to the lineage_data object included
    /// in the gp_mspec object.
    /**
      This method provides with an easy way to access the methods of
      the lineage_data object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_mspec object and this could crash the application.
    */

    lineage_data*  get_lineage_ptr    (void);

    /// \brief Retrieve a pointer to the gp_llist object included
    /// in the gp_mspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_llist object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_pspec_osi object and this could crash the application.
    */

    gp_llist*      get_llist_ptr      (void);

    /// \brief Retrieve a pointer to the gp_plist object included
    /// in the gp_mspec object.
    /**
      This method provides with an easy way to access the methods of
      the pspec object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_mspec object and this could crash the application.
    */

    gp_plist*      get_plist_ptr      (void);

    /// \brief Retrieve a pointer to the gp_sub_m_list object included
    /// in the gp_mspec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_sub_m_list object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_mspec object and this could crash the application.
    */

    gp_sub_m_list* get_sub_m_list_ptr (void);

    /// \brief Retrieve the toolbox (element \<toolbox\>) of the gp_pspec_osi object.
    /**

      Note that the toolbox returned may be empty if never set!
    */

    string         get_toolbox        (void);

    /// \brief Retrieve the type (element \<type\>) of the gp_mspec object.
    /**

      Note that the type returned may be empty if never set!
    */

    string         get_type           (void);

    /// \brief Default constructor.

                   gp_mspec           (void);

    /// \brief Destructor.

                   ~gp_mspec          (void);

    /// \brief Set the active flag (s attribute) of the \<m_spec\> element.
    /**
      \param active The active flag. Set it to true to state that the
             \<m_spec\> element is active, false otherwise.
    */

    void           set_active         (bool active);

    /// \brief Set the dynamic flag (element \<dynamic\>) of the gp_mspec object.
    /**
      \param dynamic The dynamic flag. Set it to true to state that the model
             represented by the \<m_spec\> element is dynamic, false otherwise.
    */

    void           set_dynamic        (bool dynamic);


    /// \brief Set the toolbox (element \<toolbox\>) of the gp_mspec object.
    /**
      \pre (!toolbox.empty())
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Note that the toolbox may not be an empty string.
    */

    void           set_toolbox        (string& toolbox);

    /// \brief Set the type (element \<type\>) of the gp_mspec object.
    /**
      \pre (!type.empty())
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Note that the type may not be an empty string.
    */

    void           set_type           (string& type);

  protected:

    /// \brief The "s" attribute (active / removed).

    bool           active_;

    /// \brief The \<dynamic\> element.

    bool           dynamic_;

    /// \brief The \<i_spec\> element.

    gp_ilist       ilist_;

    /// \brief The \<l_spec\> element.

    gp_llist       llist_;

    /// \brief The \<p_spec\> element.

    gp_plist       plist_;

    /// \brief The \<lineage\> element.

    lineage_data   lineage_;

    /// \brief The \<sub-m_list\> element.

    gp_sub_m_list  sub_m_list_;

    /// \brief The \<toolbox\> element.

    string         toolbox_;

    /// \brief The \<type\> element.

    string         type_;

};

#endif // _GP_MSPEC_HPP_
