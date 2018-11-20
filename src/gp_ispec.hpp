/** \file gp_ispec.hpp
  \brief Entity class to hold the \<i_spec\> element in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_ISPEC_HPP_
#define _GP_ISPEC_HPP_

#include "astrolabe_exceptions.hpp"

#include "lineage_data.hpp"
#include "gp_timespec.hpp"
#include "gp_clist.hpp"
#include "gp_tspec.hpp"

#include <string>
using namespace std;

/// \brief Entity class to hold the \<i_spec\> element in ASTROLABE metadata files.

/**
  A gp_ispec object is composed of several members. Some of these (lineage,
  gp_clist, gp_timespec and gp_tspec) are also objects on their own.

  To avoid an unnecessary complicated API for this class, methods have been
  provided to retrieve direct pointers to these members. Therefore, methods
  adhering to the following naming pattern:

  get_xxx_ptr

  may be used to obtain such pointers. The methods of the respective members
  may thus be accessed using the pointers retrieved in this way.
  
  Note that calling modules using the get_xxx_ptr methods MUST NEVER DELETE
  the object thus retrieved. Doing so, would corrupt the gp_ispec object
  that contains these, which would produce a crash afterwards.
*/

class gp_ispec
{
  public:

    /// \brief Check if the \<i_spec\> element is active (s attribute).
    /**
      \return True if the \<i_spec\> is active (s attribute value "a")
              or false otherwise (s attribute value "r").
    */

    bool           get_active         (void);

    /// \brief Retrieve a pointer to the gp_clist object included
    /// in the gp_ispec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_clist object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_ispec object and this could crash the application.
    */

    gp_clist*      get_clist_ptr      (void);

    /// \brief Retrieve a pointer to the lineage_data object included
    /// in the gp_ispec object.
    /**
      This method provides with an easy way to access the methods of
      the lineage_data object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_ispec object and this could crash the application.
    */

    lineage_data*  get_lineage_ptr    (void);

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

    /// \brief Retrieve the toolbox (element \<toolbox\>) of the gp_ispec object.
    /**

      Note that the toolbox returned may be empty if never set!
    */

    string         get_toolbox        (void);

    /// \brief Retrieve a pointer to the gp_tspec object included
    /// in the gp_ispec object.
    /**
      This method provides with an easy way to access the methods of
      the gp_tspec object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_ispec object and this could crash the application.
    */

    gp_tspec*      get_tspec_ptr      (void);

    /// \brief Retrieve the type (element \<type\>) of the gp_ispec object.
    /**

      Note that the type returned may be empty if never set!
    */

    string         get_type           (void);

    /// \brief Default constructor.

                   gp_ispec           (void);

    /// \brief Destructor.

                   ~gp_ispec          (void);

    /// \brief Set the active flag (s attribute) of the \<i_spec\> element.
    /**
      \param active The active flag. Set it to true to state that the
             \<i_spec\> element is active, false otherwise.
    */

    void           set_active         (bool active);

    /// \brief Set the toolbox (element \<toolbox\>) of the gp_ispec object.
    /**
      \pre (!toolbox.empty())
      \throw astrolabe_precondition_violation One or more preconditions have
           been violated.

      Note that the toolbox may not be an empty string.
    */

    void           set_toolbox        (string& toolbox);

    /// \brief Set the type (element \<type\>) of the gp_ispec object.
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

    /// \brief The \<c_list\> element (list of instrument constants).

    gp_clist       clist_;

    /// \brief The \<lineage\> element.

    lineage_data   lineage_;

    /// \brief The \<time_spec\> element.

    gp_timespec    timespec_;

    /// \brief The \<toolbox\> element.

    string         toolbox_;

    /// \brief The \<t_spec\> element (definition of auxiliary values).

    gp_tspec       tspec_;

    /// \brief The \<type\> element.

    string         type_;

};

#endif // _GP_ISPEC_HPP_
