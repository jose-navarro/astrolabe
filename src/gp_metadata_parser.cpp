/** \file gp_metadata_parser.cpp
  \brief Implementation file for gp_metadata_parser.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_metadata_parser.hpp"

int
gp_metadata_parser::
errors_dim
(void)
const
{
  {
    return (int)list_of_errors_.size();
  }
}

string
gp_metadata_parser::
errors_get
(int at_position)
{
  set_precondition (errors_dim() > 0);
  set_precondition (at_position >= 0);
  set_precondition (at_position < errors_dim());

  string result;

  {
    result = list_of_errors_[at_position];
  }

  return result;
}

gp_metadata_parser::
gp_metadata_parser
(void)
{
  {
    file_name_schema_         = "";
    file_name_units_database_ = "";
    metadata_                 = NULL;
    ready_                    = false;
  }
}


gp_metadata_parser::
~gp_metadata_parser
(void)
{
  {
    list_of_errors_.clear();
    list_of_warnings_.clear();
  }
}

bool
gp_metadata_parser::
is_ready
(void)
const
{
  {
    return ready_;
  }
}

void
gp_metadata_parser::
parse
(void)
{
  set_precondition (is_ready());

  {
    // A general adp_DOMparser, used as the basic XML parsing tool.

    adp_DOMparser* dp;

    // A parsing utilities object.

    gp_utils       gpu;

    // Flag. Have we got an schema to validate the parsing process?

    bool           got_schema;

    // Name of the current metadata file being parsed.

    string         metadata_file;

    //
    // Several TEMPORAL variables, used to store data read from
    // the input metadata file or to set the values of the
    // gp_metadata object.
    //

    gp_ispec*      ispec;
    gp_lspec*      lspec;
    gp_mspec*      mspec;
    gp_pspec*      pspec;

    string         sattr;
    string         sdata;
    string         sdata2;
    vector<string> vsdata;
    vector<string> vsdata2;

    // Variables used in some loops.

    int            file_index;
    int            item_num;
    int            total_items;

    // Variables involved in error messaging.

    vector<string> error_description;
    vector<string> error_list;
    string         error_message;
    string         error_tag;
    int            retcode;
    string         tmp_message;

    //
    // Try to initialize the UDUNITS2 system by means of
    // our units_manager_ object.
    //

    retcode = units_manager_.set_units_database_path(file_name_units_database_);

    if (retcode != 0)
    {
      //
      // Unable to load the units database. This prevents us from parsing anything at all,
      // so we'll report the problem and return.
      //

      error_tag = "No offending tag";

      error_description.push_back("Error detected while trying to load the units database.");
      error_description.push_back("The file used to load it was: ");

      tmp_message = "  '" + file_name_units_database_ + "'";
      error_description.push_back(tmp_message);

      error_description.push_back("Please check that it exist and that it is a correct");
      error_description.push_back("UDUNITS2 units database file.");
      error_description.push_back("This should have never happened.");
      error_description.push_back("Reinstalling the application should solve this problem.");

      error_message = gpu.build_message(2, metadata_file, error_tag, error_description);

      list_of_errors_.push_back(error_message);

    }

    // Let's ascertain if we've got a schema.

    got_schema = !file_name_schema_.empty();

    //
    // Parse all the metadata files whose names are stored in the
    // file_name_metadata_ string array.
    //
    
    for (file_index = 0; file_index < (int)file_name_metadata_.size(); file_index++)
    {

      // Get the name of the current metadata file.

      metadata_file = file_name_metadata_[file_index];

      //
      // Instantiate the new adp_DOMparser. Be careful of instantiation
      // errors.
      //

      dp = new adp_DOMparser(metadata_file.c_str());

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
      // We are ready to parse ("load") the metadata file. Again, be careful
      // with parsing (loading) errors.
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
      // At this point, the metadata file has been successfully loaded. It is
      // therefore possible to retrieve the information there stored.
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
        //
        // ----- PARSE ALL THE I_SPEC ELEMENTS INCLUDED IN THE FILE
        //

        dp->goto_label("nav_metadata_file");

        //
        // Get the number of i_spec elements under the root tag. Note
        // that there may be no i_spec tags in the input file.
        //

        total_items = dp->_n_of_repeated_sublabels("i_spec");

        // Parse all the i_spec elements.

        for (item_num = 1; item_num <= total_items; item_num++)
        {

          // Navigate to the item_num_th i_spec element.

          dp->goto_label("nav_metadata_file/i_spec", item_num);

          // Instantiate a new i_spec object.

          ispec = new gp_ispec();

          // Parse the i_spec element and set the i_spec object accordingly.

          retcode = gpu.parse_ispec(metadata_file, dp, &units_manager_, ispec, error_list);

          if (retcode != 0)
          {
            // Queue the errors detected by the parsing process.

            list_of_errors_.insert(list_of_errors_.end(),
                                   error_list.begin(),
                                   error_list.end());

          } else if (ispec->get_active())
          {
            //
            // Add the newly parsed ispec to the list of ispecs, but only
            // if it is active.
            //

            metadata_->add_ispec(*ispec);
          }

          // Throw garbage away.

          delete ispec;
          ispec = NULL;

        }

        //
        // ----- PARSE ALL THE L_SPEC ELEMENTS INCLUDED IN THE FILE
        //

        dp->goto_label("nav_metadata_file");

        //
        // Get the number of l_spec elements under the root tag. Note
        // that there may be no l_spec tags in the input file.
        //

        total_items = dp->_n_of_repeated_sublabels("l_spec");

        // Parse all the l_spec elements.

        for (item_num = 1; item_num <= total_items; item_num++)
        {
          dp->goto_label("nav_metadata_file/l_spec", item_num);

          // Instantiate a new l_spec object.

          lspec = new gp_lspec();

          // Parse the l_spec element and set the l_spec object accordingly.

          retcode = gpu.parse_lspec(metadata_file, dp, &units_manager_, lspec, error_list);

          if (retcode != 0)
          {
            // Queue the errors detected by the parsing process.
           
            list_of_errors_.insert(list_of_errors_.end(),
                                   error_list.begin(),
                                   error_list.end());

          } else if (lspec->get_active())
          {
            //
            // Add the newly parsed lspec to the list of lspecs, but only
            // if it is active.
            //

            metadata_->add_lspec(*lspec);
          }

          // Throw garbage away.

          delete lspec;
          lspec = NULL;
        }

        //
        // ----- PARSE ALL THE P_SPEC ELEMENTS INCLUDED IN THE FILE
        //

        dp->goto_label("nav_metadata_file");

        //
        // Get the number of p_spec elements under the root tag. Note
        // that there may be no p_spec tags in the input file.
        //

        total_items = dp->_n_of_repeated_sublabels("p_spec");

        // Parse all the p_spec elements.

        for (item_num = 1; item_num <= total_items; item_num++)
        {
          dp->goto_label("nav_metadata_file/p_spec", item_num);

          // Instantiate a new p_spec object.

          pspec = new gp_pspec();

          // Parse the p_spec element and set the p_spec object accordingly.

          retcode = gpu.parse_pspec(metadata_file, dp, &units_manager_, pspec, error_list);

          //
          // Manage the errors that might have been detected. Since no preconditions
          // may be violated (ours are more restrictive than parse_pspec's) we
          // only have to care about actual parsing errors.
          //

          if (retcode != 0)
          {
            // Queue the errors detected by the parsing process.

            list_of_errors_.insert(list_of_errors_.end(),
                                   error_list.begin(),
                                   error_list.end());

          } else if (pspec->get_active())
          {
            //
            // Add the newly parsed pspec to the list of pspecs, but only
            // if it is active.
            //
          
            metadata_->add_pspec(*pspec);
          }

          // Throw garbage away.

          delete pspec;
          pspec = NULL;
        }

        //
        // ----- PARSE ALL THE M_SPEC ELEMENTS INCLUDED IN THE FILE
        //

        dp->goto_label("nav_metadata_file");

        //
        // Get the number of m_spec elements under the root tag. Note
        // that there may be no m_spec tags in the input file.
        //

        total_items = dp->_n_of_repeated_sublabels("m_spec");

        // Parse all the m_spec elements.

        for (item_num = 1; item_num <= total_items; item_num++)
        {
          dp->goto_label("nav_metadata_file/m_spec", item_num);

          // Instantiate a new m_spec object.

          mspec = new gp_mspec();

          // Parse the m_spec element and set the m_spec object accordingly.

          retcode = gpu.parse_mspec(metadata_file, dp, mspec, error_list);

          if (retcode != 0)
          {
            // Queue the errors detected by the parsing process.

            list_of_errors_.insert(list_of_errors_.end(),
                                   error_list.begin(),
                                   error_list.end());

          } else if (mspec->get_active())
          {
            //
            // Add the newly parsed mspec to the list of mspecs, but only
            // if it is active.
            //

            metadata_->add_mspec(*mspec);
          }

          // Throw garbage away.

          delete mspec;
          mspec = NULL;
        }
      } // End of try clause.

      catch (...)
      {
        //
        // We've got an uncontrolled exception here.
        //
        // If the parser is being used WITHOUT an schema, this situation is not so
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

        error_message = gpu.build_message(2, metadata_file, error_tag, error_description);

        list_of_errors_.push_back(error_message);
      }

      // Destroy the parser.

      delete dp;
      dp = NULL;

    }

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
gp_metadata_parser::
queue_parsing_errors
(adp_DOMparser* dp)
{
  {
    string              error_description;
    int                 i;
    adp_parsing_errors* pe;
    astrolabe_parse_error*    pex;

    pe = dp->_parsing_errors();

    for (i=1; i<=pe->_size(); i++)
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
gp_metadata_parser::
set_parser_parameters
(vector<string>& file_name_metadata,
 string& file_name_units_database,
 string& file_name_schema,
 gp_metadata* metadata)
{
  set_precondition (file_name_metadata.size() > 0);
  set_precondition (!file_name_units_database.empty());
  set_precondition (!file_name_schema.empty());

  {
    file_name_metadata_       = file_name_metadata;
    file_name_units_database_ = file_name_units_database;
    file_name_schema_         = file_name_schema;
    metadata_                 = metadata;
    ready_                    = true;
  }

  set_postcondition (is_ready());
}

void
gp_metadata_parser::
set_parser_parameters_developer
(vector<string>& file_name_metadata,
 string&         file_name_units_database,
 gp_metadata*    metadata)
{
  set_precondition (file_name_metadata.size() > 0);
  set_precondition(!file_name_units_database.empty());
  set_precondition (metadata != NULL);

  {
    file_name_metadata_       = file_name_metadata;
    file_name_units_database_ = file_name_units_database;
    metadata_                 = metadata;
    ready_                    = true;
  }

  set_postcondition (is_ready());
}

void
gp_metadata_parser::
validate_semantics
(void)
{
  {
    // Error-related variables.

    vector<string>                 error_description;
    string                         error_message;
    string                         error_tag;

    // GEMMA parsing utilities object.

    gp_utils                       gpu;

    // Temporal counters, accumulators, value holders... used in checks.

    int                            i;
    string                         id;
    set<string>                    ids_ispec;
    set<string>                    ids_lspec;
    set<string>                    ids_mspec;
    set<string>                    ids_pspec;
    gp_ilist*                      ilist;
    gp_ispec                       ispec;
    int                            j;
    lineage_data*                  lineage;
    gp_llist*                      llist;
    gp_lspec                       lspec;
    gp_mspec                       mspec;
    gp_plist_item                  pitem;
    gp_plist*                      plist;
    gp_pspec                       pspec;
    string                         sdata;
    set<string>::iterator          sfinder;
    int                            total;

    //
    //  -----------------------------------------------------------------
    //  ------ FIRST, CHECK THOSE CONDITIONS THAT ARE FATAL ERRORS ------
    //  -----------------------------------------------------------------
    //

    //
    // --- Check that at least one metadata object of types l_, m_ and p_spec
    //     has been loaded. Instruments are optional, so, at least now, we
    //     won't ask for these.
    //

    if ((metadata_->get_dim_lspec() == 0) ||
        (metadata_->get_dim_pspec() == 0) ||
        (metadata_->get_dim_mspec() == 0))
    {

      error_description.clear();

      error_tag = "<nav_metadata_file>";
      error_description.push_back("Incomplete metadata specification. Please, specify at least one");
      error_description.push_back("<l_spec>, one <m_spec> and one <p_spec> element.");

      error_message = gpu.build_message(2, file_name_metadata_, error_tag, error_description);

      list_of_errors_.push_back(error_message);

    }

    //
    // --- Check that no repeated identifiers are used for i_spec, l_spec
    //     m_spec or p_spec objects. Note that repetitions are not allowed
    //     among objects of the SAME type.
    //

    // First, p_specs

    total = metadata_->get_dim_pspec();

    if (total > 0)
    {
      ids_pspec.clear();

      pspec = metadata_->get_pspec(0);
      lineage = pspec.get_lineage_ptr();
      sdata = lineage->id_get();

      ids_pspec.insert(sdata);

      for (i = 1; i < total; i++)
      {
        pspec = metadata_->get_pspec(i);
        lineage = pspec.get_lineage_ptr();
        sdata = lineage->id_get();

        sfinder = ids_pspec.find(sdata);

        if (sfinder != ids_pspec.end())
        {
          // The identifier already exist!!!

          error_description.clear();

          error_tag = "<nav_metadata_file><p_spec><lineage><id>";
          error_description.push_back("Repeated identifier found for two or more <p_spec> elements.: " + sdata);

          error_message = gpu.build_message(2, file_name_metadata_, error_tag, error_description);
          list_of_errors_.push_back(error_message);
        }
        else
        {
          ids_pspec.insert(sdata);
        }
      }
    }

    // Now, l_specs

    total = metadata_->get_dim_lspec();

    if (total > 0)
    {
      ids_lspec.clear();

      lspec = metadata_->get_lspec(0);
      lineage = lspec.get_lineage_ptr();
      sdata = lineage->id_get();

      ids_lspec.insert(sdata);

      for (i = 1; i < total; i++)
      {
        lspec = metadata_->get_lspec(i);
        lineage = lspec.get_lineage_ptr();
        sdata = lineage->id_get();

        sfinder = ids_lspec.find(sdata);

        if (sfinder != ids_lspec.end())
        {
          // The identifier already exist!!!

          error_description.clear();

          error_tag = "<nav_metadata_file><l_spec><lineage><id>";
          error_description.push_back("Repeated identifier found for two or more <l_spec> elements.: " + sdata);

          error_message = gpu.build_message(2, file_name_metadata_, error_tag, error_description);
          list_of_errors_.push_back(error_message);
        }
        else
        {
          ids_lspec.insert(sdata);
        }
      }
    }

    // Third, m_specs

    total = metadata_->get_dim_mspec();

    if (total > 0)
    {
      ids_mspec.clear();

      mspec = metadata_->get_mspec(0);
      lineage = mspec.get_lineage_ptr();
      sdata = lineage->id_get();

      ids_mspec.insert(sdata);

      for (i = 1; i < total; i++)
      {
        mspec = metadata_->get_mspec(i);
        lineage = mspec.get_lineage_ptr();
        sdata = lineage->id_get();

        sfinder = ids_mspec.find(sdata);

        if (sfinder != ids_mspec.end())
        {
          // The identifier already exist!!!

          error_description.clear();

          error_tag = "<nav_metadata_file><m_spec><lineage><id>";
          error_description.push_back("Repeated identifier found for two or more <m_spec> elements.: " + sdata);

          error_message = gpu.build_message(2, file_name_metadata_, error_tag, error_description);
          list_of_errors_.push_back(error_message);
        }
        else
        {
          ids_mspec.insert(sdata);
        }
      }
    }

    // And, finally, i_specs

    total = metadata_->get_dim_ispec();

    if (total > 0)
    {
      ids_ispec.clear();

      ispec = metadata_->get_ispec(0);
      lineage = ispec.get_lineage_ptr();
      sdata = lineage->id_get();

      ids_ispec.insert(sdata);

      for (i = 1; i < total; i++)
      {
        ispec = metadata_->get_ispec(i);
        lineage = ispec.get_lineage_ptr();
        sdata = lineage->id_get();

        sfinder = ids_ispec.find(sdata);

        if (sfinder != ids_ispec.end())
        {
          // The identifier already exist!!!

          error_description.clear();

          error_tag = "<nav_metadata_file><i_spec><lineage><id>";
          error_description.push_back("Repeated identifier found for two or more <i_spec> elements.: " + sdata);

          error_message = gpu.build_message(2, file_name_metadata_, error_tag, error_description);
          list_of_errors_.push_back(error_message);
        }
        else
        {
          ids_ispec.insert(sdata);
        }
      }
    }

    //
    // --- Check that the <l_spec>, <p_spec> and <i_spec> referenced by
    //     the different models (<m_spec> elements>) actually exist
    //     (that is, have been defined in our metadata).
    //

    total = metadata_->get_dim_mspec();


    for (i = 0; i < total; i++)
    {
      mspec = metadata_->get_mspec(i);
      lineage = mspec.get_lineage_ptr();
      sdata = lineage->id_get();

      // Proceed with references to <l_spec> elements.

      llist = mspec.get_llist_ptr();

      // Check all the items referenced by the model.

      for (j = 1; j <= llist->get_dim(); j++)
      {
        id = llist->get_item_id(j);

        sfinder = ids_lspec.find(id);

        if (sfinder == ids_lspec.end())
        {
          // The l_spec referenced element does not exist!!!

          error_description.clear();

          error_tag = "<nav_metadata_file><m_spec><l_list><item>";
          error_description.push_back("A <m_spec> references an inexistent <l_spec><id>");
          error_description.push_back("Lineage identifier of the offending <m_spec>: " + sdata);
          error_description.push_back("Lineage identifier of the missing <l_spec>  : " + id);

          error_message = gpu.build_message(2, file_name_metadata_, error_tag, error_description);
          list_of_errors_.push_back(error_message);
        }
      }

      // Continue with references to <i_spec> elements.

      ilist = mspec.get_ilist_ptr();

      // Check all the item elements in the i_list referenced by the model.

      for (j = 1; j <= ilist->get_dim(); j++)
      {
        id = ilist->get_item_id(j);

        sfinder = ids_ispec.find(id);

        if (sfinder == ids_ispec.end())
        {
          // The i_spec referenced element does not exist!!!

          error_description.clear();

          error_tag = "<nav_metadata_file><m_spec><i_list><item><id>";
          error_description.push_back("A <m_spec> references an inexistent <i_spec>");
          error_description.push_back("Lineage identifier of the offending <m_spec>: " + sdata);
          error_description.push_back("Lineage identifier of the missing <i_spec>  : " + id);

          error_message = gpu.build_message(2, file_name_metadata_, error_tag, error_description);
          list_of_errors_.push_back(error_message);
        }
      }

      // And finish with references to <p_spec> elements.

      plist = mspec.get_plist_ptr();

      // Check all the items referenced by the model.

      for (j = 1; j <= plist->get_dim(); j++)
      {
 
        pitem = plist->get_item(j);
        id = pitem.get_id();

        sfinder = ids_pspec.find(id);

        if (sfinder == ids_pspec.end())
        {
          // The p_spec referenced element does not exist!!!

          error_description.clear();

          error_tag = "<nav_metadata_file><m_spec><p_list><item><id>";
          error_description.push_back("A <m_spec> references an inexistent <p_spec>");
          error_description.push_back("Lineage identifier of the offending <m_spec>: " + sdata);
          error_description.push_back("Lineage identifier of the missing <p_spec>  : " + id);

          error_message = gpu.build_message(2, file_name_metadata_, error_tag, error_description);
          list_of_errors_.push_back(error_message);
        }
      }
    }

    //
    //  -------------------------------------------------------------------
    //  ------ SECOND, CHECK THOSE CONDITIONS THAT ARE JUST WARNINGS ------
    //  -------------------------------------------------------------------
    //

    // There are no warning test to perform.

  }
}

int
gp_metadata_parser::
warnings_dim
(void)
const
{
  {
    return (int)list_of_warnings_.size();
  }
}

string
gp_metadata_parser::
warnings_get
(int at_position)
{
  set_precondition (warnings_dim() > 0);
  set_precondition (at_position >= 0);
  set_precondition (at_position < warnings_dim());

  string result;

  {
    result = list_of_warnings_[at_position];
  }

  return result;
}

