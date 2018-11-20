/** \file gp_navdir_data.cpp
\brief Implementation file for gp_navdir_data.hpp
\ingroup ASTROLABE_metadata
*/

#include "gp_navdir_data.hpp"

void
gp_navdir_data::
add_metadata_navigation_file_name
(string& filename)
{
  {
    metadata_file_names_.push_back(filename);
  }
}

string&
gp_navdir_data::
get_input_instruments_file_name
(void)
const
{
  {
    return *(new string(instruments_file_name_));
  }
}

int
gp_navdir_data::
get_dim_input_metadata_navigation_files
(void)
{
  {
    return (int)metadata_file_names_.size();
  }
}

string&       
gp_navdir_data::
get_input_navigation_metadata_file
(int position)
{
  set_precondition((position >= 0) & (position < get_dim_input_metadata_navigation_files()));
  {
    return *(new string(metadata_file_names_[position]));
  }
}

string&
gp_navdir_data::
get_input_observations_file_name
(void)
const
{
  {
    return *(new string(observations_file_name_));
  }
}

string&
gp_navdir_data::
get_input_observations_correlations_file_name
(void)
const
{
  {
    return *(new string(observations_correlations_file_name_));
  }
}

string&
gp_navdir_data::
get_input_options_file_format
(void)
const
{
  {
    return *(new string(options_file_format_));
  }
}

string&
gp_navdir_data::
get_input_options_file_name
(void)
const
{
  {
    return *(new string(options_file_name_));
  }
}

string&
gp_navdir_data::
get_input_options_file_version
(void)
const
{
  {
    return *(new string(options_file_version_));
  }
}

lineage_data*
gp_navdir_data::
get_lineage_ptr
(void)
{
  {
    return &lineage_;
  }
}

gp_navdir_data::
gp_navdir_data
(void)
{
  {
    instruments_file_name_               = "";
    log_file_name_                       = "";
    metadata_file_names_.clear();
    observations_file_name_              = "";
    observations_correlations_file_name_ = "";
    options_file_format_                 = "";
    options_file_name_                   = "";
    options_file_version_                = "";
    residuals_file_name_                 = "";
    residuals_correlations_file_name_    = "";
    states_file_name_                    = "";
    states_correlations_file_name_       = "";
  }
}

gp_navdir_data::
~gp_navdir_data
(void)
{
  {
    instruments_file_name_ = "";
    log_file_name_ = "";
    metadata_file_names_.clear();
    observations_file_name_ = "";
    observations_correlations_file_name_ = "";
    options_file_name_ = "";
    residuals_file_name_ = "";
    residuals_correlations_file_name_ = "";
    states_file_name_ = "";
    states_correlations_file_name_ = "";
  }
}

string&
gp_navdir_data::
get_output_log_file_name
(void)
const
{
  {
    return *(new string(log_file_name_));
  }
}

string&
gp_navdir_data::
get_output_residuals_file_name
(void)
const
{
  {
    return *(new string(residuals_file_name_));
  }
}

string&
gp_navdir_data::
get_output_residuals_correlations_file_name
(void)
const
{
  {
    return *(new string(residuals_correlations_file_name_));
  }
}

string&
gp_navdir_data::
get_output_states_file_name
(void)
const
{
  {
    return *(new string(states_file_name_));
  }
}

string&
gp_navdir_data::
get_output_states_correlations_file_name
(void)
const
{
  {
    return *(new string(states_correlations_file_name_));
  }
}

void
gp_navdir_data::
set_input_instruments_file_name
(string& filename)
{
  {
    instruments_file_name_ = filename;
  }
}

void
gp_navdir_data::
set_input_observations_file_name
(string& filename)
{
  {
    observations_file_name_ = filename;
  }
}

void
gp_navdir_data::
set_input_observations_correlations_file_name
(string& filename)
{
  {
    observations_correlations_file_name_ = filename;
  }
}

void
gp_navdir_data::
set_input_options_file_format
(string& format)
{
  {
    options_file_format_ = format;
  }
}

void
gp_navdir_data::
set_input_options_file_name
(string& filename)
{
  {
    options_file_name_ = filename;
  }
}

void
gp_navdir_data::
set_input_options_file_version
(string& version)
{
  {
    options_file_version_ = version;
  }
}

void
gp_navdir_data::
set_output_log_file_name
(string& filename)
{
  {
    log_file_name_ = filename;
  }
}

void
gp_navdir_data::
set_output_residuals_file_name
(string& filename)
{
  {
    residuals_file_name_ = filename;
  }
}

void
gp_navdir_data::
set_output_residuals_correlations_file_name
(string& filename)
{
  {
    residuals_correlations_file_name_ = filename;
  }
}

void
gp_navdir_data::
set_output_states_file_name
(string& filename)
{
  {
    states_file_name_ = filename;
  }
}

void
gp_navdir_data::
set_output_states_correlations_file_name
(string& filename)
{
  {
    states_correlations_file_name_ = filename;
  }
}
