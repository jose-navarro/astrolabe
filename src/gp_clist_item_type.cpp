#include "gp_clist_item_type.hpp"

bool gp_clist_item_type::
dimensions_are_valid
(vector<int>& dimensions)
const
{
  {
    size_t i;
    size_t total;

    total = dimensions.size();
    if (total == 0) return false;

    for (i = 0; i < total; i++)
    {
      if (dimensions[i] <= 1) return false;
    }

    return true;
  }
}

bool
gp_clist_item_type::
get_dimension_at
(int position, int & dimension)
const
{
  {
    // Check our preconditions.

    if (is_scalar_)                           return false;
    if (position < 0)                         return false;
    if (position >= n_of_matrix_dimensions()) return false;

    // Retrieve the requested dimension.

    dimension = matrix_dimensions_[position];

    // That's all.

    return true;
  }
}

gp_clist_item_type::
gp_clist_item_type
(void)
{
  {
    is_scalar_        = true;
    matrix_dimensions_.clear();
  }
}


gp_clist_item_type::
~gp_clist_item_type
(void)
{
  {
    matrix_dimensions_.clear();
  }
}

bool
gp_clist_item_type::
is_scalar
(void)
const
{
  {
    return is_scalar_;
  }
}

int
gp_clist_item_type::
n_of_matrix_dimensions
(void)
const
{
  if (is_scalar_) return 0;
  return (int)matrix_dimensions_.size();
}

int
gp_clist_item_type::
set_type_to_matrix
(vector<int>& dimensions)
{
  {
    size_t i;
    size_t total;

    // Check our preconditions.

    if (!is_scalar_)                       return 1;
    if (!dimensions_are_valid(dimensions)) return 1;

    // OK, everything is correct. Copy the dimensions to our local member.

    total = dimensions.size();
    for (i = 0; i < total; i++) matrix_dimensions_.push_back(dimensions[i]);

    // Our type is now matrix.

    is_scalar_ = false;

    // That's all.

    return 0;

  }
}
