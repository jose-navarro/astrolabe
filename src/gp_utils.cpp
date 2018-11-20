/** \file gp_utils.cpp
  \brief Implementation file for gp_utils.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_utils.hpp"

//
// IMPORTANT WARNING: DO NOT INCLUDE the following #include statement
// in this file's header (gp_utils.hpp). It breaks the compilation of
// other files in the library.
//
// Including the following header here solves the problem. There may
// be some kind of name clashing...
//

#include "gp_units_manager.hpp"

string
gp_utils::
build_message
(int             error_level,
 const string&   offending_file,
 string&         tag,
 vector<string>& description)
{
  string result;

  {
    int    i;
    string indent;
    int    total_lines;

    // File name of the XML file involved in the problem.

    result = "*** File: " + offending_file + "\n";

    // Offending tag.

    result  = result + "    Offending tag(s): " + tag + "\n";

    // Warning or error?

    if (error_level == 1)
    {
      result = result + "    [WARNING] ";
      indent =          "              ";
    }
    else
    {
      result = result + "    [ERROR] ";
      indent =          "            ";
    }

    //
    // Description. Iterate for each line in the description vector.
    // However, the first line needs to be processed in a different way
    // to include the "warning" or "error" tag.
    //

    total_lines = (int)description.size();

    if (total_lines == 0) return result;

    result = result + description[0] + "\n";

    for (i = 1; i < total_lines; i++) result += (indent + description[i] + "\n");
  }

  return result;
}

string
gp_utils::  
build_message
(int                   error_level,
 const vector<string>& offending_files,
 string&               tag,
 vector<string>&       description)
{
  {
    string full_list_of_files;
    int    i;
    int    total;

    //
    // Reformat the list of files in a single one so we may use the
    // simpler version of this method to build the error message.
    //

    full_list_of_files = "";
    total              = (int)offending_files.size();

    for (i = 0; i < total; i++) 
    {
      full_list_of_files = full_list_of_files + offending_files[i];
      if (i != (total - 1)) full_list_of_files = full_list_of_files + "\n          ";
    }

    // Build the message using the one-file-only version of this method.

    return build_message (error_level, full_list_of_files, tag, description);
  }
}


gp_utils::
gp_utils
(void)
{
  {
    // Intentionally left blank.
  }
}

gp_utils::
~gp_utils
(void)
{
  {
    // Intentionally left blank.
  }
}

bool
gp_utils::
is_whitespace
(const char c)
{
  {
    return ((c == ' ')  ||
            (c == '\t') ||
            (c == '\r') ||
            (c == '\n') ||
            (c == '\v') ||
            (c == '\f'));
  }
}

int
gp_utils::  
parse_clist
(const string&     filename,
 adp_DOMparser*    dp,
 gp_units_manager* units_manager,
 string&           lineage_id,
 gp_clist*         clist,
 vector<string>&   error_messages)
{
  int result;

  {
    bool           is_scalar;
    gp_clist_item* item;
    int            item_index;
    string         item_type;
    gp_covariance* covariance;
    int            dimension;
    string         dimension_txt;
    vector<string> error_description;
    string         error_tag;
    vector<int>    matrix_dimensions;
    int            n_items;
    string         n_items_txt;
    int            n;
    string         n_txt;
    gp_ref*        ref;
    int            retcode;
    gp_scale*      scale;
    bool           status;
    string         tmp_msg;
    string         tmp_txt;
    stringstream   tostring_converter;
    gp_unit*       units;

    // Check our preconditions.

    if (filename.empty())           return 1;
    if (dp == NULL)                 return 1;
    if (units_manager == NULL)      return 1;
    if (!units_manager->is_valid()) return 1;
    if (lineage_id.empty())         return 1;
    if (clist == NULL)              return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    // Get the actual number of item elements in the c_list element.

    n_items = dp->_n_of_repeated_sublabels("item");
    tostring_converter.str("");
    tostring_converter << n_items;
    n_items_txt = tostring_converter.str();

    // Get the declared dimension of the c_list.

    dp->goto_label_relative("dimension");
    dimension = dp->_label_value_int();
    dimension_txt = dp->_label_value_string();
    clist->set_dim(dimension);
    dp->go_back(); // Back to c_list.

    //
    // Check that the actual and declared dimension match.
    // Report an error if they don't match, but keep working to
    // detect as many errors as possible.
    //

    if (dimension != n_items)
    {
      error_description.clear();

      error_tag = "<nav_metadata_file><i_spec><c_list><dimension>";

      error_description.push_back("The value of the <dimension> element ("
                                   + dimension_txt
                                   + ") does not match with actual number of <item> elements (" 
                                   + n_items_txt 
                                   + ").");
      error_description.push_back("Offending <i_spec> lineage identifier: " + lineage_id);
      tmp_msg = build_message(2, filename, error_tag, error_description);

      error_messages.push_back(tmp_msg);

      result = 2;
    }

    // Parse the n_items item elements that have been found.

    for (item_index = 1; item_index <= n_items; item_index++)
    {

      // Instantiate a new item (gp_clist_item) element.

      item = new gp_clist_item();

      // Go to the item at position item_index.

      dp->goto_label_relative("item",item_index);

      // Get its "n" attribute in both numeric and textual forms.

      n     = dp->_label_attribute_int("n");
      n_txt = dp->_label_attribute_string("n");

      //
      // Check that the value of the "n" attribute is within the
      // valid range [1..dimension].
      //

      if ((n < 1) || (n > dimension))
      {
        //
        // The value of the "n" attribute is not in the valid range.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><i_spec><c_list><item>";

        error_description.push_back("The value of the n attribute ("
                                     + n_txt
                                     + ") does not fall within the valid range of values [1.." 
                                     + dimension_txt 
                                     + "].");
        error_description.push_back("Offending <i_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      // Check that the value of the "n" attribute is not repeated.

      if (clist->exists_item(n))
      {
        //
        // Repeated "n" attribute value.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><i_spec><c_list><item>";

        error_description.push_back("Repeated value for the item n attribute (" + n_txt + ").");
        error_description.push_back("Offending <i_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      // Set the value of the "n" attribute.

      item->set_n(n);

      // Let's get and check the type of the item.

      dp->goto_label_relative("type");
      item_type = dp->_label_value_string();

      if (!validate_ispec_clist_item_type(item_type, is_scalar, matrix_dimensions))
      {
        //
        // The syntax or even the semantics of the type tag is not correct.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><i_spec><c_list><item><type>";

        error_description.push_back("Invalid syntax.");
        error_description.push_back("Use either 'scalar', 'SCALAR', 'matrix(n,m,...,t)' or 'MATRIX(n,m,...,t)'");
        error_description.push_back("where n, m, ... , t stand for integer values > 1.");
        error_description.push_back("Offending <i_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      //
      // Well, the validation function above will tell us if the type is
      // "SCALAR" or "MATRIX". In the first case there's nothing to do,
      // since the type subobject of the gp_clist_item object is created
      // as a scalar by default.
      //
      // On the contrary, if it is not a scalar type, then we must set
      // explicitly the type to matrix, indicating the values of the
      // different dimensions found by the validator.
      //
      // Note that we must expect NO errors, since the preconditions of
      // the set_type_to_matrix() method below have been guaranteed by
      // validate_ispec_clist_item_type() above.
      //

      if (!is_scalar) item->set_type_to_matrix(matrix_dimensions);

      dp->go_back(); // Back to item.

      // Parse the <ref> element.

      dp->goto_label_relative("ref");

      ref = item->get_ref_ptr();
      status = parse_ref(dp, ref);

      if (!status)
      {
        //
        // Errors parsing the ref element.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><i_spec><c_list><item><ref>";

        error_description.push_back("The syntax of the list of codes in the <coor_ref_frame_VC> or ");
        error_description.push_back("<ref_frame_VC> or <coor_system_VC> is not correct.");
        error_description.push_back("Offending <i_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      // Validate the ref object just parsed.

      status = validate_ref(ref);

      if (!status)
      {
        //
        // Problems with the number of codes found. We'll report the error
        // and keep going to detect as many problems as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><i_spec><c_list><item><ref>";
        error_description.push_back("Invalid number of elements in <ref> specification.");
        error_description.push_back("The number of codes in reference frame and coordinate");
        error_description.push_back("system lists do not match.");
        error_description.push_back("Offending <i_spec> lineage identifier: " + lineage_id);

        tmp_msg = build_message(2, filename, error_tag, error_description);
        error_messages.push_back(tmp_msg);

        result = 2;
      }

      dp->go_back(); // Back to item.

      // Parse the OPTIONAL units element.

      try
      {
        dp->goto_label_relative("units");
        units = item->get_unit_ptr();

        //
        // When using a schema, the preconditions of the next method are always
        // guaranteed, so there is only two reasons to fail: the number of
        // units parsed is not exactly one or the unit itself is not correct.
        //

        retcode = parse_units(dp, units_manager, 1, units);

        if (retcode == 2)
        {

          //
          // Invalid number of units found. We'll keep going, however, to detect
          // as many errors as possible.
          //

          tostring_converter.str("");
          tostring_converter << units->get_dim();
          tmp_txt = tostring_converter.str();

          error_description.clear();

          error_tag = "<nav_metadata_file><i_spec><c_list><item><units>";

          error_description.push_back("Multiple unit (" + tmp_txt + ") specification found in item element");
          error_description.push_back("Only a single unit specification is allowed.");
          error_description.push_back("Offending <i_spec> lineage identifier: " + lineage_id);
          tmp_msg = build_message(2, filename, error_tag, error_description);

          error_messages.push_back(tmp_msg);

          result = 2;
        }
        else if (retcode == 3)
        {
          // Invalid unit found.

          error_description.clear();

          error_tag = "<nav_metadata_file><i_spec><c_list><item><units>";

          error_description.push_back("Invalid syntax for unit specification found in item element");
          error_description.push_back("Offending <i_spec> lineage identifier: " + lineage_id);
          tmp_msg = build_message(2, filename, error_tag, error_description);

          error_messages.push_back(tmp_msg);

          result = 2;
        }

        dp->go_back(); // Back to item.
      }
      catch (...)
      {
        // No problem. The units element is optional, so there's nothing to do.
      }

      // Parse the OPTIONAL c (covariance) element.

      try
      {
        dp->goto_label_relative("c");
        covariance = item->get_covariance_ptr();

        // Parse the covariance element.

        retcode = parse_covariance(filename, dp,
                                   "<nav_metadata_file><i_spec><c_list><item><c>",
                                   lineage_id, covariance, tmp_msg);

        // Check errors.

        if (retcode == 2)
        {
          //
          // Errors parsing the covariance. We'll keep going to identify
          // as many errors as possible.
          //

          error_messages.push_back(tmp_msg);
          result = 2;
        }

        dp->go_back(); // Back to item.
      }
      catch (...)
      {
        // No problem. The c element is optional, so there's nothing to do.
      }


      // Parse the OPTIONAL s (scale) element.

      try
      {
        dp->goto_label_relative("s");
        scale = item->get_scale_ptr();

        // Parse the scale.

        retcode = parse_scale(filename, dp, 
                              "<nav_metadata_file><i_spec><c_list><item><s>",
                              lineage_id, scale, tmp_msg);

        // Check errors.

        if (retcode == 2) 
        {
          //
          // Parsing the scale reported errors. We'll keep going to find as
          // many errors as possible.
          //

          error_messages.push_back(tmp_msg);
          result = 2;
        }

        dp->go_back(); // Back to item.
      }
      catch (...)
      {
        // No problem. The s element is optional, so there's nothing to do.
      }

      dp->go_back(); // Back to c_list.

      //
      // Insert the new item into the c_list. But only when
      // its "n" attribute is not repeated.
      //

      if (!clist->exists_item(n)) clist->add_item(*item);
      delete item;
      item = NULL;
    }

    return result;
  }
}

int
gp_utils::
parse_covariance
(const string&  filename,
 adp_DOMparser* dp,
 const string&  tag_parsed,
 const string&  lineage_id,
 gp_covariance* cov,
 string&        error_message)
{
  int result;
  {
    double         ddata;
    vector<string> error_description;
    string         error_tag;
    int            i;
    string         sdata;
    string         sdata2;
    bool           status;
    vector<string> vsdata;

    // Check our preconditions.

    if (filename.empty())   return 1;
    if (dp == NULL)         return 1;
    if (tag_parsed.empty()) return 1;
    if (lineage_id.empty()) return 1;
    if (cov == NULL)        return 1;

    // Initialize.

    result = 0;
    error_message = "";


    // Get the list of covariance values.

    sdata = dp->_label_value_string();

    //
    // "Simplfy" all whitespace. This will keep a single blank char
    // between each covariance value. We need it to be able to split
    // the list of covariance values into single elements.
    //

    sdata2 = string_whitespace_simplify(sdata);

    // Split the list into tokens. Covariance values are separated by blank spaces!

    vsdata = string_tokenize(sdata2,' ');

    // Set the size of the covariance list in the covariance object.

    cov->set_dim((int)vsdata.size());

    //
    // Add each single covariance value to the covariance list. Note that a conversion
    // from string to double is needed, and that this conversion may
    // FAIL.
    //

    for (i = 0; i < (int)vsdata.size(); i++)
    {
      status = string_convert_to_double(vsdata[i],ddata);

      if (status)
      {
        cov->set_value(i, ddata);
      }
      else
      {
        //
        // Error detected while converting a string to double form. Report this
        // and LEAVE IMMEDIATELY the conversion loop.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file>" + tag_parsed + "<s>";
        error_description.push_back("Invalid value found for " + tag_parsed + " covariance matrix element: " + vsdata[i]);
        error_description.push_back("Offending " + tag_parsed + "'s lineage identifier: " + lineage_id);

        error_message = build_message(2, filename, error_tag, error_description);

        result = 2;
        break;
      }
    }
  }

  return result;
}

int
gp_utils::  
parse_ilist
(const string&   filename,
 adp_DOMparser*  dp,
 string&         lineage_id,
 gp_ilist*       ilist,
 vector<string>& error_messages)
{
  int result;

  {
    int            dimension;
    string         dimension_txt;
    vector<string> error_description;
    string         error_tag;
    string         identifier;
    int            iitem_index;
    string         iitem_type;
    int            n_iitems;
    string         n_iitems_txt;
    int            n;
    string         n_txt;
    string         tmp_msg;
    string         tmp_txt;
    stringstream   tostring_converter;

    // Check our preconditions.

    if (filename.empty())   return 1;
    if (dp == NULL)         return 1;
    if (lineage_id.empty()) return 1;
    if (ilist == NULL)      return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    // Get the actual number of item elements in the i_list element.

    n_iitems = dp->_n_of_repeated_sublabels("item");
    tostring_converter.str("");
    tostring_converter << n_iitems;
    n_iitems_txt = tostring_converter.str();

    // Get the declared dimension of the i_list.

    dp->goto_label_relative("dimension");
    dimension = dp->_label_value_int();
    dimension_txt = dp->_label_value_string();

    ilist->set_dim(dimension);

    dp->go_back(); // Back to i_list.

    //
    // Check that the actual and declared dimension match.
    // Report an error if they don't match, but keep working to
    // detect as many errors as possible.
    //

    if (dimension != n_iitems)
    {
      error_description.clear();

      error_tag = "<nav_metadata_file><m_spec><i_list><dimension>";

      error_description.push_back("The value of the <dimension> element ("
                                   + dimension_txt
                                   + ") does not match with actual number of <item> elements (" 
                                   + n_iitems_txt 
                                   + ").");
      error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
      tmp_msg = build_message(2, filename, error_tag, error_description);

      error_messages.push_back(tmp_msg);

      result = 2;
    }

    // Parse the n_iitems item elements that have been found.

    for (iitem_index = 1; iitem_index <= n_iitems; iitem_index++)
    {

      // Go to the item at position iitem_index.

      dp->goto_label_relative("item",iitem_index);

      // Get its "n" attribute in both numeric and textual forms.

      n     = dp->_label_attribute_int("n");
      n_txt = dp->_label_attribute_string("n");

      //
      // Check that the value of the "n" attribute is within the
      // valid range [1..dimension].
      //

      if ((n < 1) || (n > dimension))
      {
        //
        // The value of the "n" attribute is not in the valid range.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><m_spec><i_list><item>";

        error_description.push_back("The value of the n attribute ("
                                     + n_txt
                                     + ") does not fall within the valid range of values [1.." 
                                     + dimension_txt 
                                     + "].");
        error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      // Check that the value of the "n" attribute is not repeated.

      if (ilist->exists_item(n))
      {
        //
        // Repeated "n" attribute value.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><m_spec><i_list><item>";

        error_description.push_back("Repeated value for the item n attribute (" + n_txt + ").");
        error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }


      // Let's get the identifier of the item.

      dp->goto_label_relative("id");

      identifier = dp->_label_value_string();

      dp->go_back(); // Back to item (iitem_index).


      dp->go_back(); // Back to i_list.

      //
      // Add the new item (couple made of the n attribute plus the
      // identifier) to the i_list.
      // We won't do that if the current item has a repeated n
      // attribute.
      //

      if (!ilist->exists_item(n)) ilist->set_item_id(n, identifier);
    }

    return result;
  }
}

int
gp_utils::
parse_ispec
(const string&     filename,
 adp_DOMparser*    dp,
 gp_units_manager* units_manager,
 gp_ispec*         ispec,
 vector<string>&   error_messages)
{
  int result;

  {
    gp_tspec*      tspec;
    bool           active;
    gp_clist*      clist;
    vector<string> error_list;
    lineage_data*  lineage;
    string         lineage_id;
    int            retcode;
    string         sattr;
    string         sdata;
    string         sdata2;
    gp_timespec*   timespec;

    // Check our preconditions.

    if (filename.empty())           return 1;
    if (dp == NULL)                 return 1;
    if (units_manager == NULL)      return 1;
    if (!units_manager->is_valid()) return 1;
    if (ispec == NULL)              return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    //
    // The i_spec may be disabled through the "s" attribute.
    // Let's check it.
    //

    active = false;

    sattr = dp->_label_attribute_string("s");
    if (sattr == "a") active = true;

    ispec->set_active(active);

    //
    // We'll parse the i_spec element no matter it is active or not to
    // detect possible errors in the input file.
    //


    // Parse the lineage element

    //
    // Get a pointer to the lineage_data object included in the ispec object.
    // This simplifies the procedure of setting all its members.
    //

    dp->goto_label_relative("lineage");
    lineage = ispec->get_lineage_ptr();

    //
    // Parse it. Note that the parse_lineage method fails only
    // when preconditions are not matched. Since we're sure that
    // this won't happen, we may safely ignore its return code.
    //

    parse_lineage(dp, lineage);
    lineage_id = lineage->id_get();


    dp->go_back(); // Back to i_spec.

    // --- Parse the type element.

    dp->goto_label_relative("type");
    sdata = dp->_label_value_string();
    sdata2 = string_whitespace_remove_all(sdata);
    ispec->set_type(sdata2);
    dp->go_back(); // Back to i_spec.

    // Parse the toolbox element. Be careful. It is optional.

    try
    {
      dp->goto_label_relative("toolbox");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_remove_all(sdata);
      ispec->set_toolbox(sdata2);
      dp->go_back(); // Back to i_spec.
    }
    catch (...)
    {
      // No problem. The toolbox is optional, so there's nothing to do.
    }

    // Parse the time_spec element.

    dp->goto_label_relative("time_spec");
    timespec = ispec->get_timespec_ptr();

    //
    // Precondition violations should not arise when calling parse_timespec.
    // Some of them are this method's ones, and are satisfied. The others are
    // also fulfilled when calling this method below. So the only possible
    // retcode we'll get will be related to a malformed time_spec ref / units
    // sub-elements. parse_timespec builds its own error messages, so in case
    // of errors will copy these to the global error list.
    //

    retcode = parse_timespec(filename, dp, units_manager, "<i_spec>", lineage_id, timespec, error_list);
    if (retcode != 0)
    {
      error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
      result = 2;
    }

    dp->go_back(); // Back to i_spec.

    // Parse the c_list element

    dp->goto_label_relative("c_list");
    clist = ispec->get_clist_ptr();

    //
    // The actual parsing process is done below.
    //
    // This method may fail because of two reasons: (1) preconditions and
    // (2) actual parsing problems. We're sure that preconditions are
    // guaranteeed, so we only have to take care of the second situation.
    // Should this happen, we'll copy the list of errors returned by
    // parse_clist to our global list of errors.
    //
    // At any rate, we'll keep going to detect as many errors as possible.
    //

    retcode = parse_clist(filename, dp, units_manager, lineage_id, clist, error_list);

    if (retcode != 0)
    {
      error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
      result = 2;
    }

    dp->go_back(); // Back to i_spec.

    // Parse the OPTIONAl t_spec element.

    try
    {
      dp->goto_label_relative("t_spec");
      tspec = ispec->get_tspec_ptr();

      //
      // The actual parsing process is done below.
      //
      // This method may fail because of two reasons: (1) preconditions and
      // (2) actual parsing problems. We're sure that preconditions are
      // guaranteeed, so we only have to take care of the second situation.
      // Should this happen, we'll copy the list of errors returned by
      // parse_tspec to our global list of errors.
      //
      // At any rate, we'll keep going to detect as many errors as possible.
      //

      retcode = parse_tspec(filename, dp, units_manager, "<i_spec>", lineage_id, tspec, error_list);

      if (retcode != 0)
      {
        error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
        result = 2;
      }

      dp->go_back(); // Back to i_spec.
    }
    catch (...)
    {
      // The t_spec element is optional, so there's nothing we should do.
    }
  }

  return result;
}

bool
gp_utils::
parse_lineage
(adp_DOMparser* dp,
 lineage_data*  lineage)
{
  bool result;

  {
    int    i;
    string sdata;
    string sdata2;
    int    total_sub_items;

    if (dp == NULL)      return false;
    if (lineage == NULL) return false;

    result = true;

    // Lineage :id (mandatory).

    dp->goto_label_relative("id");
    sdata = dp->_label_value_string();
    sdata2 = string_whitespace_simplify(sdata);

    lineage->id_set(sdata2);

    dp->go_back(); // Back to lineage.

    // Lineage : name (optional).

    try
    {
      dp->goto_label_relative("name");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_simplify(sdata);

      lineage->name_set(sdata2);

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label name not found. No problem. It is optional. Nothing to do.
    }

    // Lineage : autor (optional).

    try
    {
      dp->goto_label_relative("author");

      //
      // Get the number of items under the author tag. Since
      // the tag author is present, the schema will guarantee
      // that at least one <item> sublabel exists.
      //

      total_sub_items = dp->_n_of_repeated_sublabels("item");

      // Retrieve all the items hanging from author.

      for (i = 1; i <= total_sub_items; i++)
      {
        dp->goto_label_relative("item", i);
        sdata = dp->_label_value_string();
        sdata2 = string_whitespace_simplify(sdata);

        lineage->author_item_add(sdata2);

        dp->go_back(); // Back to author.
      }

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label author not found. No problem. It is optional. Nothing to do.
    }

    // Lineage : organization (optional).

    try
    {
      dp->goto_label_relative("organization");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_simplify(sdata);

      lineage->organization_set(sdata2);

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label organization not found. No problem. It is optional. Nothing to do.
    }

    // Lineage : department (optional).

    try
    {
      dp->goto_label_relative("department");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_simplify(sdata);

      lineage->department_set(sdata2);

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label department not found. No problem. It is optional. Nothing to do.
    }

    // Lineage : date_time (optional).

    try
    {
      dp->goto_label("date_time");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_simplify(sdata);

      lineage->date_time_set(sdata2);

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label date_time not found. No problem. It is optional. Nothing to do.
    }

    // Lineage: ref_document (optional).

    try
    {
      dp->goto_label_relative("ref_document");

      //
      // Get the number of items under the ref_document tag. Since
      // the tag ref_document is present, the schema will guarantee
      // that at least one <item> sublabel exists.
      //

      total_sub_items = dp->_n_of_repeated_sublabels("item");


      // Retrieve all the items hanging from ref_document.

      for (i = 1; i <= total_sub_items; i++)
      {
        dp->goto_label_relative("item", i);
        sdata = dp->_label_value_string();
        sdata2 = string_whitespace_simplify(sdata);

        lineage->ref_document_item_add(sdata2);

        dp->go_back(); // Back to ref_document.
      }

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label ref_document not found. No problem. It is optional. Nothing to do.
    }

    // Lineage : project (optional).

    try
    {
      dp->goto_label_relative("project");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_simplify(sdata);

      lineage->project_set(sdata2);

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label project not found. No problem. It is optional. Nothing to do.
    }

    // Lineage : task (optional).

    try
    {
      dp->goto_label_relative("task");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_simplify(sdata);

      lineage->task_set(sdata2);

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label task not found. No problem. It is optional. Nothing to do.
    }

    // Lineage : remarks (optional).

    try
    {
      dp->goto_label_relative("remarks");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_simplify(sdata);

      lineage->remarks_set(sdata2);

      dp->go_back(); // Back to lineage.
    }
    catch (...)
    {
      // Label remarks not found. No problem. It is optional. Nothing to do.
    }

  }

  return result;
}

int
gp_utils::  
parse_llist
(const string&   filename,
 adp_DOMparser*  dp,
 string&         lineage_id,
 gp_llist*       llist,
 vector<string>& error_messages)
{
  int result;

  {
    int            dimension;
    string         dimension_txt;
    vector<string> error_description;
    string         error_tag;
    string         identifier;
    int            litem_index;
    string         litem_type;
    int            n_litems;
    string         n_litems_txt;
    int            n;
    string         n_txt;
    string         tmp_msg;
    string         tmp_txt;
    stringstream   tostring_converter;

    // Check our preconditions.

    if (filename.empty())   return 1;
    if (dp == NULL)         return 1;
    if (lineage_id.empty()) return 1;
    if (llist == NULL)      return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    // Get the actual number of item elements in the l_list.

    n_litems = dp->_n_of_repeated_sublabels("item");
    tostring_converter.str("");
    tostring_converter << n_litems;
    n_litems_txt = tostring_converter.str();

    // Get the declared dimension of the l_list.

    dp->goto_label_relative("dimension");
    dimension = dp->_label_value_int();
    dimension_txt = dp->_label_value_string();

    llist->set_dim(dimension);

    dp->go_back(); // Back to l_list.

    //
    // Check that the actual and declared dimension match.
    // Report an error if they don't match, but keep working to
    // detect as many errors as possible.
    //

    if (dimension != n_litems)
    {
      error_description.clear();

      error_tag = "<nav_metadata_file><m_spec><l_list><dimension>";

      error_description.push_back("The value of the <dimension> element ("
                                   + dimension_txt
                                   + ") does not match with actual number of <item> elements (" 
                                   + n_litems_txt 
                                   + ").");
      error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
      tmp_msg = build_message(2, filename, error_tag, error_description);

      error_messages.push_back(tmp_msg);

      result = 2;
    }

    // Parse the n_litems item elements that have been found.

    for (litem_index = 1; litem_index <= n_litems; litem_index++)
    {

      // Go to the item at position litem_index.

      dp->goto_label_relative("item",litem_index);

      // Get its "n" attribute in both numeric and textual forms.

      n     = dp->_label_attribute_int("n");
      n_txt = dp->_label_attribute_string("n");

      //
      // Check that the value of the "n" attribute is within the
      // valid range [1..dimension].
      //

      if ((n < 1) || (n > dimension))
      {
        //
        // The value of the "n" attribute is not in the valid range.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><m_spec><l_list><item>";

        error_description.push_back("The value of the n attribute ("
                                     + n_txt
                                     + ") does not fall within the valid range of values [1.." 
                                     + dimension_txt 
                                     + "].");
        error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      // Check that the value of the "n" attribute is not repeated.

      if (llist->exists_item(n))
      {
        //
        // Repeated "n" attribute value.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><m_spec><l_list><item>";

        error_description.push_back("Repeated value for the item n attribute (" + n_txt + ").");
        error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }


      // Let's get the identifier of the item.

      dp->goto_label_relative("id");

      identifier = dp->_label_value_string();

      dp->go_back(); // Back to item (litem_index).


      dp->go_back(); // Back to l_list.

      //
      // Add the new item (couple made of the n attribute plus the
      // identifier) to the l_list.
      // We won't do that if the current item has a repeated n
      // attribute.
      //

      if (!llist->exists_item(n)) llist->set_item_id(n, identifier);
    }

    return result;
  }
}

int
gp_utils::  
parse_lspec
(const string&     filename,
 adp_DOMparser*    dp,
 gp_units_manager* units_manager,
 gp_lspec*         lspec,
 vector<string>&   error_messages)
{
  int result;

  {
    gp_tspec*      tspec;
    bool           active;
    gp_covariance* cov;
    int            dimension;
    vector<string> error_description;
    vector<string> error_list;
    string         error_message;
    string         error_tag;
    lineage_data*  lineage;
    string         lineage_id;
    gp_ref*        ref;
    int            retcode;
    string         sattr;
    gp_scale*      scale;
    string         sdata;
    string         sdata2;
    bool           status;
    gp_timespec*   timespec;
    string         tmp_msg;
    gp_unit*       units;

    // Check our preconditions.

    if (filename.empty())           return 1;
    if (dp == NULL)                 return 1;
    if (units_manager == NULL)      return 1;
    if (!units_manager->is_valid()) return 1;
    if (lspec == NULL)              return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    //
    // The l_spec may be disabled through the "s" attribute.
    // Let's check it.
    //

    active = false;

    sattr = dp->_label_attribute_string("s");
    if (sattr == "a") active = true;

    lspec->set_active(active);

    //
    // We'll parse the l_spec element no matter it is active or not to
    // detect possible errors in the input file.
    //


    // Parse the lineage element

    //
    // Get a pointer to the lineage_data object included in the lspec object.
    // This simplifies the procedure of setting all its members.
    //

    dp->goto_label_relative("lineage");
    lineage = lspec->get_lineage_ptr();

    //
    // Parse it. Note that the parse_lineage method fails only
    // when preconditions are not matched. Since we're sure that
    // this won't happen, we may safely ignore its return code.
    //

    status = parse_lineage(dp, lineage);
    lineage_id = lineage->id_get();


    dp->go_back(); // Back to l_spec.

    // --- Parse the type element.

    dp->goto_label_relative("type");
    sdata = dp->_label_value_string();
    sdata2 = string_whitespace_remove_all(sdata);
    lspec->set_type(sdata2);
    dp->go_back(); // Back to l_spec.

    // Parse the toolbox element. Be careful. It is optional.

    try
    {
      dp->goto_label_relative("toolbox");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_remove_all(sdata);
      lspec->set_toolbox(sdata2);
      dp->go_back(); // Back to l_spec.
    }
    catch (...)
    {
      // No problem. The toolbox is optional, so there's nothing to do.
    }

    // --- Parse the dimension element.

    dp->goto_label_relative("dimension");
    dimension = dp->_label_value_int();
    lspec->set_dimension(dimension);
    dp->go_back(); // Back to l_spec.

    // Parse the ref element.

    dp->goto_label_relative("ref");
    ref = lspec->get_ref_ptr();

    status = parse_ref(dp, ref);

    if (!status)
    {
      //
      // Errors parsing the ref element. We'll report the error
      // and keep going to detect as many problems as possible.
      //

      error_description.clear();

      error_tag = "<nav_metadata_file><l_spec><ref>";
      error_description.push_back("The syntax of the list of codes in the <coor_ref_frame_VC> or ");
      error_description.push_back("<ref_frame_VC> or <coor_system_VC> is not correct.");
      error_description.push_back("Lineage identifier of the offending <l_spec> element: " + lineage_id);

      error_message = build_message(2, filename, error_tag, error_description);
      error_messages.push_back(error_message);

      result = 2;
    }

    // Validate the ref object just parsed.

    status = validate_ref(ref);

    if (!status)
    {
      //
      // Problems with the number of codes found. We'll report the error
      // and keep going to detect as many problems as possible.
      //

        error_description.clear();

        error_tag = "<nav_metadata_file><l_spec><ref>";
        error_description.push_back("Invalid number of elements in <ref> specification.");
        error_description.push_back("The number of codes in reference frame and coordinate");
        error_description.push_back("system lists do not match.");
        error_description.push_back("Lineage identifier of the offending <l_spec> element: " + lineage_id);

        error_message = build_message(2, filename, error_tag, error_description);
        error_messages.push_back(error_message);

        result = 2;
    }

    dp->go_back(); // Back to l_spec.

    // Parse the time_spec element.

    dp->goto_label_relative("time_spec");
    timespec = lspec->get_timespec_ptr();

    //
    // Precondition violations should not arise when calling parse_timespec.
    // Some of them are this method's ones, and are satisfied. The others are
    // also fulfilled when calling this method below. So the only possible
    // retcode we'll get will be related to a malformed time_spec ref / units
    // sub-elements. parse_timespec builds its own error messages, so in case
    // of errors will copy these to the global error list.
    //

    retcode = parse_timespec(filename, dp, units_manager, "<l_spec>", lineage_id, timespec, error_list);
    if (retcode != 0)
    {
      error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
      result = 2;
    }

    dp->go_back(); // Back to l_spec.

    // Parse the OPTIONAL units element

    try
    {
      dp->goto_label_relative("units");
      units = lspec->get_unit_ptr();

      //
      // When using a schema, the preconditions of the parse_units
      // method are guaranteed. Therefore, only two problems may
      // arise: invalid number of units or errors detected when
      // parsing the units.
      //

      retcode = parse_units(dp, units_manager, dimension, units);

      if (retcode == 2)
      {
        // Invalid number of units.

        error_description.clear();

        error_tag = "<nav_metadata_file><l_spec><units>";

        error_description.push_back("Invalid number of units found.");
        error_description.push_back("There must be exactly as many unit specs as the <dimension> element states.");
        error_description.push_back("Offending <l_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }
      else if (retcode == 3)
      {
        // Invalid unit found.

        error_description.clear();

        error_tag = "<nav_metadata_file><l_spec><units>";

        error_description.push_back("Invalid syntax for one or more unit specifications");
        error_description.push_back("Offending <l_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      dp->go_back(); // Back to l_spec.
    }
    catch (...)
    {
      // No problem. The units are optional, so there's nothing to do.
    }

    // Parse the OPTIONAL scale (s) element.

    try
    {
      dp->goto_label_relative("s");
      scale = lspec->get_scale_ptr();

      //
      // The next method may fail because preconditions are violated
      // (this won't happen) or because actual errors have been detected.
      // We'll take care of the last case only.
      //

      retcode = parse_scale(filename, dp, "<l_spec>", lineage_id, scale, error_message);
      if (retcode == 2)
      {
        error_messages.push_back(error_message);
        result = 2;
      }

      dp->go_back(); // Back to l_spec.
    }
    catch (...)
    {
      // No problem. The scale is optional, so there's nothing to do.
    }

    // Parse the OPTIONAL covariance (c) element.

    try
    {
      dp->goto_label_relative("c");
      cov = lspec->get_covariance_ptr();

      //
      // The next method may fail because preconditions are violated
      // (this won't happen) or because actual errors have been detected.
      // We'll take care of the last case only.
      //

      retcode = parse_covariance(filename, dp, "<l_spec>", lineage_id, cov, error_message);
      if (retcode == 2)
      {
        error_messages.push_back(error_message);
        result = 2;
      }

      dp->go_back(); // Back to l_spec.
    }
    catch (...)
    {
      // No problem. The covariance is optional, so there's nothing to do.
    }


    // Parse the OPTIONAl t_spec element.

    try
    {
      dp->goto_label_relative("t_spec");
      tspec = lspec->get_tspec_ptr();

      //
      // The actual parsing process is done below.
      //
      // This method may fail because of two reasons: (1) preconditions and
      // (2) actual parsing problems. We're sure that preconditions are
      // guaranteeed, so we only have to take care of the second situation.
      // Should this happen, we'll copy the list of errors returned by
      // parse_tspec to our global list of errors.
      //
      // At any rate, we'll keep going to detect as many errors as possible.
      //

      retcode = parse_tspec(filename, dp, units_manager, "<l_spec>", lineage_id, tspec, error_list);

      if (retcode != 0)
      {
        error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
        result = 2;
      }

      dp->go_back(); // Back to l_spec.
    }
    catch (...)
    {
      // The t_spec element is optional, so there's nothing we should do.
    }
  }

  return result;
}

int
gp_utils::
parse_mspec
(const string&   filename,
 adp_DOMparser*  dp,
 gp_mspec*       mspec,
 vector<string>& error_messages)
{
  int result;

  {
    bool           active;
    bool           dynamic;
    vector<string> error_list;
    gp_ilist*      ilist;
    lineage_data*  lineage;
    string         lineage_id;
    gp_llist*      llist;
    gp_plist*      plist;
    int            retcode;
    string         sattr;
    string         sdata;
    string         sdata2;
    gp_sub_m_list* smlist;

    // Check our preconditions.

    if (filename.empty()) return 1;
    if (dp == NULL)       return 1;
    if (mspec == NULL)    return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    //
    // The m_spec may be disabled through the "s" attribute.
    // Let's check it.
    //

    active = false;

    sattr = dp->_label_attribute_string("s");
    if (sattr == "a") active = true;

    mspec->set_active(active);

    //
    // We'll parse the m_spec element no matter it is active or not to
    // detect possible errors in the input file.
    //


    // Parse the lineage element

    //
    // Get a pointer to the lineage_data object included in the mspec object.
    // This simplifies the procedure of setting all its members.
    //

    dp->goto_label_relative("lineage");
    lineage = mspec->get_lineage_ptr();

    //
    // Parse it. Note that the parse_lineage method fails only
    // when preconditions are not matched. Since we're sure that
    // this won't happen, we may safely ignore its return code.
    //

    parse_lineage(dp, lineage);
    lineage_id = lineage->id_get();


    dp->go_back(); // Back to m_spec.

    // --- Parse the type element.

    dp->goto_label_relative("type");
    sdata = dp->_label_value_string();
    sdata2 = string_whitespace_remove_all(sdata);
    mspec->set_type(sdata2);
    dp->go_back(); // Back to m_spec.

    // Parse the toolbox element. Be careful. It is optional.

    try
    {
      dp->goto_label_relative("toolbox");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_remove_all(sdata);
      mspec->set_toolbox(sdata2);
      dp->go_back(); // Back to i_spec.
    }
    catch (...)
    {
      // No problem. The toolbox is optional, so there's nothing to do.
    }

    // --- Parse the dynamic element.

    dp->goto_label_relative("dynamic");
    sdata = dp->_label_value_string();
    sdata2 = string_whitespace_remove_all(sdata);
    // Convert to lowercase.
    std::transform(sdata2.begin(), sdata2.end(), sdata2.begin(), ::tolower);
    // Get the bool value and set the attribute in the m_spec object.
    dynamic = (sdata2 == "yes");
    mspec->set_dynamic(dynamic);
    dp->go_back(); // Back to m_spec.

    // Parse the l_list element

    dp->goto_label_relative("l_list");
    llist = mspec->get_llist_ptr();

    //
    // The actual parsing process is done below.
    //
    // This method may fail because of two reasons: (1) preconditions and
    // (2) actual parsing problems. We're sure that preconditions are
    // guaranteeed, so we only have to take care of the second situation.
    // Should this happen, we'll copy the list of errors returned by
    // parse_llist to our global list of errors.
    //
    // At any rate, we'll keep going to detect as many errors as possible.
    //

    retcode = parse_llist(filename, dp, lineage_id, llist, error_list);

    if (retcode != 0)
    {
      error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
      result = 2;
    }

    dp->go_back(); // Back to m_spec.

    // Parse the p_list element

    dp->goto_label_relative("p_list");
    plist = mspec->get_plist_ptr();

    //
    // The actual parsing process is done below.
    //
    // Same considerations than above about preconditions / errors (see the
    // parsing of the <l_list> element.
    //

    retcode = parse_plist(filename, dp, lineage_id, plist, error_list);

    if (retcode != 0)
    {
      error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
      result = 2;
    }

    dp->go_back(); // Back to m_spec.

    // Parse the OPTIONAL i_list element

    try
    {

      dp->goto_label_relative("i_list");
      ilist = mspec->get_ilist_ptr();

      //
      // The actual parsing process is done below.
      //
      // Same considerations than above about preconditions / errors (see the
      // parsing of the <l_list> element.
      //

      retcode = parse_ilist(filename, dp, lineage_id, ilist, error_list);

      if (retcode != 0)
      {
        error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
        result = 2;
      }

      dp->go_back(); // Back to m_spec.
    }
    catch (...)
    {
      // The i_list element is optional, so there's nothing we should do.
    }

    // Parse the OPTIONAL sub-m_list element

    try
    {

      dp->goto_label_relative("sub-m_list");
      smlist = mspec->get_sub_m_list_ptr();

      //
      // The actual parsing process is done below.
      //
      // Same considerations than above about preconditions / errors (see the
      // parsing of the <l_list> element.
      //

      retcode = parse_sub_m_list(filename, dp, lineage_id, smlist, error_list);

      if (retcode != 0)
      {
        error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
        result = 2;
      }

      dp->go_back(); // Back to m_spec.
    }
    catch (...)
    {
      // The sub-m_list element is optional, so there's nothing we should do.
    }
  }

  return result;
}

int
gp_utils::  
parse_plist
(const string&   filename,
 adp_DOMparser*  dp,
 string&         lineage_id,
 gp_plist*       plist,
 vector<string>& error_messages)
{
  int result;

  {
    int            dimension;
    string         dimension_txt;
    vector<string> error_description;
    string         error_tag;
    string         identifier;
    int            n_pitems;
    string         n_pitems_txt;
    int            n;
    string         n_txt;
    gp_plist_item*      pitem;
    int            pitem_index;
    string         pitem_type;
    string         role;
    string         tmp_msg;
    string         tmp_txt;
    stringstream   tostring_converter;

    // Check our preconditions.

    if (filename.empty())   return 1;
    if (dp == NULL)         return 1;
    if (lineage_id.empty()) return 1;
    if (plist == NULL)      return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    // Get the actual number of item elements in the p_list element.

    n_pitems = dp->_n_of_repeated_sublabels("item");
    tostring_converter.str("");
    tostring_converter << n_pitems;
    n_pitems_txt = tostring_converter.str();

    // Get the declared dimension of the p_list.

    dp->goto_label_relative("dimension");
    dimension = dp->_label_value_int();
    dimension_txt = dp->_label_value_string();
    plist->set_dim(dimension);

    dp->go_back(); // Back to p_list.

    //
    // Check that the actual and declared dimension match.
    // Report an error if they don't match, but keep working to
    // detect as many errors as possible.
    //

    if (dimension != n_pitems)
    {
      error_description.clear();

      error_tag = "<nav_metadata_file><m_spec><p_list><dimension>";

      error_description.push_back("The value of the <dimension> element ("
                                   + dimension_txt
                                   + ") does not match with actual number of <item> elements (" 
                                   + n_pitems_txt 
                                   + ").");
      error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
      tmp_msg = build_message(2, filename, error_tag, error_description);

      error_messages.push_back(tmp_msg);

      result = 2;
    }

    // Parse the n_pitems item elements that have been found.

    for (pitem_index = 1; pitem_index <= n_pitems; pitem_index++)
    {

      // Instantiate a new item element.

      pitem = new gp_plist_item();

      // Go to the item at position pitem_index.

      dp->goto_label_relative("item",pitem_index);

      // Get its "n" attribute in both numeric and textual forms.

      n     = dp->_label_attribute_int("n");
      n_txt = dp->_label_attribute_string("n");

      //
      // Check that the value of the "n" attribute is within the
      // valid range [1..dimension].
      //

      if ((n < 1) || (n > dimension))
      {
        //
        // The value of the "n" attribute is not in the valid range.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><m_spec><p_list><item>";

        error_description.push_back("The value of the n attribute ("
                                     + n_txt
                                     + ") does not fall within the valid range of values [1.." 
                                     + dimension_txt 
                                     + "].");
        error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      // Check that the value of the "n" attribute is not repeated.

      if (plist->exists_item(n))
      {
        //
        // Repeated "n" attribute value.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><m_spec><p_list><item>";

        error_description.push_back("Repeated value for the item n attribute (" + n_txt + ").");
        error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      // Set the value of the "n" attribute.

      pitem->set_n(n);

      // Let's get the identifier of the item.

      dp->goto_label_relative("id");

      identifier = dp->_label_value_string();
      pitem->set_id(identifier);

      dp->go_back(); // Back to item (pitem_index).

      // Get now the role of the item.

      dp->goto_label_relative("role");

      role = dp->_label_value_string();
      pitem->set_role(role);

      dp->go_back(); // Back to item (pitem_index).


      dp->go_back(); // Back to p_list.

      //
      // Add the new item to the p_list.
      // We won't do that if the current item has a repeated n
      // attribute.
      //

      if (!plist->exists_item(n)) plist->add_item(*pitem);
      delete pitem;
      pitem = NULL;
    }

    return result;
  }
}

int
gp_utils::  
parse_pspec
(const string&     filename,
 adp_DOMparser*        dp,
 gp_units_manager* units_manager,
 gp_pspec*         pspec,
 vector<string>&   error_messages)
{
  int result;

  {
    gp_tspec*      tspec;
    bool           active;
    gp_covariance* cov;
    int            dimension;
    vector<string> error_description;
    vector<string> error_list;
    string         error_message;
    string         error_tag;
    lineage_data*  lineage;
    string         lineage_id;
    gp_ref*        ref;
    int            retcode;
    string         sattr;
    gp_scale*      scale;
    string         sdata;
    string         sdata2;
    bool           status;
    gp_timespec*   timespec;
    string         tmp_msg;
    gp_unit*       units;

    // Check our preconditions.

    if (filename.empty())           return 1;
    if (dp == NULL)                 return 1;
    if (units_manager == NULL)      return 1;
    if (!units_manager->is_valid()) return 1;
    if (pspec == NULL)              return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    //
    // The p_spec may be disabled through the "s" attribute.
    // Let's check it.
    //

    active = false;

    sattr = dp->_label_attribute_string("s");
    if (sattr == "a") active = true;

    pspec->set_active(active);

    //
    // We'll parse the p_spec element no matter it is active or not to
    // detect possible errors in the input file.
    //


    // Parse the lineage element

    //
    // Get a pointer to the lineage_data object included in the pspec object.
    // This simplifies the procedure of setting all its members.
    //

    dp->goto_label_relative("lineage");
    lineage = pspec->get_lineage_ptr();

    //
    // Parse it. Note that the parse_lineage method fails only
    // when preconditions are not matched. Since we're sure that
    // this won't happen, we may safely ignore its return code.
    //

    status = parse_lineage(dp, lineage);
    lineage_id = lineage->id_get();


    dp->go_back(); // Back to p_spec.

    // --- Parse the type element.

    dp->goto_label_relative("type");
    sdata = dp->_label_value_string();
    sdata2 = string_whitespace_remove_all(sdata);
    pspec->set_type(sdata2);
    dp->go_back(); // Back to p_spec.

    // Parse the toolbox element. Be careful. It is optional.

    try
    {
      dp->goto_label_relative("toolbox");
      sdata = dp->_label_value_string();
      sdata2 = string_whitespace_remove_all(sdata);
      pspec->set_toolbox(sdata2);
      dp->go_back(); // Back to p_spec.
    }
    catch (...)
    {
      // No problem. The toolbox is optional, so there's nothing to do.
    }

    // --- Parse the dimension element.

    dp->goto_label_relative("dimension");
    dimension = dp->_label_value_int();
    pspec->set_dimension(dimension);
    dp->go_back(); // Back to p_spec.

    // Parse the ref element.

    dp->goto_label_relative("ref");
    ref = pspec->get_ref_ptr();

    status = parse_ref(dp, ref);

    if (!status)
    {
      //
      // Errors parsing the ref element. We'll report the error
      // and keep going to detect as many problems as possible.
      //

      error_description.clear();

      error_tag = "<nav_metadata_file><p_spec><ref>";
      error_description.push_back("The syntax of the list of codes in the <coor_ref_frame_VC> or ");
      error_description.push_back("<ref_frame_VC> or <coor_system_VC> is not correct.");
      error_description.push_back("Lineage identifier of the offending <p_spec> element: " + lineage_id);

      error_message = build_message(2, filename, error_tag, error_description);
      error_messages.push_back(error_message);

      result = 2;
    }

    // Validate the ref object just parsed.

    status = validate_ref(ref);

    if (!status)
    {
      //
      // Problems with the number of codes found. We'll report the error
      // and keep going to detect as many problems as possible.
      //

        error_description.clear();

        error_tag = "<nav_metadata_file><p_spec><ref>";
        error_description.push_back("Invalid number of elements in <ref> specification.");
        error_description.push_back("The number of codes in reference frame and coordinate");
        error_description.push_back("system lists do not match.");
        error_description.push_back("Lineage identifier of the offending <p_spec> element: " + lineage_id);

        error_message = build_message(2, filename, error_tag, error_description);
        error_messages.push_back(error_message);

        result = 2;
    }

    dp->go_back(); // Back to p_spec.

    // Parse the time_spec element.

    dp->goto_label_relative("time_spec");
    timespec = pspec->get_timespec_ptr();

    //
    // Precondition violations should not arise when calling parse_timespec.
    // Some of them are this method's ones, and are satisfied. The others are
    // also fulfilled when calling this method below. So the only possible
    // retcode we'll get will be related to a malformed time_spec ref / units
    // sub-elements. parse_timespec builds its own error messages, so in case
    // of errors will copy these to the global error list.
    //

    retcode = parse_timespec(filename, dp, units_manager, "<p_spec>", lineage_id, timespec, error_list);
    if (retcode != 0)
    {
      error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
      result = 2;
    }

    dp->go_back(); // Back to p_spec.

    // Parse the OPTIONAL units element

    try
    {
      dp->goto_label_relative("units");
      units = pspec->get_unit_ptr();

      //
      // When using a schema, the preconditions of the parse_units
      // method are guaranteed. Therefore, only two problems may
      // arise: invalid number of units or errors detected when
      // parsing the units.
      //

      retcode = parse_units(dp, units_manager, dimension, units);

      if (retcode == 2)
      {
        // Invalid number of units.

        error_description.clear();

        error_tag = "<nav_metadata_file><p_spec><units>";

        error_description.push_back("Invalid number of units found.");
        error_description.push_back("There must be exactly as many unit specs as the <dimension> element states.");
        error_description.push_back("Offending <p_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }
      else if (retcode == 3)
      {
        // Invalid unit found.

        error_description.clear();

        error_tag = "<nav_metadata_file><p_spec><units>";

        error_description.push_back("Invalid syntax for one or more unit specifications");
        error_description.push_back("Offending <p_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      dp->go_back(); // Back to p_spec.
    }
    catch (...)
    {
      // No problem. The units are optional, so there's nothing to do.
    }

    // Parse the OPTIONAL scale (s) element.

    try
    {
      dp->goto_label_relative("s");
      scale = pspec->get_scale_ptr();

      //
      // The next method may fail because preconditions are violated
      // (this won't happen) or because actual errors have been detected.
      // We'll take care of the last case only.
      //

      retcode = parse_scale(filename, dp, "<p_spec>", lineage_id, scale, error_message);
      if (retcode == 2)
      {
        error_messages.push_back(error_message);
        result = 2;
      }

      dp->go_back(); // Back to p_spec.
    }
    catch (...)
    {
      // No problem. The scale is optional, so there's nothing to do.
    }

    // Parse the OPTIONAL covariance (c) element.

    try
    {
      dp->goto_label_relative("c");
      cov = pspec->get_covariance_ptr();

      //
      // The next method may fail because preconditions are violated
      // (this won't happen) or because actual errors have been detected.
      // We'll take care of the last case only.
      //

      retcode = parse_covariance(filename, dp, "<p_spec>", lineage_id, cov, error_message);
      if (retcode == 2)
      {
        error_messages.push_back(error_message);
        result = 2;
      }

      dp->go_back(); // Back to p_spec.
    }
    catch (...)
    {
      // No problem. The covariance is optional, so there's nothing to do.
    }


    // Parse the OPTIONAl t_spec element.

    try
    {
      dp->goto_label_relative("t_spec");
      tspec = pspec->get_tspec_ptr();

      //
      // The actual parsing process is done below.
      //
      // This method may fail because of two reasons: (1) preconditions and
      // (2) actual parsing problems. We're sure that preconditions are
      // guaranteeed, so we only have to take care of the second situation.
      // Should this happen, we'll copy the list of errors returned by
      // parse_tspec to our global list of errors.
      //
      // At any rate, we'll keep going to detect as many errors as possible.
      //

      retcode = parse_tspec(filename, dp, units_manager, "<p_spec>", lineage_id, tspec, error_list);

      if (retcode != 0)
      {
        error_messages.insert(error_messages.end(), error_list.begin(), error_list.end());
        result = 2;
      }

      dp->go_back(); // Back to p_spec.
    }
    catch (...)
    {
      // The t_spec element is optional, so there's nothing we should do.
    }
  }

  return result;
}


bool
gp_utils::
parse_ref
(adp_DOMparser* dp,
 gp_ref*        ref)
{
  {
    int                 i;
    int                 error;
    gp_ref_code         ref_code;
    gp_ref_code_element ref_code_element;
    string              sdata;
    string              sdata2;
    vector<gp_ref_code> vsdata;
    vector<gp_ref_code> vsdata2;

    // Check our preconditions.

    if (dp == NULL)  return false;
    if (ref == NULL) return false;

    // Ascertain what type of ref we are dealing with and react consequently.

    try
    {
      // Trying to find a coordinate reference frame

      dp->goto_label_relative("coor_ref_frame_VC");

      //
      // At this point, we're sure that we've got it. Get the
      // list of codes.
      //

      sdata = dp->_label_value_string();

      // Remove all whitespace.

      sdata2 = string_whitespace_remove_all(sdata);

      // Parse the codes.

      error = parse_ref_code_list(sdata2, vsdata);
      if (error != 0) return false;

      // Set the type of the ref element.

      ref->set_type(gp_ref::GP_REF_TYPE_CRF);

      // Add each single code to the list of CRF codes.

      for (i = 0; i < (int)vsdata.size(); i++) ref->add_CRF(vsdata[i]);

      dp->go_back(); // Back to ref.
    }
    catch (...)
    {
      //
      // We couldn't find a coordinate reference frame. Therefore, we
      // MUST find a reference frame (the schema says so). Note that
      // if we're not using a schema and the tag is missing, an
      // uncontrolled exception will be thrown (that must be
      // catched by the calling module).
      //

      dp->goto_label_relative("ref_frame_VC");

      // We've got the reference frame tag. Get the list of codes.

      sdata = dp->_label_value_string();

      // Remove all whitespace

      sdata2 = string_whitespace_remove_all(sdata);

      // Parse the codes.

      error = parse_ref_code_list(sdata2, vsdata);
      if (error != 0) return false;

      // Set the type of the ref element.

      ref->set_type(gp_ref::GP_REF_TYPE_CS_RF);

      // Add each single code to the list of RF codes.

      for (i = 0; i < (int)vsdata.size(); i++) ref->add_RF(vsdata[i]);

      dp->go_back(); // Back to ref.

      //
      // We need to ascertain here if the coordinate system list
      // of codes, which is optional, is present or not.
      //

      try
      {

        dp->goto_label_relative("coor_system_VC");

        //
        // We've just found the list of coordinate system codes.
        // Get it.
        //

        sdata = dp->_label_value_string();

        // Remove all whitespace.

        sdata2 = string_whitespace_remove_all(sdata);

        // Parse the codes.

        error = parse_ref_code_list(sdata2, vsdata2);
        if (error != 0) return false;

        // Add each single code to the list of CS codes.

        for (i = 0; i < (int)vsdata2.size(); i++) ref->add_CS(vsdata2[i]);

        dp->go_back(); // Back to ref.
      }
      catch (...)
      {
        //
        // No coordinate system. We'll add as many "-" codes as necessary
        // (as many as present in the reference frame list). Note that the
        // dash ("-") stands for "no code available".
        //

        ref_code_element.set_code("-");
        ref_code.add_element(ref_code_element);

        for (i = 0; i < ref->get_dim_RF(); i++) ref->add_CS(ref_code);
      }
    }
  }

  return true;
}

int
gp_utils::
parse_ref_code_list
(const string & code_list_text,
 vector<gp_ref_code>& the_codes)
{
  {
    gp_ref_code*         a_ref_code;
    gp_ref_code_element* a_ref_code_element;
    string               code;
    int                  code_idx;
    vector<string>       codes;
    string               element;
    int                  element_idx;
    vector<string>       elements;
    int                  left;
    int                  len;
    int                  limit;
    bool                 open_parentheses;
    int                  pos_clos_par1, pos_clos_par2;
    int                  pos_open_par1, pos_open_par2;
    string               ref_code_id;
    string               ref_code_parameter;
    int                  right;
    size_t               where;

    //
    // WARNING: this method assumes that no whitespace exist in
    // the input code_list_text string.
    //

    //
    // Split the string using the commas as markers. Note that
    // commas inside parenthesis -- as in "(xxx, yyy, zzz)" are
    // NOT taken into account to split the input string (these
    // separate parameters, not full ref codes!).
    //

    //
    // But first, look for extra commas at the beginning or the end
    // of the input string.
    //

    limit = (int)code_list_text.size() - 1;

    if (code_list_text[0] == ',')     return 6;
    if (code_list_text[limit] == ',') return 6;

    left = right = 0;
    open_parentheses = false;

    while (right <= limit)
    {
      if (code_list_text[right] == '(')
      {
        open_parentheses = true;
        right++;
      }
      else if (code_list_text[right] == ')')
      {
        open_parentheses = false;
        right++;
      }
      else if (code_list_text[right] == ',')
      {
        if (open_parentheses)
        {
          right++;
        }
        else
        {
          //
          // Found a comma. Extract the substring
          // and push it to the list of CRFs. Note that
          // we exclude the comma located at CRF_text[right].
          //
          // Note also that if the length of the code is zero...
          // it means that we have two consecutive commas and
          // no text in between, which is an error.

          len = right - left;

          if (len == 0) return 6;

          codes.push_back(code_list_text.substr(left, len));

          // Update pointers to begin again.

          left = right + 1;
          right = left;
        }
      }
      else
      {
        // Regular character. Keep going.

        right++;
      }
    }

    // Do not forget to add the last segment, if any.

    right--; // Back to the last valid position in the CRF_text string.

    if (left <= right)
    {
      //
      // There is a CRF code at the tail of the CRF_text string.
      // It does not include a comma; if so, we would not be
      // processing it here but in the elseif above. That's
      // why we include the character pointed by CRF_text[right]
      //

      len = right - left + 1;
      codes.push_back(code_list_text.substr(left, len));
    }

    // At least, there must be ONE CRF code to process.

    if (codes.size() == 0) return 1;

    //
    // Now, we have the different, individual CRF codes stored in the
    // codes array.
    //

    //
    // Let's process each single CRF, taking into account their elements
    // and parameters.
    //

    for (code_idx = 0; code_idx < (int)codes.size(); code_idx++)
    {

      // Get the next (yet to be processed) CRF candidate.

      code = codes[code_idx];

      // Reset the list of elements for the current CRF candidate.

      elements.clear();

      //
      // Let's see how many elements it has. These are separated by
      // plus signs (+). Separate these to process them later
      // individually.
      //

      limit = (int)code.size() - 1;

      //
      // But first, check that no extra (at the beginning or end)
      // plus signs are present.
      //

      if (code[0] == '+')     return 7;
      if (code[limit] == '+') return 7;

      left = right = 0;

      while (right <= limit)
      {
        if (code[right] == '+')
        {
          //
          // Found a plus sign. Extract the substring
          // and push it to the list of elements. Note that
          // we exclude the plus sign located at codes[right].
          //
          // Note that if the length of the substring is zero,
          // it means that we have two consecutive plus signs,
          // which is an error.
          //

          len = right - left;

          if (len == 0) return 7;

          elements.push_back(code.substr(left, len));

          left = right + 1;
          right = left;
        }
        else
        {
          right++;
        }
      }

      // Don't forget the last element at the end of the code.

      right--; // Back to the last valid position in the code string.

      if (left <= right)
      {
        //
        // There is an element at the tail of the code string.
        // It does not include a plus sign; if so, we would not be
        // processing it here but in the elseif above. That's
        // why we include the character pointed by code[right]
        //

        len = right - left + 1;
        elements.push_back(code.substr(left, len));
      }

      // At least, one element must be part of the current ref code.

      if (elements.size() == 0) return 2;

      //
      // The different elements integrating the current ref code have
      // been identified. Now it's the time to analyze each
      // of these to find any parameters they could have.
      //

      a_ref_code = new gp_ref_code();

      for (element_idx = 0; element_idx < (int)elements.size(); element_idx++)
      {
        // Get the current element from the elements array.

        element = elements.at(element_idx);

        //
        // Build a new ref code element object so we can set its attributes
        // as soon as we find them.
        //

        a_ref_code_element = new gp_ref_code_element();

        // Let's see if the element has parameters.

        // Find opening and closing parentheses, if any.

        where = element.find_first_of("(");
        if (where == string::npos) pos_open_par1 = -1;
        else                       pos_open_par1 = (int)where;

        where = element.find_last_of("(");
        if (where == string::npos) pos_open_par2 = -1;
        else                       pos_open_par2 = (int)where;

        where = element.find_first_of(")");
        if (where == string::npos) pos_clos_par1 = -1;
        else                       pos_clos_par1 = (int)where;

        where = element.find_last_of(")");
        if (where == string::npos) pos_clos_par2 = -1;
        else                       pos_clos_par2 = (int)where;

        //
        // Check that the parentheses match and that these appear
        // in the right order.
        //

        if (pos_open_par1 != pos_open_par2) return 3; // Multiple (
        if (pos_clos_par1 != pos_clos_par2) return 3; // Multiple )
        if (pos_open_par1 >  pos_clos_par1) return 3; // )(

        if ((pos_open_par1  < 0) & (pos_clos_par1 >= 0)) return 3; // Only )
        if ((pos_open_par1 >= 0) & (pos_clos_par1 < 0))  return 3; // Only (

                                                                   //
                                                                   // Check if we have just a couple of parentheses witn no
                                                                   // text in between.
                                                                   //

        if ((pos_open_par1 != -1) && (pos_open_par1 == (pos_clos_par1 - 1))) return 4;

        //
        // Check if we just have a couple of parenthesis, with no
        // preceding code (that is, just a list of parameters).
        //

        if (pos_open_par1 == 0) return 5;

        //
        // Copy the identifier (the text found up to the opening
        // parentheses if any, or to the end of the string
        // otherwise).
        //

        if (pos_open_par1 != -1)
        {
          ref_code_id = element.substr(0, pos_open_par1);
        }
        else
        {
          ref_code_id = element.substr(0, element.size());
          left = right = -1;
        }

        a_ref_code_element->set_code(ref_code_id);

        //
        // Scan the parameters (if any) and copy these to the ref code
        // element when present. Note that there may be a single
        // parameter; in such case, there will be no commas in the text.
        //

        if (pos_open_par1 != -1)
        {
          // We've got at least one parameter.

          limit = pos_clos_par1 - 1;
          left = right = pos_open_par1 + 1;

          while (right <= limit)
          {
            if (element[right] == ',')
            {
              //
              // Found a comma. Extract the substring (parameter)
              // and add it to the ref code element. Note that
              // we exclude the comma located at element[right].
              //
              // Note also that if the length of the substring is
              // zero, it means that we have found two consecutive
              // commas, and this is an error.

              len = right - left;

              if (len == 0)
              {
                delete a_ref_code_element;
                delete a_ref_code;

                return 6;
              }

              ref_code_parameter = element.substr(left, len);
              a_ref_code_element->add_parameters(ref_code_parameter);

              left = right + 1;
              right = left;
            }
            else
            {
              right++;
            }
          }

          // Don't forget the last parameter at the end of the element.

          right--; // Back to the last valid position in the element string.

          if (left <= right)
          {
            //
            // There is a parameter at the tail of the element string.
            // It does not include a comma; if so, we would not be
            // processing it here but in the if above. That's
            // why we include the character pointed by element[right]
            //

            len = right - left + 1;
            ref_code_parameter = element.substr(left, len);
            a_ref_code_element->add_parameters(ref_code_parameter);
          }
        }

        //
        // We have processed completely the current element, either
        // when it has parameters or not.
        // It's therefore time to add the current ref code element to the
        // current ref code.
        //

        a_ref_code->add_element(*a_ref_code_element);

        // Destroy the current, local ref code element.

        delete a_ref_code_element;
        a_ref_code_element = NULL;
      }

      // Add the ref code to the list of ref codes found.

      the_codes.push_back(*a_ref_code);

      // Destroy the current, local ref code.

      delete a_ref_code;
      a_ref_code = NULL;
    }

    return 0;
  }
}

int
gp_utils::
parse_scale
(const string&  filename,
 adp_DOMparser* dp,
 const string&  tag_parsed,
 const string&  lineage_id,
 gp_scale*      scale,
 string&        error_message)
{
  int result;
  {
    double         ddata;
    int            i;
    vector<string> error_description;
    string         error_tag;
    string         sdata;
    string         sdata2;
    bool           status;
    vector<string> vsdata;

    // Check our preconditions.

    if (filename.empty())   return 1;
    if (dp == NULL)         return 1;
    if (tag_parsed.empty()) return 1;
    if (lineage_id.empty()) return 1;
    if (scale == NULL)      return 1;

    // Initialize.

    result = 0;
    error_message = "";

    // Get the list of scale values.

    sdata = dp->_label_value_string();

    //
    // "Simplfy" all whitespace. This will keep a single blank char
    // between each scale value. We need it to be able to split
    // the list of scale values into single elements.
    //

    sdata2 = string_whitespace_simplify(sdata);

    // Split the list into tokens. Scale values are separated by blank spaces!

    vsdata = string_tokenize(sdata2,' ');

    //
    // Add each single scale value to the scale list. Note that a conversion
    // from string to double is needed, and that this conversion may
    // FAIL.
    //

    for (i = 0; i < (int)vsdata.size(); i++)
    {
      status = string_convert_to_double(vsdata[i],ddata);

      if (status)
      {
        if (ddata > 0)
        {
          scale->add_scale(ddata);
        }
        else
        {
          // Null or negative scale factor found.Report this
          // and LEAVE IMMEDIATELY the conversion loop.
          //

          error_description.clear();

          error_tag = "<nav_metadata_file>" + tag_parsed + "<s>";
          error_description.push_back("Non positive value found for " + tag_parsed + " scale factor: " + vsdata[i]);
          error_description.push_back("Offending " + tag_parsed + "'s lineage identifier: " + lineage_id);

          error_message = build_message(2, filename, error_tag, error_description);

          result = 2;

          break;
        }
      }
      else
      {
        //
        // Error detected while converting a string to double form. Report this
        // and LEAVE IMMEDIATELY the conversion loop.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file>" + tag_parsed + "<s>";
        error_description.push_back("Invalid value found for " + tag_parsed + " scale factor: " + vsdata[i]);
        error_description.push_back("Offending " + tag_parsed + "'s lineage identifier: " + lineage_id);

        error_message = build_message(2, filename, error_tag, error_description);

        result = 2;

        break;
      }
    }
  }
  return result;
}

int
gp_utils::  
parse_sub_m_list
(const string&   filename,
 adp_DOMparser*  dp,
 string&         lineage_id,
 gp_sub_m_list*  smlist,
 vector<string>& error_messages)
{
  int result;

  {
    int            dimension;
    string         dimension_txt;
    vector<string> error_description;
    string         error_tag;
    string         identifier;
    int            n;
    int            n_smitems;
    string         n_smitems_txt;
    string         n_txt;
    int            smitem_index;
    string         smitem_type;
    string         tmp_msg;
    string         tmp_txt;
    stringstream   tostring_converter;

    // Check our preconditions.

    if (filename.empty())   return 1;
    if (dp == NULL)         return 1;
    if (lineage_id.empty()) return 1;
    if (smlist == NULL)     return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    // Get the actual number of item elements in the sub-m_list element.

    n_smitems = dp->_n_of_repeated_sublabels("item");
    tostring_converter.str("");
    tostring_converter << n_smitems;
    n_smitems_txt = tostring_converter.str();

    // Get the declared dimension of the sub-m_list.

    dp->goto_label_relative("dimension");
    dimension = dp->_label_value_int();
    dimension_txt = dp->_label_value_string();

    smlist->set_dim(dimension);

    dp->go_back(); // Back to sub-m_list.

    //
    // Check that the actual and declared dimension match.
    // Report an error if they don't match, but keep working to
    // detect as many errors as possible.
    //

    if (dimension != n_smitems)
    {
      error_description.clear();

      error_tag = "<nav_metadata_file><m_spec><sub-m_list><dimension>";

      error_description.push_back("The value of the <dimension> element ("
                                   + dimension_txt
                                   + ") does not match with actual number of <item> elements (" 
                                   + n_smitems_txt 
                                   + ").");
      error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
      tmp_msg = build_message(2, filename, error_tag, error_description);

      error_messages.push_back(tmp_msg);

      result = 2;
    }

    // Parse the n_smitems item elements that have been found.

    for (smitem_index = 1; smitem_index <= n_smitems; smitem_index++)
    {

      // Go to the item at position smitem_index.

      dp->goto_label_relative("item",smitem_index);

      // Get its "n" attribute in both numeric and textual forms.

      n     = dp->_label_attribute_int("n");
      n_txt = dp->_label_attribute_string("n");

      //
      // Check that the value of the "n" attribute is within the
      // valid range [1..dimension].
      //

      if ((n < 1) || (n > dimension))
      {
        //
        // The value of the "n" attribute is not in the valid range.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><m_spec><sub-m_list><item>";

        error_description.push_back("The value of the n attribute ("
                                     + n_txt
                                     + ") does not fall within the valid range of values [1.." 
                                     + dimension_txt 
                                     + "].");
        error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }

      // Check that the value of the "n" attribute is not repeated.

      if (smlist->exists_item(n))
      {
        //
        // Repeated "n" attribute value.
        // Repor the error, but keep working trying to find as many
        // errors as possible.
        //

        error_description.clear();

        error_tag = "<nav_metadata_file><m_spec><sub-m_list><item>";

        error_description.push_back("Repeated value for the item n attribute (" + n_txt + ").");
        error_description.push_back("Offending <m_spec> lineage identifier: " + lineage_id);
        tmp_msg = build_message(2, filename, error_tag, error_description);

        error_messages.push_back(tmp_msg);

        result = 2;
      }


      // Let's get the identifier of the item.

      dp->goto_label_relative("id");

      identifier = dp->_label_value_string();

      dp->go_back(); // Back to item (smitem_index).


      dp->go_back(); // Back to sub-m_list.

      //
      // Add the new item (couple made of the n attribute plus the
      // identifier) to the sub-m_list.
      // We won't do that if the current item has a repeated n
      // attribute.
      //

      if (!smlist->exists_item(n)) smlist->set_item_id(n, identifier);
    }

    return result;
  }
}

int
gp_utils::  
parse_timespec
(const string&     filename,
 adp_DOMparser*    dp,
 gp_units_manager* units_manager,
 const string&     tag_parsed,
 const string&     lineage_id,
 gp_timespec*      tspec,
 vector<string>&   error_messages)
{
  int result;

  {
    vector<string> error_description;
    vector<string> error_list;
    string         error_message;
    string         error_tag;
    gp_ref*        ref;
    int            retcode;
    bool           status;
    gp_unit*       units;

    // Check our preconditions.

    if (filename.empty())           return 1;
    if (dp == NULL)                 return 1;
    if (units_manager == NULL)      return 1;
    if (!units_manager->is_valid()) return 1;
    if (tag_parsed.empty())         return 1;
    if (lineage_id.empty())         return 1;
    if (tspec == NULL)              return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    //
    // We'll parse the t_spec element.
    //

    // Parse the ref element.

    dp->goto_label_relative("ref");
    ref = tspec->get_ref_ptr();

    status = parse_ref(dp, ref);

    if (!status)
    {
      //
      // Errors parsing the ref element. We'll report the error
      // and keep going to detect as many problems as possible.
      //

      error_description.clear();

      error_tag = "<nav_metadata_file>" + tag_parsed + "<time_spec><ref>";
      error_description.push_back("The syntax of the list of codes in the <coor_ref_frame_VC> or ");
      error_description.push_back("<ref_frame_VC> or <coor_system_VC> is not correct.");
      error_description.push_back("Lineage identifier of the offending " + tag_parsed + " element: " + lineage_id);

      error_message = build_message(2, filename, error_tag, error_description);
      error_messages.push_back(error_message);

      result = 2;
    }

    //
    // Nonetheless, check that the number of elements in the ref object
    // is the correct one.
    //

    status = validate_ref(ref);

    if (!status)
    {
      //
      // Problems with the number of codes found. We'll report the error
      // and keep going to detect as many problems as possible.
      //

        error_description.clear();

        error_tag = "<nav_metadata_file>" + tag_parsed + "<time_spec><ref>";
        error_description.push_back("Invalid number of elements in <ref> specification.");
        error_description.push_back("The number of codes in reference frame and coordinate");
        error_description.push_back("system lists do not match.");
        error_description.push_back("Lineage identifier of the offending " + tag_parsed + " element: " + lineage_id);

        error_message = build_message(2, filename, error_tag, error_description);
        error_messages.push_back(error_message);

        result = 2;
    }

    dp->go_back(); // Back to t_spec.

    // Parse the units element

    dp->goto_label_relative("units");
    units = tspec->get_unit_ptr();

    //
    // When using a schema, the preconditions of the next method are always
    // guaranteed, so there is only two reasons to fail: the number of
    // units parsed is not exactly one or the unit itself is not correct.
    //

    retcode = parse_units(dp, units_manager, 1, units);

    if (retcode == 2)
    {

      //
      // Invalid number of units found. We'll keep going, however, to detect
      // as many errors as possible.
      //

      error_description.clear();

      error_tag = "<nav_metadata_file>" + tag_parsed + "<time_spec><units>";

      error_description.push_back("Multiple unit specification found.");
      error_description.push_back("Only a single unit specification is allowed.");
      error_description.push_back("Lineage identifier of the offending " + tag_parsed + " element: " + lineage_id);
      error_message = build_message(2, filename, error_tag, error_description);

      error_messages.push_back(error_message);

      result = 2;
    }
    else if (retcode == 3)
    {
      // Invalid unit found.

      error_description.clear();

      error_tag = "<nav_metadata_file>" + tag_parsed + "<time_spec><units>";

      error_description.push_back("Invalid syntax for unit specification found");
      error_description.push_back("Lineage identifier of the offending " + tag_parsed + " element: " + lineage_id);
      error_message = build_message(2, filename, error_tag, error_description);

      error_messages.push_back(error_message);

      result = 2;
    }

    dp->go_back(); // Back to t_spec.

  }

  return result;
}

int
gp_utils::    
parse_tspec
(const string&     filename,
 adp_DOMparser*    dp,
 gp_units_manager* units_manager,
 const string&     tag_parsed,
 const string&     lineage_id,
 gp_tspec*         tspec,
 vector<string>&   error_messages)
{
  int result;

  {
    int            dimension;
    vector<string> error_description;
    string         error_message;
    string         error_tag;
    gp_ref*        ref;
    int            retcode;
    bool           status;
    gp_unit*       units;

    // Check our preconditions.

    if (filename.empty())           return 1;
    if (dp == NULL)                 return 1;
    if (units_manager == NULL)      return 1;
    if (!units_manager->is_valid()) return 1;
    if (tag_parsed.empty())         return 1;
    if (lineage_id.empty())         return 1;
    if (tspec == NULL)              return 1;

    // Initialize.

    result = 0;
    error_messages.clear();

    // Get the dimension element.

    dp->goto_label_relative("dimension");

    dimension = dp->_label_value_int();
    tspec->set_dimension(dimension);

    dp->go_back(); // Back to t_spec.

    // Parse the ref element.

    dp->goto_label_relative("ref");

    ref = tspec->get_ref_ptr();
    status = parse_ref(dp, ref);
    if (!status)
    {
      //
      // Errors parsing the ref element. We'll report the error
      // and keep going to detect as many problems as possible.
      //

      error_description.clear();

      error_tag = "<nav_metadata_file>" + tag_parsed + "<t_spec><ref>";
      error_description.push_back("The syntax of the list of codes in the <coor_ref_frame_VC> or ");
      error_description.push_back("<ref_frame_VC> or <coor_system_VC> is not correct.");
      error_description.push_back("Lineage identifier of the offending " + tag_parsed + " element: " + lineage_id);

      error_message = build_message(2, filename, error_tag, error_description);
      error_messages.push_back(error_message);

      result = 2;
    }

    // Validate the ref object just parsed.

    status = validate_ref(ref);

    if (!status)
    {
      //
      // Problems with the number of codes found. We'll report the error
      // and keep going to detect as many problems as possible.
      //

        error_description.clear();

        error_tag = "<nav_metadata_file>" + tag_parsed + "<t_spec><ref>";
        error_description.push_back("Invalid number of elements in <ref> specification.");
        error_description.push_back("The number of codes in reference frame and coordinate");
        error_description.push_back("system lists do not match.");
        error_description.push_back("Lineage identifier of the offending " + tag_parsed + " element: " + lineage_id);

        error_message = build_message(2, filename, error_tag, error_description);
        error_messages.push_back(error_message);

        result = 2;
    }


    dp->go_back(); // Back to t_spec.

  // Parse the units element.

    dp->goto_label_relative("units");

    units = tspec->get_unit_ptr();

    //
    // When using a schema, the preconditions of the next method are always
    // guaranteed, so there is only two reasons to fail: the number of
    // units parsed is not exactly one or the unit itself is not correct.
    //

    retcode = parse_units(dp, units_manager, dimension, units);

    if (retcode == 2)
    {
      //
      // Problems with the number of codes found. We'll report the error
      // and keep going to detect as many problems as possible.
      //

      error_description.clear();

      error_tag = "<nav_metadata_file>" + tag_parsed + "<t_spec><units>";
      error_description.push_back("Invalid number of units found.");
      error_description.push_back("It must match the value stated in the <dimension> element.");
      error_description.push_back("Lineage identifier of the offending " + tag_parsed + " element: " + lineage_id);

      error_message = build_message(2, filename, error_tag, error_description);
      error_messages.push_back(error_message);

      result = 2;
    }
    else if (retcode == 3)
    {
      // Invalid unit found.

      error_description.clear();

      error_tag = "<nav_metadata_file>" + tag_parsed + "<t_spec><units>";
      error_description.push_back("Invalid syntax for one or more unit specifications");
      error_description.push_back("Lineage identifier of the offending " + tag_parsed + " element: " + lineage_id);
      error_message = build_message(2, filename, error_tag, error_description);

      error_messages.push_back(error_message);

      result = 2;
    }

    dp->go_back(); // Back to t_spec.


    // That's all.

    return result;
    
  }
}

int
gp_utils::
parse_units
(adp_DOMparser*    dp,
 gp_units_manager* units_manager,
 int               dimension,
 gp_unit*          units)
{

  {
    int            i;
    int            total_units;
    int            retcode;
    string         sdata;
    string         sdata2;
    vector<string> vsdata;

    // Check that our parameters are non-null.

    if (dp == NULL)                 return 1;
    if (units_manager == NULL)      return 1;
    if (!units_manager->is_valid()) return 1;
    if (dimension <= 0)             return 1;
    if (units == NULL)              return 1;

    // Get the list of units.

    sdata = dp->_label_value_string();

    // Remove all whitespace

    sdata2 = string_whitespace_remove_all(sdata);

    // Split the list into tokens. Units are separated by commas.

    vsdata = string_tokenize(sdata2,',');

    //
    // Check that the number of units present matches with those
    // stated by our input parameter dimension.
    //

    total_units = (int)vsdata.size();
    if (total_units != dimension) return 2;

    //
    // Validate each single unit. We'll use the units_manager object, which
    // relies on the UDUNITS2 library, to do it. BUT!!! But we'll check
    // first if the text contains a "-", which means "dimensionless"....
    //

    for (i = 0; i < total_units; i++)
    {
      if (vsdata[i] != "-")
      {
        retcode = units_manager->validate_unit(vsdata[i]);
        if (retcode != 0) return 3;
      }
    }

    // All the units parsed are syntactically correct. Keep going!

    // Set the size of the list of units.

    units->set_dim(total_units);

    // Add each single unit to the list of units codes.

    for (i = 0; i < total_units; i++) units->set_unit(i,vsdata[i]);
  }

  return 0;
}

bool
gp_utils::
string_convert_to_double
(string& numberAsString, double& result)
{
  bool status;

  if (numberAsString.empty()) return false; // Precondition.

  {
    status = true;

    std::stringstream stream(numberAsString);
    stream >> result;
    if (stream.fail()) status = false;
  }

  return status;
}

bool
gp_utils::
string_convert_to_int
(string& numberAsString, int& result)
{
  bool   status;
  double dtest;
  int    itest;

  if (numberAsString.empty()) return false; // Precondition.

  {
    status = true;

    std::stringstream stream(numberAsString);
    stream >> dtest;
    if (stream.fail()) status = false;

    itest = (int)dtest;

    if ((double)itest != dtest) return false; // Decimals found. Not a double.

    result = itest;
    return true;
  }

  return status;
}

vector<string>&
gp_utils::
string_tokenize
(string& source,
 const char separator)
{
  vector<string>* result;
  {
    stringstream* ss;

    ss = new stringstream(source);
    result = new vector<string>;

    while(ss->good())
    {
      string substr;
      getline(*ss, substr, separator);
      result->push_back(substr);
    }

    //
    // Remove garbage. DON'T delete the result; we return it as
    // a reference!!!
    //

    delete ss;
  }

  return *result;
}

string&
gp_utils::
string_whitespace_remove_all
(string& source)
{
  string* result;

  {
    char*   buffer1;
    char*   buffer2;
    int     first;
    int     i;
    int     j;
    int     last;
    int     source_size;

    // Length of the source string.

    source_size = (int)source.length();

    // If the source string is empty, we return the same.

    if (source_size == 0)
    {
      result = new string();
      return *result;
    }

    // Get a char* copy of the string.

    buffer1 = new char[source_size + 1];
    strcpy (buffer1, source.c_str());


    // Let's find the limits of the leading and trailing whitespace sequences.

    for (first = 0; first < source_size; first++)
    {
      if (!is_whitespace(buffer1[first])) break;
    }

    for (last = source_size - 1; last >= 0; last--)
    {
      if (!is_whitespace(buffer1[last])) break;
    }

    //
    // If first is greater than last, it means that the source string
    // contains only whitespace. We'll, then, return an empy string
    // as result.
    //

    if (first > last)
    {
      delete buffer1;

      result = new string();
      return *result;
    }

    //
    // Just to save some work: if first and last are EQUAL, it means that
    // the source string contained only one non-whitespace character, so there's
    // no need to look for whitespace in between. In such case, we'll return a
    // string that contains the single character found.
    //

    if (first == last)
    {
      result = new string(&(buffer1[first]), 1);
      delete buffer1;
      return *result;
    }

    //
    // Here we know that first and last point, respectively, to the first
    // and last positions in buffer1 where non-whitespace characters are
    // stored.
    //
    // The work to do now is to remove all whitespace that might exist
    // between first and last.
    //

    buffer2 = new char[last-first+2]; // We provide with one extra char for the NULL.

    j = 0;

    for (i = first; i <= last; i++)
    {
      if (!is_whitespace(buffer1[i]))
      {
        buffer2[j] = buffer1[i];
        j++;
      }
    }

    buffer2[j] = 0; // NULL terminated char*.

    //
    // We've removed the whitespace characters. We may return a copy of our
    // buffer2 in string form. Don't forget to delete the auxiliary char*
    // buffers!
    //

    result = new string(buffer2);

    delete buffer1;
    delete buffer2;

    return *result;
  }
}

string&
gp_utils::
string_whitespace_simplify
(string& source)
{
  string* result;

  {
    char*   buffer1;
    char*   buffer2;
    int     first;
    int     i;
    int     j;
    int     last;
    int     source_size;
    bool    white_char_copied;

    // Length of the source string.

    source_size = (int)source.length();

    // If the source string is empty, we return the same.

    if (source_size == 0)
    {
      result = new string();
      return *result;
    }

    // Get a char* copy of the string.

    buffer1 = new char[source_size + 1];
    strcpy (buffer1, source.c_str());


    // Let's find the limits of the leading and trailing whitespace sequences.

    for (first = 0; first < source_size; first++)
    {
      if (!is_whitespace(buffer1[first])) break;
    }

    for (last = source_size - 1; last >= 0; last--)
    {
      if (!is_whitespace(buffer1[last])) break;
    }

    //
    // If first is greater than last, it means that the source string
    // contains only whitespace. We'll, then, return an empy string
    // as result.
    //

    if (first > last)
    {
      delete buffer1;

      result = new string();
      return *result;
    }

    //
    // Just to save some work: if first and last are EQUAL, it means that
    // the source string contained only one non-whitespace character, so there's
    // no need to look for whitespace in between. In such case, we'll return a
    // string that contains the single character found.
    //

    if (first == last)
    {
      result = new string(&(buffer1[first]), 1);
      delete buffer1;
      return *result;
    }

    //
    // Here we know that first and last point, respectively, to the first
    // and last positions in buffer1 where non-whitespace characters are
    // stored.
    //
    // The work to do now is to collapse whitespace; that is, for every
    // sequence of whitespace chars found, we'll copy a single blank char to the
    // output.
    //

    buffer2 = new char[last-first+2]; // We provide with one extra char for the NULL.

    white_char_copied = false;
    j = 0;

    for (i = first; i <= last; i++)
    {
      if (is_whitespace(buffer1[i]))
      {
        if (!white_char_copied)
        {
          white_char_copied = true;
          buffer2[j] = ' ';
          j++;
        }
      }
      else
      {
        buffer2[j] = buffer1[i];
        j++;

        white_char_copied = false;
      }
    }

    buffer2[j] = 0; // NULL terminated char*.

    //
    // We've purged the whitespace characters. We may return a copy of our
    // buffer2 in string form. Don't forget to delete the auxiliary char*
    // buffers!
    //

    result = new string(buffer2);

    delete buffer1;
    delete buffer2;

    return *result;
  }
}

bool
gp_utils::
validate_ispec_clist_item_type
(string&      the_item,
 bool&        is_scalar,
 vector<int>& dimensions)
{
  {
    int            i;
    int            idata;
    string         list_of_values;
    size_t         list_length;
    string         matrix;
    string         no_whitespace;
    size_t         pos_par1;
    size_t         pos_par2;
    bool           status;
    vector<string> values;

    // Let's make some assumptions...

    is_scalar = true;

    // First of all, remove all whitespaces to simplify the parsing process.

    no_whitespace = string_whitespace_remove_all(the_item);

    // Look for an opening parenthesis.

    pos_par1 = no_whitespace.find("(");
    if (pos_par1 == std::string::npos)
    {
      //
      // We haven't found the opening parenthesis. Then, the item
      // must contain the word "scalar" (either in lower or uppercase,
      // but not mixed case).
      //

      if ((no_whitespace == "scalar") || (no_whitespace == "SCALAR")) return true;

      return false;
    }

    //
    // At this point we should be facing a matrix specification. Then:
    //
    // - We must find a closing parenthesis,
    // - The word before the opening one must be MATRIX or matrix (no
    //   mixed case is allowed).
    // - There must be a comma-separated list of integer values between
    //   the two parenthesis.
    //

    is_scalar = false;

    //
    // Well, do we have a closing parenthesis??? If not, we've found a
    // syntax error.
    //

    pos_par2 = no_whitespace.rfind(")");
    if (pos_par2 == std::string::npos) return false;

    //
    // The parenthesis must be in the right positions!
    // (opening one on the left, the closing one, on the right).
    //

    if (pos_par2 < pos_par1) return false;

    // No text between the parentheses.

    if (pos_par2 == (pos_par1 + 1)) return false;

    //
    // We've got a couple of matching parenthesis. Let's see if we have
    // the word MATRIX or matrix. If not, go away sayinig we've got
    // problems.
    //

    matrix = no_whitespace.substr(0, pos_par1);

    if ((matrix != "MATRIX") & (matrix != "matrix")) return false;

    //
    // Well, the characters between the two parentheses must be a list
    // of positive integers separated by commmas. Let's extract this
    // chunk from the string.
    //

    list_length = (pos_par2 - pos_par1) - 1;
    list_of_values = no_whitespace.substr(pos_par1 + 1, list_length);

    //
    // Let's split the list in tokens, killing the commas separating the
    // integer values. The values array will hold a single value in each
    // of its components.
    //

    values = string_tokenize(list_of_values, ',');

    //
    // Check here that the list of values is not empty (that is,
    // there were only whitespace or commas between the two
    // facing parentheses).
    //

    if (values.size() == 0) return false;

    //
    // Check that each item is a positive integer value > 1.
    //

    dimensions.clear();

    for (i = 0; i < (int)values.size(); i++)
    {
      status = string_convert_to_int(values[i], idata);

      if (!status)
      {
        dimensions.clear();
        return false; // Not an integer value.
      }

      if (idata <= 1)
      {
        dimensions.clear();
        return false;
      }

      dimensions.push_back(idata);
    }

    //
    // Well, all tests have been passed. We've got a correct
    // item...
    //

    return true;
  }
}

bool
gp_utils::  
validate_ref
(gp_ref* ref)
{

  {
    int    total_cs;
    int    total_rf;

    //
    // We are only interested in those refs that include a coordinate system
    // plus a reference frame lists.
    //

    if (ref->get_type() == gp_ref::GP_REF_TYPE_CS_RF)
    {
      //
      // The ref element is specified using a coordinate system plus a reference frame.
      // Check the dimensions of both lists: these must match!
      //

      total_cs = ref->get_dim_CS();
      total_rf = ref->get_dim_RF();

      if (total_cs != total_rf) return false;
    }
    return true;
  }
}
