/** \file gp_covariance.hpp
  \brief Entity class to store a the \<c\> element (covariance matrices)
         in ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_COVARIANCE_HPP_
#define _GP_COVARIANCE_HPP_

#include "astrolabe_exceptions.hpp"

#include <vector>
using namespace std;

/// \brief Entity class to store a the \<c\> element (covariance matrices)
/// in ASTROLABE metadata files.
/**
  Entity class provided to store standard deviation + correlation values
  parsed from metadata files.

  Covariance matrices are represented using standard deviations and
  correlations.

  Internally (and this is important to understand how the set_* and get_*
  methods work) the values are stored as follows. Assuming that the covariance
  matrix is the following:

  \f[
  \begin{array}{lllll}
  \sigma_1    &             &            &        &             \\
  \rho_{2,1}  & \sigma_2    &            &        &             \\
  \rho_{3,1}  & \rho_{3,2}  & \sigma_3   &        &             \\
  \cdots      & \cdots      & \cdots     &        &             \\
  \rho_{n,1}  & \rho_{n,2}  & \rho_{n,3} & \cdots & \sigma_{n}
  \end{array}
  \f]

  Then the list of values in the previous matrix are stored as follows:

  Standard deviations only:

  \f[
  \sigma_1,\sigma_2,\sigma_3,\cdots,\sigma_{n}
  \f]

  Standard deviations plus correlations
  
  \f[
  \sigma_1,\sigma_2,\sigma_3,\cdots,\sigma_{n},\rho_{2,1},\rho_{3,1}, \rho_{3,2},\rho_{n,1},\rho_{n,2},\rho_{n,3},\cdots,\rho_{n,n-1}
  \f]

  Note that correlations may be or not stored by this class. It is the responsibility
  of the calling module to determine this situation, retrieving the
  number of values stored and checking it against the expected number of
  values for both situations.

  Additionally, and since this class is unable to tell apart standard deviation
  from correlation values, set_* and get_* methods do not distinguish among
  these.
 */

class gp_covariance
{
  public:

   /// \brief Retrieve the total number of values stored, either
    /// standard deviations or correlations.
    /**
      \return The number of values stored.
    */

    int    get_dim        (void);

    /// \brief Retrieve a value in a given position of the covariance matrix,
    /// no matter whether this is a standard deviation or a correlation.
    /**
      \pre ((pos >= 0) & (pos < get_dim()))
      \return The value stored at position pos.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Note the 0-based index policy to get values.

      See the documentation at the beginning of this class to understand how
      standard deviations and correlations values are stored and retrieved
      from the positional point of view.

      If the position pos is not within the allowed range, an
      exception is thrown.
    */

    double get_value      (int pos);

    /// \brief Default constructor.

           gp_covariance  (void);

    /// \brief Destructor.

           ~gp_covariance (void);

    /// \brief Set the total dimension (including standard deviations and
    /// optionally correlation values) of the covariance matrix.
    /**
      \pre (get_dim() == 0)
      \pre (dim > 0)
      \param dim The dimension (summing up all possible values) of the covarianc
             matrix.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.

      Set the dimension of the covariance matrix. Note that this dimension
      (see parameter dim) must include all standard deviation plus optionally
      correlation values to store.

      This method may be used only once, when the dimension of the matrix has
      not been yet set (that is, its dimension is exactly 0). If this condition
      is not honored, an exception will be thrown.

      Additionally, the dimension to set (see parameter dim) must be strictly
      positive. An exception will also be thrown if such condition is not
      respected.

      Once that the dimension of the covariance matrix is correctly set, the
      standard deviation and correlation values are set to 0 by this method.
    */

    void   set_dim        (int dim);

    /// \brief Set the value of the covariance matrix at a given position,
    /// no matter whether this value is either a standard deviation or
    /// a correlation.
    /**
      \pre ((pos >= 0) & (pos < get_dim()))
      \param pos The index of the value to set.
      \param value The value to set.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Note the 0-based index policy to set values.

      See the documentation at the beginning of this class to understand
      how standard deviations and correlations values are stored and retrieved
      from the positional point of view.

      If the position pos is not within the allowed range, an
      exception is thrown.
    */

    void   set_value      (int pos, double value);

  protected:

    /// \brief Number of stored values.

    int            dim_;

    /// \brief Array with the standard deviations and, optionally, correlations.

    vector<double> values_;
};

#endif // _GP_COVARIANCE_HPP_
