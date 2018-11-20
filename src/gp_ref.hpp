/** \file gp_ref.hpp
  \brief Entity class for \<ref\> elements in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_REF_HPP_
#define _GP_REF_HPP_

#include "gp_ref_code.hpp"
#include "astrolabe_exceptions.hpp"

#include <vector>
#include <string>

using namespace std;

/// \brief Entity class for \<ref\> elements in ASTROLABE metadata files.
/**
This class is able to hold the data needed by a \<ref\> element included
in ASTROLABE metadata files.

\<ref\> objects may be of two types: those that contain a list of
coordinate reference systems or, on the contrary, storing a list of
pairs of coordinate systems and reference frames.

To reduce complexity and avoid unnecessary class derivation, both
kind of \<ref\> objects are implemented in a single class. The
type of the \<ref\> object is an attribute that is used to tell apart
its kind.
*/

class gp_ref
{
  public:

    /// \brief Public constant: the type of the gp_ref object
    /// is not set.

    const static int GP_REF_TYPE_NOT_SET;

    /// \brief Public constant: the type of the gp_ref object
    /// is coordinate reference frame.

    const static int GP_REF_TYPE_CRF;

    /// \brief Public constant: the type of the gp_ref object
    /// is coordinate system plus reference frame.

    const static int GP_REF_TYPE_CS_RF;

    /// \brief Add a new coordinate reference frame code.
    /**
      \pre (get_type() == GP_REF_TYPE_CRF)
      \param crf The coordinate reference frame code.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Add a new coordinate reference frame to the gp_ref object.

      The type of the gp_ref object must be GP_REF_TYPE_CRF,
      otherwise, an exception will be thrown.
    */

    void        add_CRF   (gp_ref_code& crf);

    /// \brief Add a new coordinate system code.
    /**
      \pre (get_type() == GP_REF_TYPE_CS_RF)
      \param cs The coordinate system code.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Add a new coordinate system value to the gp_ref object.

      The type of the gp_ref object must be GP_REF_TYPE_CS_RF,
      otherwise, an exception will be thrown.
    */

    void        add_CS    (gp_ref_code& cs);

    /// \brief Add a new pair of coordinate system plus reference frame codes.
    /**
      \pre (get_type() == GP_REF_TYPE_CS_RF)
      \param cs The coordinate system code.
      \param rf The reference frame code.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Add a new couple of coordinate system and reference frame codes to
      the gp_ref object.

      The type of the gp_ref object must be GP_REF_TYPE_CS_RF,
      otherwise, an exception will be thrown.
    */

    void        add_CS_RF (gp_ref_code& cs, gp_ref_code& rf);

    /// \brief Add a new reference frame code.
    /**
      \pre (get_type() == GP_REF_TYPE_CS_RF)
      \param rf The reference frame code.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Add a new reference frame code to the gp_ref object.

      The type of the gp_ref object must be GP_REF_TYPE_CS_RF,
      otherwise, an exception will be thrown.
    */

    void        add_RF    (gp_ref_code& rf);

    /// \brief Retrieve the coordinate reference frame code at position pos.
    /**
      \pre (get_type() == GP_REF_TYPE_CRF)
      \pre ((pos >= 0) & (pos < get_dim()))
      \param pos Position of the coordinate reference frame code to retrieve. Must
             be a value equal to or greater than zero and less than
             get_dim().
      \return The coordinate reference frame code at position pos.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Valid only when the type of the gp_ref object is GP_REF_TYPE_CRF.
      The position pos to retrieve must be 0 <= pos < get_dim().

    */

    gp_ref_code get_CRF    (int pos);

    /// \brief Retrieve the coordinate system code at position pos.
    /**
      \pre (get_type() == GP_REF_TYPE_CS_RF)
      \pre (has_CS())
      \pre ((pos >= 0) & (pos < get_dim()))
      \param pos Position of the coordinate system code to retrieve. Must
             be a value equal to or greater than zero and less than
             get_dim().
      \return The coordinate system code at position pos.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Valid only when the type of the gp_ref object is GP_REF_TYPE_CS_RF
      and the ref object has a list of coordinate systems (has_CS()), since
      these are optional in such case.

      The position pos to retrieve must be 0 <= pos < get_dim().

    */

    gp_ref_code get_CS     (int pos);

    /// \brief Retrieve the number of coordinate reference frames or
    /// couples of coordinate system and reference frame pairs stored
    /// in the object.

    int         get_dim    (void);

    /// \brief Retrieve the actual number of coordinate system codes stored
    /// in the object.
    /**
      This method should be used for parsing validation purposes only.
      External, non-parsing modules, should use method get_dim() instead.
    */

    int         get_dim_CS (void);

    /// \brief Retrieve the actual number of reference frame codes stored
    /// in the object.
    /**
      This method should be used for parsing validation purposes only.
      External, non-parsing modules, should use tethod get_dim() instead.
    */

    int         get_dim_RF (void);

    /// \brief Retrieve the reference frame code at position pos.
    /**
      \pre (get_type() == GP_REF_TYPE_CS_RF)
      \pre ((pos >= 0) & (pos < get_dim()))
      \param pos Position of the reference frame code to retrieve. Must
             be a value equal to or greater than zero and less than
             get_dim().
      \return The reference frame code at position pos.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Valid only when the type of the gp_ref object is GP_REF_TYPE_CS_RF.
      The position pos to retrieve must be 0 <= pos < get_dim().

    */

    gp_ref_code get_RF     (int pos);

    /// \brief Retrieve the type of the object.
    /**
      \return Either GP_REF_TYPE_NOT_SET (meaning that the
             type of the gp_ref object has not been set yet),
             or GP_REF_TYPE_CRF if the object contains a
             series of coordinate reference frames or, finally,
             GP_REF_TYPE_CS_RF if,on the contrary, the
             object contains a list of pairs of coordinate systems
             and reference frames.
    */

    int         get_type   (void);

    /// \brief Default constructor.

                gp_ref     (void);

    /// \brief Destructor.

                ~gp_ref    (void);

    /// \brief Check if the ref object has a list of coordinate system
    /// codes. Used mainly in preconditions.

    bool        has_CS     (void);

    /// \brief Set the type, that is, either coordinate reference frame or
    /// coordinate system plus reference frame.
    /**
      \pre (get_type() == GP_REF_TYPE_NOT_SET)
      \param reference_type The type of gp_ref object to use. It may
             be either a coordinate reference frame object (set this parameter
             to GP_REF_TYPE_CRF) or a combination of coordinate systems
             and reference frames (set this parameter to GP_REF_TYPE_CS_RF).
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.

      Set the type of gp_ref object to use. It may contain either a list
      of coordinate reference frames or a combined list of coordinate systems
      plus reference frames codes.

      In the first case, parameter reference_type should be set to
      GP_REF_TYPE_CRF, while GP_REF_TYPE_CS_RF should be used
      in the second one.

      Note that this method may be called only once, just after the gp_ref
      object has been created. In other words, the type of the object may
      not be changed once set.
    */

    void        set_type   (int reference_type);

  protected:

    /// \brief List of coordinate reference frames. Valid only when the
    /// gp_ref's type is GP_REF_TYPE_CRF.

    vector<gp_ref_code> coordinate_reference_frame_;

    /// \brief List of coordinate systems. Valid only when the gp_ref's
    /// type is GP_REF_TYPE_CS_RF.

    vector<gp_ref_code> coordinate_system_;

    /// \brief Flag to indicate whether the gp_ref has a list of coordinate
    /// system codes (these are optional).

    bool                has_CS_;

    /// \brief List of reference frames. Valid only when the gp_ref's
    /// type is GP_REF_TYPE_CS_RF.

    vector<gp_ref_code> reference_frame_;

    /// \brief Type of gp_ref in use (either a coordinate reference frame or
    /// a combination of coordinate system plus reference frame.

    int            reference_type_;

};

#endif // _GP_REF_HPP_

