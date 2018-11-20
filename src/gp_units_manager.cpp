/** \file gp_units_manager.cpp
\brief Implementation file for gp_units_manager.hpp
\ingroup ASTROLABE_metadata
*/

#include "gp_units_manager.hpp"



gp_units_manager::
gp_units_manager
(void)
{
  {
    path_to_units_system_ = "";
    units_system_         = NULL;
    valid_                = false;

    //
    // Avoid any kind of warning or error notifications. These
    // will be treated by us analyzing error codes.
    //

    ut_set_error_message_handler(ut_ignore);
  }
}


gp_units_manager::
~gp_units_manager
(void)
{
  {
    if (units_system_ != NULL) ut_free_system(units_system_);
  }
}

bool
gp_units_manager::
is_valid
(void)
const
{
  {
    return valid_;
  }
}

int
gp_units_manager::
set_units_database_path
(string & database_path)
{
  {
    // We cannot accept an empty path and filename.

    if (database_path.empty()) return 1;

    // It's not empty. Copy the input parameter to our local member.

    path_to_units_system_ = database_path;

    //
    // We'll try now to load the units database. This may
    // fail if the path given is not correct.
    //

    units_system_ = ut_read_xml(path_to_units_system_.c_str());

    if (units_system_ == NULL) return 2;

    // That's all. We are valid now!

    valid_ = true;
    return 0;
  }
}

int
gp_units_manager::
validate_unit
(string & unit_to_validate)
{
  int result;
  {
    ut_unit* the_unit;

    // Check preconditions.

    if (!valid_) return 1;
    if (unit_to_validate.empty()) return 1;

    //
    // Try to parse the unit. We'll try to do it using the
    // three different text encodings supported by UDUNITS2.
    //
    // As soon as we get a positive answer, we'll cancel the
    // remaining attempts. On the contrary, we'll say that a
    // unit is invalid when the three attempts to parse the unit
    // fail.
    //

    // First, try the UTF-8 encoding.

    the_unit = ut_parse(units_system_, unit_to_validate.c_str(), UT_UTF8);

    if (the_unit == NULL)
    {
      // No luck? Try LATIN1 (synonim of ISO_8859_1)
      the_unit = ut_parse(units_system_, unit_to_validate.c_str(), UT_LATIN1);

      if (the_unit == NULL)
      {
        // No luck yet? Last attempt: classic ASCII.
        ut_parse(units_system_, unit_to_validate.c_str(), UT_ASCII);
      }
    }

    // Release memory and set return code.

    result = 2;

    if (the_unit != NULL)
    {
      ut_free(the_unit);
      result = 0;
    }

    return result;
  }
}
