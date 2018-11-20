/** \file gp_scale.hpp
  \brief Entity class implementing the \<scale\> element in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_SCALE_HPP_
#define _GP_SCALE_HPP_

#include "astrolabe_exceptions.hpp"

#include <vector>
using namespace std;

/// \brief Entity class implementing the \<scale\> element in ASTROLABE metadata files.

class gp_scale
{
  public:

    /// \brief Add a scale value.
    /**
      \param value The new scale value to add at the end of the
             current list of scale values.
    */

    void   add_scale (double value);

    /// \brief Number of scale values.

    int    get_dim   (void);

    /// \brief  Constructor.

           gp_scale  (void);

    /// \brief Destructor.

           ~gp_scale (void);

    /// \brief Return the scale value at position pos.
    /**
      \pre ((pos >= 0) & (pos <= get_dim()))
      \param pos The index of the scale value to retrieve, based at 0.
      \return The value of the scale at position pos.
      \throw astrolabe_precondition_violation One or more preconditions have
             been violated.

      Note that index pos must comply with the following restriction:

      ((pos >= 0) & (pos < get_dim())

      Otherwise, an exception will be thrown.
    */

    double get_value (int pos);

  protected:

    /// \brief The array containing the scale values.

    vector<double> scale_;
};

#endif // _GP_SCALE_HPP_
