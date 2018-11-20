/** \file gp_clist_item_type.hpp
\brief Entity class to hold the \<type\> element included in the
\<c_list\>\<item\> element found in ASTROLABE metadata files.
\ingroup ASTROLABE_metadata
*/

#ifndef _GP_CLIST_ITEM_TYPE_HPP_
#define _GP_CLIST_ITEM_TYPE_HPP_

#include <vector>
using namespace std;

/// \brief Entity class to hold the \<type\> element included in the \<c_list\>\<item\>
///        element found in ASTROLABE metadata files.

class gp_clist_item_type
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

    bool  dimensions_are_valid   (vector<int>& dimensions) const;

    /// \brief Get the nth dimension of the matrix.
    /**
      \pre (!is_scalar())
      \pre ((position >= 0) & (position < n_of_matrix_dimensions()))
      \param position The position whose dimension is to be retrieved.
             Its value must be in the range 0 .. n_of_matrix_dimensions ().
      \param dimension The value of the requested dimension.
      \return True if the type is MATRIX and the position requested falls
             within the legal range, false otherwise.
    */

    bool  get_dimension_at       (int position, int& dimension) const;

    /// \brief Default constructor.
    /**
      The default constructor builds a gp_clist_item_type element whose
      type is "SCALAR".

      Since SCALAR types have no extra accompanying information, just
      instantiating the object using the constructor should suffice and
      no additional actions would be required.

      On the other side, if the type must be MATRIX, then method
      set_type_matrix() must be called after the constructor has
      been invoked.
    */

         gp_clist_item_type      (void);

    /// \brief Destructor.

          ~gp_clist_item_type    (void);

    /// \brief Check if the type is "SCALAR".
    /**
      \return True if the type is "SCALAR", false if it is "MATRIX".
    */
    bool  is_scalar              (void) const;

    /// \brief Return the number of dimensions of the matrix, when
    /// the type is "MATRIX".
    /**
      \return The number of dimensions of the matrix.

      When the type is "MATRIX", the return value equals the number of
      dimensions it has. On the contrary, if the type is "SCALAR", the
      returned result will always be zero.
    */

    int   n_of_matrix_dimensions (void) const;

    /// \brief Set the type to "MATRIX" and provide with the values
    /// of its dimensions.
    /**
      \pre (is_scalar())
      \pre (dimensions_are_valid(dimensions))
      \param dimensions The values of the dimensions of the matrix.
      \return An error code. 0 stands for successful completion. 1 means
            that one or more preconditions have been violated.
    */

    int   set_type_to_matrix     (vector<int>& dimensions);

  protected:

    /// \brief States whether the type is "SCALAR" (true) or "MATRIX" (false).

    bool        is_scalar_;

    /// \brief Dimensions of the matrix, when the type is "MATRIX"

    vector<int> matrix_dimensions_;


};

#endif // _GP_CLIST_ITEM_TYPE_HPP_
