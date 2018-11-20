/** \file gp_navdir_data_parser.cpp
\brief Implementation file for gp_navdir_data_parser.hpp.
\ingroup ASTROLABE_metadata
*/

#include "gp_navdir_data_parser.hpp"

int
gp_navdir_data_parser::
errors_dim
(void)
const
{
  {
    return (int)list_of_errors_.size();
  }
}

string
gp_navdir_data_parser::
errors_get
(int at_position)
{
  set_precondition(errors_dim() > 0);
  set_precondition(at_position >= 0);
  set_precondition(at_position < errors_dim());

  string result;

  {
    result = list_of_errors_[at_position];
  }

  return result;
}

gp_navdir_data_parser::
gp_navdir_data_parser
(void)
{
  {
    file_name_schema_      = "";
    file_name_navdir_data_ = "";
    list_of_errors_.clear();
    list_of_warnings_.clear();
    navdir_data_           = NULL;
    ready_                 = false;
  }
}


gp_navdir_data_parser::
~gp_navdir_data_parser
(void)
{
  {
    list_of_errors_.clear();
    list_of_warnings_.clear();
  }
}

bool
gp_navdir_data_parser::
is_ready
(void)
const
{
  {
    return ready_;
  }
}

void
gp_navdir_data_parser::
parse
(void)
{
  set_precondition(is_ready());

  {
    // A general adp_DOMparser, used as the basic XML parsing tool.

    adp_DOMparser* dp;

    // A parsing utilities object. Used to parse lineage objects only.

    gp_utils       gpu;

    // Flag. Have we got an schema to validate the parsing process?

    bool           got_schema;

    //
    // Several TEMPORAL variables, used to store data read from
    // the input metadata file or to set the values of the
    // gp_metadata object.
    //

    lineage_data*  lineage;

    string         sattr;
    string         sdata;

    // Variables used in some loops.

    int            item_num;
    int            total_items;

    // Variables involved in error messaging.

    vector<string> error_description;
    vector<string> error_list;
    string         error_message;
    string         error_tag;

    // Let's ascertain if we've got a schema.

    got_schema = !file_name_schema_.empty();

    //
    // Instantiate the new adp_DOMparser. Be careful of instantiation
    // errors.
    //

    dp = new adp_DOMparser(file_name_navdir_data_.c_str());

    if (!dp->is_parser_valid())
    {
      queue_parsing_errors(dp);
      delete dp;
      dp = NULL;
      return;
    }

    // If we have an schema, inform the parser about it.

    if (got_schema)
    {
      //
      // The only possible reason to get an exception here is that
      // the schema file name is empty (precondition violation). This
      // should never happen, since we've already tested for a non
      // empty file name. Therefore, no try / cach sentence has been
      // included.
      //

      dp->set_schema(file_name_schema_.c_str());
    }

    //
    // We are ready to parse ("load") the navigation directory file.
    // Again, be careful with parsing (loading) errors.
    //

    try
    {
      dp->parse();
    }
    catch (...)
    {

      // Problems parsing the input metadata file. Report these and quit.

      queue_parsing_errors(dp);
      delete dp;
      dp = NULL;
      return;
    }

    //
    // At this point, the navigation directory file has been successfully loaded.
    // It is therefore possible to retrieve the information there stored.
    //
    //           ******** VERY IMPORTANT WARNING ********
    //
    // The code below is ASSUMING that a schema to validate the syntax of
    // the XML file has been used. This means that there exist NO
    // individual control code (try / catch clauses) to protect the
    // software against malformed input files.
    //
    // However, to enable quick software development when a metadata file
    // is modified, a GLOBAL try/catch clause surrounding the totality
    // of the parsing process is provided. This will avoid crashes, but
    // provide very litte information about the problem causing the
    // exception.
    //

    try
    {
      // Start retrieving data. Go to the main tag in the file.

      dp->goto_label("nav-directory_file");

      //
      // Get a pointer to the lineage_data object included in the
      // navdir_data  object (it was automaticall instantiated,
      // although it is empty).
      //
      // This simplifies the procedure of setting all its members.
      //

      dp->goto_label_relative("lineage");
      lineage = navdir_data_->get_lineage_ptr();

      //
      // Parse it. Note that the parse_lineage method fails only
      // when its parameters (dp, lineage) are NULL. This is not
      // so, thus, we can ignore the return value.
      //

      gpu.parse_lineage(dp, lineage);

      dp->go_back(); // Back to nav_directory_file.

      //
      // Now, all the tags under the "input" element will be parsed.
      //

      dp->goto_label_relative("input");

      //
      // OPTIONAL input options file. If it exists, we'll retrieve its
      // name, format and (optional) version. These two last items are
      // attributes of this tag.
      //

      try
      {

        dp->goto_label_relative("options/op_file");

        // Get the file name.

        sdata = dp->_label_value_string();
        navdir_data_->set_input_options_file_name(sdata);

        // Get its format attribute (mandatory).

        sattr = dp->_label_attribute_string("format");
        navdir_data_->set_input_options_file_format(sattr);

        // Try to get its version attribute (optional)

        try
        {
          sattr = dp->_label_attribute_string("version");
          navdir_data_->set_input_options_file_version(sattr);
        }
        catch (...)
        {
          // No version attribute. No problem. There's nothing to do.
        }



        dp->go_back(); // Back to input.
      }
      catch (...)
      {
        //
        // No options file present. No problem, it's optional, so
        // there's nothing to do.
        //
      }

      // MANDATORY input navigation metadata files. There may be > 1.

      dp->goto_label_relative("metadata");

      //
      // Get the number of nav_metadata elements under the metadata tag.
      // Note that there will be at least one.
      //

      total_items = dp->_n_of_repeated_sublabels("nav_metadata");

      for (item_num = 1; item_num <= total_items; item_num++)
      {

        // Navigate to the item_num_th nav_metadata element.

        dp->goto_label_relative("nav_metadata", item_num);
        sdata = dp->_label_value_string();

        navdir_data_->add_metadata_navigation_file_name(sdata);

        dp->go_back(); // Back to metadata.
      }

      dp->go_back(); // Back to input.

      // The MANDATORY input observations (obs_e) file

      dp->goto_label_relative("observations/obs_e");
      sdata = dp->_label_value_string();

      navdir_data_->set_input_observations_file_name(sdata);

      dp->go_back(); // Back to input.

      // The OPTIONAL input observations correlations file.

      try
      {
        dp->goto_label_relative("observations/tr_obs_correlation_Rll");
        sdata = dp->_label_value_string();

        navdir_data_->set_input_observations_correlations_file_name(sdata);

        dp->go_back(); // Back to input.

      }
      catch (...)
      {
        //
        // The observations correlations file name tag is optional, so if
        // it does not exist (and it doesn't, we're handling an
        // exception) we don't need to worry about it. Therefore, we
        // do nothing.
        //
      }

      // The OPTIONAL input instruments file.

      try
      {
        dp->goto_label_relative("instruments/obs_e");
        sdata = dp->_label_value_string();

        navdir_data_->set_input_instruments_file_name(sdata);

        dp->go_back(); // Back to input.
      }
      catch (...)
      {
        //
        // The input instruments file is optional, so if we don't find it, as
        // it has just happened, we need neither to worry about it nor to
        // do anything at all. So we do nothing.
        //
      }

      // There's no more tags under input. So we go back.

      dp->go_back(); // Back to nav_directory_file.

      //
      // Now, all the tags under the "output" element will be parsed.
      //

      dp->goto_label_relative("output");

      // OPTIONAL output log file.

      try
      {
        dp->goto_label_relative("log");
        sdata = dp->_label_value_string();

        navdir_data_->set_output_log_file_name(sdata);

        dp->go_back(); // Back to output.
      }
      catch (...)
      {
        //
        // Since the output log file name is optional, nothing happens when
        // we don't find its tag in the input xml file. So there's nothing
        // we can or must do.
        //
      }

      // MANDATORY output states file name.

      dp->goto_label_relative("states/obs_e");
      sdata = dp->_label_value_string();

      navdir_data_->set_output_states_file_name(sdata);

      dp->go_back(); // Back to output.

      // OPTIONAL output states correlations file name.

      try
      {
        dp->goto_label_relative("states/tr_states_correlation_Rxx");
        sdata = dp->_label_value_string();

        navdir_data_->set_output_states_correlations_file_name(sdata);

        dp->go_back(); // Back to output.

      }
      catch (...)
      {
        //
        // The states correlations file name is optional. So we
        // there's nothing we should do if we don't find it.
      }

      //
      // All the residuals section is optional. So we'll try to
      // get there using a try clause. If were are successful,
      // then the residuals file become mandatory, but the 
      // residuals correlations file is still optional...
      //

      try
      {
        dp->goto_label_relative("residuals");

        //
        // At this point, we know that the residuals section
        // exist. Therefore, the residuals file MUST exist.
        //

        dp->goto_label_relative("obs_e");
        sdata = dp->_label_value_string();

        navdir_data_->set_output_residuals_file_name(sdata);

        dp->go_back(); // Back to residuals.

        //
        // Finally, the residuals correlations file is optional,
        // so we must be carefull when trying to go to its
        // tag.
        //

        try
        {
          dp->goto_label_relative("tr_res_correlation_Rvv");
          sdata = dp->_label_value_string();

          navdir_data_->set_output_residuals_correlations_file_name(sdata);

          dp->go_back(); // Back to residuals.
        }
        catch (...)
        {
          //
          // No residuals correlations file name. Nothing to do, since
          // it's optional.
          //
        }

        // No more tags under residuals. Go back.

        dp->go_back(); // Back to output.
      }
      catch (...)
      {
        // No residuals section at all. Nothing to do.
      }

      // The tags under output have been exhausted. We go back.

      dp->go_back(); // Back to nav_directory_file.

    } // End of try clause.

    catch (...)
    {
      //
      // We've got an uncontrolled exception here.
      //
      // If the parser is being used WITHOUT a schema, this situation is not so
      // strange, since many assuptions made (as for instance, the presence of
      // some tags) may simply fail.
      //
      // On the contrary, if we're using an schema and reach this point, it
      // means that there is an error in the way the parser is programmed.
      //
      // At any rate, we'll add an error to the list of errors stating
      // that we've reached this point in the code and that the
      // parsing may not continue.
      //

      error_tag = "Unable to identify offending tag(s)";

      error_description.clear();

      if (!got_schema)
      {
        error_description.push_back("Unidentified exception catched. Since NO schema has been provided");
        error_description.push_back("the parser may have failed because an erroneous input XML file.");
        error_description.push_back("Are you a developer modifying this software?");
      }
      else
      {
        error_description.push_back("Sorry: unidentified error found. Revise your input XML file carefully!");
      }

      error_message = gpu.build_message(2, file_name_navdir_data_, error_tag, error_description);

      list_of_errors_.push_back(error_message);
    }

    // Destroy the parser.

    delete dp;
    dp = NULL;


    //
    // At this point, all possible values have been fetched, either
    // successfully or not.
    //
    // When errors have been detected, the size of list_of_errors_ will
    // be greater than zero. If so, we cannot continue and must return.
    //

    if (list_of_errors_.size() > 0) return;

    //
    // No parsing errors! Now we must check the semantic validity of the
    // options just read.
    //
    // validate_semantics() will take care of such task. If problems are
    // detected, these wil be added to either the list of errors or
    // warnings.
    //

    validate_semantics();

    return;
  }
}

void
gp_navdir_data_parser::
queue_parsing_errors
(adp_DOMparser* dp)
{
  {
    string                 error_description;
    int                    i;
    adp_parsing_errors*    pe;
    astrolabe_parse_error* pex;

    pe = dp->_parsing_errors();

    for (i = 1; i <= pe->_size(); i++)
    {
      pex = pe->get_error(i);
      error_description = (*pex)._description();

      list_of_errors_.push_back(error_description);

      delete pex;
    }
    delete pe;
  }
}

void
gp_navdir_data_parser::
set_parser_parameters
(string&         file_name_navdir,
 string&         file_name_schema,
 gp_navdir_data* navdir_data)
{
  set_precondition(!file_name_navdir.empty());
  set_precondition(!file_name_schema.empty());

  {
    file_name_navdir_data_ = file_name_navdir;
    file_name_schema_      = file_name_schema;
    navdir_data_           = navdir_data;
    ready_                 = true;
  }

  set_postcondition(is_ready());
}

void
gp_navdir_data_parser::
set_parser_parameters_developer
(string&         file_name_navdir,
 gp_navdir_data* navdir_data)
{
  set_precondition(!file_name_navdir.empty());
  set_precondition(navdir_data != NULL);

  {
    file_name_navdir_data_ = file_name_navdir;
    navdir_data_           = navdir_data;
    ready_                 = true;
  }

  set_postcondition(is_ready());
}

void
gp_navdir_data_parser::
validate_semantics
(void)
{
  {
    // NO SEMANTICS TO VALIDATE.

    // Add any semantic checks needed here.
  }
}

int
gp_navdir_data_parser::
warnings_dim
(void)
const
{
  {
    return (int)list_of_warnings_.size();
  }
}

string
gp_navdir_data_parser::
warnings_get
(int at_position)
{
  set_precondition(warnings_dim() > 0);
  set_precondition(at_position >= 0);
  set_precondition(at_position < warnings_dim());

  string result;

  {
    result = list_of_warnings_[at_position];
  }

  return result;
}
