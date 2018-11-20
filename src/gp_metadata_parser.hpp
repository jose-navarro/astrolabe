/** \file gp_metadata_parser.hpp
  \brief Parser for ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_METADATA_PARSER_HPP_
#define _GP_METADATA_PARSER_HPP_

#include "astrolabe_exceptions.hpp"
#include "adp_DOMparser.hpp"

#include "gp_metadata.hpp"
#include "gp_utils.hpp"
#include "gp_units_manager.hpp"

#include <vector>
#include <string>
#include <set>

#include <algorithm>

using namespace std;

/// \brief Parser for ASTROLABE metadata files.
/**

  Class gp_metadata_parser offers a very simple API to parse
  (read and load in memory as an object) ASTROLABE metadata files.

  The steps to perform such parsing are the following:

  - Instantiate a gp_metadata_parser object.
  - Set its parameters (using either set_parser_parameters() or
    set_parser_parameters_developer()).
  - Start the parsing process itself using method parse() and
  - Check for any warnings or errors detected in the previous process
    using warnings_dim(), errors_dim(), warnings_get() and errors_get().

  The parsing process is considered successful if no errors
  have been detected (that is, when errors_dim() returns a zero).
  However, a successful parsing might return warnings, that is,
  warnings_dim() might return a value greater than zero.

  Warnings by themselves do not invalidate the parsing process
  These are only messages indicating that some combinations of
  values are nonsense and that some default action has been taken.

  After a successful parsing, the data loaded is stored in a
  gp_metadata object, which is one of the parameters to
  set when configuring the parser. To retrieve data stored
  in the gp_metadata object, the interface (API) of this object
  must be used.

  As stated above, there are two different ways to set a
  parser:

    - A "production" mode, where an XML schema defining the
      syntax of ASTROLABE metadata files is rigorously defined and
    - A "development" mode, where such schema is not required.

  The two methods included in this class' interface to set
  up the parser are set_parser_parameters() ("production"
  mode) and set_parser_parameters_developer() ("developer"
  mode).

  The developer mode should be used only when modifying this
  class, that is, when changes are introduced to adapt
  the parser to any changes included in the ASTROLABE metadata
  files. Why?

  Method parse() has been developed assuming that many
  tests about the syntactical correctness of the
  input file have been made through the use of an
  XML schema. This is so because, in this way, the
  final parsing code is much simpler that if every
  possible error (not controlled by an schema validation)
  should be tested by the parser itself.

  Therefore, when no XML schema is present, a syntactically
  incorrect input metadata file may lead to undesired,
  uncontrolled errors.

  This means that the developer set up should be used
  only by developers introducing new changes to the
  parser code. Once that these changes are finished,
  the corresponding XML schema must be also updated and
  used afterwards in a production-only environment.

  Below, a full working example showing how to use
  this class is included. Note that the main program
  in this example receives all data necessary to run
  the parser from the command line. Depending on
  the number of parameters received, it automatically
  sets up the parser in either developer or production
  mode.

  The example below also provide with code to show how to
  retrieve the data loaded in the gp_metadata_osi object
  once the parsing is done. For more information about
  this process, see the documentation of class
  gp_metadata.

\code
#include "gp_metadata_parser.hpp"
#include "gp_metadata.hpp"

#include <iostream>
using namespace std;

void print_lineage(int indent, lineage_data*       lineage);
void print_clist_item_type(int indent, gp_clist_item_type* gci_type);
void print_ref(int indent, gp_ref*             ref);
void print_tspec(int indent, gp_tspec*           tspec);

int main
(int   argC,
  char* argV[])
{
  {

    // The parser.

    gp_metadata_parser  metadata_parser;

    //
    // The metadata object that will hold the values parsed from the
    // input file.
    //

    gp_metadata         metadata;

    // Input metadata, UDUNITS2 units database and XML schema file names.

    vector<string>      metadata_files;
    string              udunits_units_database;
    string              xml_schema;

    //
    // Auxiliary pointers / objects used to retrieve the metadata
    // once loaded.
    //

    bool                baux;
    gp_clist*           clist;
    gp_clist_item       citem;
    gp_clist_item_type* citem_type;
    gp_covariance*      cov;
    double              daux;
    gp_ilist*           ilist;
    gp_ispec            ispec;
    lineage_data*       lineage;
    gp_llist*           llist;
    gp_lspec            lspec;
    gp_mspec            mspec;
    gp_plist_item       pitem;
    gp_plist*           plist;
    gp_pspec            pspec;
    gp_ref*             ref;
    string              saux;
    string              saux2;
    gp_scale*           scale;
    gp_sub_m_list*      submlist;
    gp_tspec*           tspec;
    gp_timespec*        timespec;
    gp_unit*            units;

    // Error and warning handling variables.

    int                 error_total;
    int                 error_index;
    string              error_message;
    int                 warning_total;
    int                 warning_index;
    string              warning_message;

    // Auxiliar variables used in loops.

    int                 i;
    int                 j;
    int                 k;
    int                 list_dim, list_dim2;
    int                 total_items;

    //
    // Test the number of parameters. We must receive an input metadata file!
    // argV[0] is the name of the program and argV[1] is the name of the
    // input file. argV[2] is the full path to the UDUNITS2 file defining its
    // units and, finally, we may also receive the name of a schema in argV[3];
    //

    if (argC < 3)
    {
      cout << "Usage: test_metadata_parser metadata_file UDUNITS2_units_database_file [schema_file]" << endl;
      return 1;
    }

    //
    // Depending on the number of parameters, we'll have just a metadata file
    // or, additionally, an schema to validate the syntax of the former.
    //
    // Set up the parser depending on that.
    //

    metadata_files.push_back(argV[1]);
    udunits_units_database = argV[2];

    if (argC == 3)
    {
      // Developer's setup (NO schema to help validating the input file)


      metadata_parser.set_parser_parameters_developer(metadata_files,
        udunits_units_database,
        &metadata);
    }
    else
    {
      // Production set up, with a schema included.

      xml_schema = argV[3];
      metadata_parser.set_parser_parameters(metadata_files,
        udunits_units_database,
        xml_schema,
        &metadata);
    }

    // Parse the input file.

    metadata_parser.parse();

    // Check if the parsing process produced warnings or errors.

    error_total = metadata_parser.errors_dim();

    if (error_total > 0)
    {
      cout << "Parsing finished with errors!" << endl << endl;

      for (error_index = 0; error_index < error_total; error_index++)
      {
        error_message = metadata_parser.errors_get(error_index);
        cout << error_message << endl;
      }
    }
    else
    {
      cout << "Parsing finished with no errors!" << endl;
    }

    warning_total = metadata_parser.warnings_dim();

    if (warning_total > 0)
    {
      cout << "Parsing finished with warnings!" << endl << endl;

      for (warning_index = 0; warning_index < warning_total; warning_index++)
      {
        warning_message = metadata_parser.warnings_get(warning_index);
        cout << warning_message << endl;
      }
    }
    else
    {
      cout << "Parsing finished with no warnings!" << endl;
    }

    //
    // If errors have been detected, stop the process here.
    //
    // Otherwise, dump the metadata to screen. This will show how to
    // access the information stored in the metadata object.
    //

    if (error_total > 0) return 0;

    // -----------------------------
    // 1 - Dump the p_spec metadata.
    // -----------------------------

    total_items = metadata.get_dim_pspec();

    for (i = 0; i < total_items; i++)
    {
      cout << "p_spec #" << i + 1 << endl;

      // Retrieve the ith pspec in the metadata object.

      pspec = metadata.get_pspec(i);

      // Retrieve the lineage and all its elements.

      lineage = pspec.get_lineage_ptr();
      print_lineage(2, lineage);

      // Retrieve its type.

      saux = pspec.get_type();
      cout << "  type: " << saux << endl;

      // Retrieve its toolbox (optional).

      saux = pspec.get_toolbox();
      if (!saux.empty()) cout << "  toolbox: " << saux << endl;

      // Retrieve its dimension.

      j = pspec.get_dimension();
      cout << "  dimension: " << j << endl;

      // Retrieve a pointer to its ref. Print it.

      ref = pspec.get_ref_ptr();
      print_ref(2, ref);

      //
      // Retrieve a pointer to its time_spec. Print it.
      // Note that it contains a ref plus a units object!
      //

      timespec = pspec.get_timespec_ptr();

      cout << "  time spec" << endl;

      ref = timespec->get_ref_ptr();
      print_ref(4, ref);

      units = timespec->get_unit_ptr();
      list_dim = units->get_dim();

      cout << "    units:";

      for (j = 0; j < list_dim; j++)
      {
        saux = units->get_unit(j);
        cout << " " << saux;
      }
      cout << endl;

      // Retrieve the units (if any!)

      units = pspec.get_unit_ptr();
      list_dim = units->get_dim();

      if (list_dim > 0)
      {
        cout << "  units:";

        for (j = 0; j < list_dim; j++)
        {
          saux = units->get_unit(j);
          cout << " " << saux;
        }
        cout << endl;
      }

      // Retrieve the covariance (if any!)

      cov = pspec.get_covariance_ptr();
      list_dim = cov->get_dim();

      if (list_dim > 0)
      {
        cout << "  covariance matrix:";

        for (j = 0; j < list_dim; j++)
        {
          daux = cov->get_value(j);
          cout << " " << daux;
        }
        cout << endl;
      }

      // Retrieve the scale factors (if any!)

      scale = pspec.get_scale_ptr();
      list_dim = scale->get_dim();

      if (list_dim > 0)
      {
        cout << "  scale factors:";

        for (j = 0; j < list_dim; j++)
        {
          daux = scale->get_value(j);
          cout << " " << daux;
        }
        cout << endl;
      }

      // Retrieve the t_spec (if any).

      tspec = pspec.get_tspec_ptr();
      if (tspec->get_dimension() > 0) print_tspec(2, tspec);

    }

    // -----------------------------
    // 2 - Dump the i_spec metadata.
    // -----------------------------

    total_items = metadata.get_dim_ispec();

    for (i = 0; i < total_items; i++)
    {
      cout << "i_spec #" << i + 1 << endl;

      // Retrieve the ith ispec in the metadata object.

      ispec = metadata.get_ispec(i);

      // Retrieve the lineage and all its elements.

      lineage = ispec.get_lineage_ptr();
      print_lineage(2, lineage);

      // Retrieve its type.

      saux = ispec.get_type();
      cout << "  type: " << saux << endl;

      // Retrieve its toolbox (optional).

      saux = ispec.get_toolbox();
      if (!saux.empty()) cout << "  toolbox: " << saux << endl;

      //
      // Retrieve a pointer to its time_spec. Print it.
      // Note that it contains a ref plus a units object!
      //

      timespec = pspec.get_timespec_ptr();

      cout << "  time spec" << endl;

      ref = timespec->get_ref_ptr();
      print_ref(4, ref);

      units = timespec->get_unit_ptr();
      list_dim = units->get_dim();

      cout << "    units:";

      for (j = 0; j < list_dim; j++)
      {
        saux = units->get_unit(j);
        cout << " " << saux;
      }
      cout << endl;

      // Retrieve the list of constants.

      clist = ispec.get_clist_ptr();
      cout << "  c_list " << endl;

      list_dim2 = clist->get_dim();

      cout << "    dimension: " << list_dim2 << endl;

      for (k = 1; k <= list_dim2; k++)
      {
        citem = clist->get_item(k);
        cout << "    item # " << k << endl;

        // item type.

        citem_type = citem.get_type_ptr();
        print_clist_item_type(6, citem_type);

        // item ref.

        // Retrieve a pointer to its ref. Print it.

        ref = citem.get_ref_ptr();
        print_ref(6, ref);

        // Retrieve the units (if any!)

        units = citem.get_unit_ptr();
        list_dim = units->get_dim();

        if (list_dim > 0)
        {
          cout << "      units:";

          for (j = 0; j < list_dim; j++)
          {
            saux = units->get_unit(j);
            cout << " " << saux;
          }
          cout << endl;
        }
      }

      // Retrieve the t_spec (if any).

      tspec = ispec.get_tspec_ptr();
      if (tspec->get_dimension() > 0) print_tspec(2, tspec);

    }

    // -----------------------------
    // 3 - Dump the l_spec metadata.
    // -----------------------------

    total_items = metadata.get_dim_lspec();

    for (i = 0; i < total_items; i++)
    {
      cout << "l_spec #" << i + 1 << endl;

      // Retrieve the ith lspec in the metadata object.

      lspec = metadata.get_lspec(i);

      // Retrieve the lineage and all its elements.

      lineage = lspec.get_lineage_ptr();
      print_lineage(2, lineage);

      // Retrieve its type.

      saux = lspec.get_type();
      cout << "  type: " << saux << endl;

      // Retrieve its toolbox (optional).

      saux = lspec.get_toolbox();
      if (!saux.empty()) cout << "  toolbox: " << saux << endl;

      // Retrieve its dimension.

      j = lspec.get_dimension();
      cout << "  dimension: " << j << endl;

      // Retrieve a pointer to its ref. Print it.

      ref = lspec.get_ref_ptr();
      print_ref(2, ref);

      //
      // Retrieve a pointer to its time_spec. Print it.
      // Note that it contains a ref plus a units object!
      //

      timespec = pspec.get_timespec_ptr();

      cout << "  time spec" << endl;

      ref = timespec->get_ref_ptr();
      print_ref(4, ref);

      units = timespec->get_unit_ptr();
      list_dim = units->get_dim();

      cout << "    units:";

      for (j = 0; j < list_dim; j++)
      {
        saux = units->get_unit(j);
        cout << " " << saux;
      }
      cout << endl;

      // Retrieve the units (if any!)

      units = lspec.get_unit_ptr();
      list_dim = units->get_dim();

      if (list_dim > 0)
      {
        cout << "  units:";

        for (j = 0; j < list_dim; j++)
        {
          saux = units->get_unit(j);
          cout << " " << saux;
        }
        cout << endl;
      }

      // Retrieve the covariance (if any!)

      cov = lspec.get_covariance_ptr();
      list_dim = cov->get_dim();

      if (list_dim > 0)
      {
        cout << "  covariance matrix:";

        for (j = 0; j < list_dim; j++)
        {
          daux = cov->get_value(j);
          cout << " " << daux;
        }
        cout << endl;
      }

      // Retrieve the scale factors (if any!)

      scale = lspec.get_scale_ptr();
      list_dim = scale->get_dim();

      if (list_dim > 0)
      {
        cout << "  scale factors:";

        for (j = 0; j < list_dim; j++)
        {
          daux = scale->get_value(j);
          cout << " " << daux;
        }
        cout << endl;
      }

      // Retrieve the t_spec (if any).

      tspec = lspec.get_tspec_ptr();
      if (tspec->get_dimension() > 0) print_tspec(2, tspec);
    }

    // -----------------------------
    // 4 - Dump the m_spec metadata.
    // -----------------------------

    total_items = metadata.get_dim_mspec();

    for (i = 0; i < total_items; i++)
    {
      cout << "m_spec #" << i + 1 << endl;

      // Retrieve the ith mspec in the metadata object.

      mspec = metadata.get_mspec(i);

      // Retrieve the lineage and all its elements.

      lineage = mspec.get_lineage_ptr();
      print_lineage(2, lineage);

      // Retrieve its type.

      saux = mspec.get_type();
      cout << "  type: " << saux << endl;

      // Retrieve its toolbox (optional).

      saux = mspec.get_toolbox();
      if (!saux.empty()) cout << "  toolbox: " << saux << endl;

      // Is it dynamic?

      baux = mspec.get_dynamic();
      if (baux) cout << "  dynamic: YES" << endl;
      else      cout << "  dynamic: NO" << endl;

      // Process the l_list element.

      llist = mspec.get_llist_ptr();

      cout << "  clist" << endl;

      // Get its dimension.

      list_dim = llist->get_dim();
      cout << "    dimension: " << list_dim << endl;

      // Process all the item elements in the l_list.

      for (j = 1; j <= list_dim; j++)
      {
        cout << "    item #" << j << endl;
        saux = llist->get_item_id(j);
        cout << "      id: " << saux << endl;
      }

      // Process the p_list element.

      plist = mspec.get_plist_ptr();

      cout << "  plist" << endl;

      // Get its dimension.

      list_dim = plist->get_dim();
      cout << "    dimension: " << list_dim << endl;

      // Process all the item elements in the p_list.

      for (j = 1; j <= list_dim; j++)
      {
        pitem = plist->get_item(j);
        cout << "    item #" << j << endl;
        saux = pitem.get_id();
        cout << "      id  : " << saux << endl;
        saux = pitem.get_role();
        cout << "      role: " << saux << endl;
      }

      // Process the OPTIONAL i_list element.

      ilist = mspec.get_ilist_ptr();
      list_dim = ilist->get_dim();

      if (list_dim > 0)
      {
        cout << "  ilist" << endl;

        // Print (since we already have) its dimension.

        cout << "    dimension: " << list_dim << endl;

        // Process all the item elements in the l_list.

        for (j = 1; j <= list_dim; j++)
        {
          cout << "    item #" << j << endl;
          saux = ilist->get_item_id(j);
          cout << "      id: " << saux << endl;
        }
      }

      // Process the OPTIONAL sub-m_list element.

      submlist = mspec.get_sub_m_list_ptr();
      list_dim = submlist->get_dim();

      if (list_dim > 0)
      {
        cout << "  sub-m_list" << endl;

        // Print (since we already have) its dimension.

        cout << "    dimension: " << list_dim << endl;

        // Process all the item elements in the sub-m_list.

        for (j = 1; j <= list_dim; j++)
        {
          cout << "    item #" << j << endl;
          saux = submlist->get_item_id(j);
          cout << "      id: " << saux << endl;
        }
      }
    }

    // THAT'S ALL.

    cout << "Process finished" << endl;
  }
  return 0;
}

void
print_lineage
(int indent,
  lineage_data* lineage)
{
  {
    int    i;
    int    j;
    int    list_dim;
    string saux;
    string tab1, tab2, tab3;

    tab1 = "";
    for (i = 0; i < indent; i++) tab1 += " ";
    tab2 = tab1 + "  ";
    tab3 = tab2 + "  ";


    cout << tab1 << "lineage" << endl;

    saux = lineage->id_get();
    cout << tab2 << "id: " << saux << endl;

    saux = lineage->name_get();
    if (!saux.empty()) cout << tab2 << "name: " << saux << endl;

    saux = lineage->date_time_get();
    if (!saux.empty()) cout << tab2 << "date & time: " << saux << endl;

    list_dim = lineage->author_item_dim();
    if (list_dim > 0)
    {
      cout << tab2 << "author" << endl;
      for (j = 0; j < list_dim; j++)
      {
        saux = lineage->author_item_get(j);
        cout << tab3 << "item: " << saux << endl;
      }
    }

    saux = lineage->organization_get();
    if (!saux.empty()) cout << tab2 << "organization: " << saux << endl;

    saux = lineage->department_get();
    if (!saux.empty()) cout << tab2 << "department: " << saux << endl;

    saux = lineage->date_time_get();
    if (!saux.empty()) cout << tab2 << "date_time: " << saux << endl;

    list_dim = lineage->ref_document_item_dim();
    if (list_dim > 0)
    {
      cout << tab2 << "ref_document" << endl;
      for (j = 0; j < list_dim; j++)
      {
        saux = lineage->ref_document_item_get(j);
        cout << tab3 << "item: " << saux << endl;
      }
    }

    saux = lineage->project_get();
    if (!saux.empty()) cout << tab2 << "project: " << saux << endl;

    saux = lineage->task_get();
    if (!saux.empty()) cout << tab2 << "task: " << saux << endl;

    saux = lineage->remarks_get();
    if (!saux.empty()) cout << tab2 << "remarks: " << saux << endl;
  }
}

void
print_clist_item_type
(int indent,
  gp_clist_item_type* gci_type)
{
  {
    int    i;
    int    dimension;
    string tab1;
    int    total_dimensions;

    tab1 = "";
    for (i = 0; i < indent; i++) tab1 += " ";
    cout << tab1 << "type: ";

    //
    // Depending on the item type (scalar, matrix) we have,
    // we have to print it in this or that way.
    //

    if (gci_type->is_scalar())
    {
      cout << "scalar" << endl;
      return;
    }

    //
    // Well, here we have a matrix type. We've got to print
    // not only the keyword "matrix" but also the opening
    // and closing parenthesis as well as the dimensions of
    // such matrix.
    //

    cout << "matrix(";
    total_dimensions = gci_type->n_of_matrix_dimensions();
    for (i = 0; i < total_dimensions - 1; i++)
    {
      gci_type->get_dimension_at(i, dimension);
      cout << dimension << ",";
    }

    gci_type->get_dimension_at(total_dimensions - 1, dimension);
    cout << dimension << ")" << endl;
  }
}

void
print_ref
(int     indent,
  gp_ref* ref)
{
  {
    int    i;
    int    j;
    int    list_dim;
    int    ref_type;
    string saux;
    string saux2;
    string tab1;
    string tab2;

    tab1 = "";
    for (i = 0; i < indent; i++) tab1 += " ";

    // Discover what type of ref we've got.

    ref_type = ref->get_type();

    if (ref_type == gp_ref::GP_REF_TYPE_CRF)
    {
      // Coordinate reference frame.

      list_dim = ref->get_dim();

      // Retrieve the codes in the list of coordinate reference frames.

      cout << tab1 << "CRF:";

      for (j = 0; j < list_dim; j++)
      {
        saux = ref->get_CRF(j);
        cout << " " << saux;
      }
      cout << endl;
    }
    else // (ref_type == gp_ref::GP_REF_TYPE_CS_RF)
    {
      // Reference frame plus optional coordinate system.

      list_dim = ref->get_dim();

      //
      // Retrieve the mandatory codes in the list of reference frames.
      // Get also the OPTIONAL codes of the coordinate systems. These
      // may be empty strings!!!
      //

      cout << tab1 << "RF/CS:";

      for (j = 0; j < list_dim; j++)
      {
        saux = ref->get_RF(j);
        saux2 = ref->get_CS(j);
        cout << " " << saux;
        if (!saux2.empty())  cout << "/" << saux2;
      }
      cout << endl;
    }
  }
}

void
print_tspec
(int       indent,
  gp_tspec* tspec)
{
  {

    int      i;
    int      j;
    int      list_dim;
    gp_ref*  ref;
    string   saux;
    string   tab1, tab2;
    gp_unit* units;

    tab1 = "";
    for (i = 0; i < indent; i++) tab1 += " ";
    tab2 = tab1 + "  ";

    cout << tab1 << "t_spec" << endl;

    // Get the dimension.

    j = tspec->get_dimension();
    cout << tab2 << "dimension: " << j << endl;

    // Get the ref pointer. Print it.

    ref = tspec->get_ref_ptr();
    print_ref(indent + 2, ref);

    // Get the units pointer.

    units = tspec->get_unit_ptr();
    list_dim = units->get_dim();

    cout << tab2 << "Units:";

    for (j = 0; j < list_dim; j++)
    {
      saux = units->get_unit(j);
      cout << " " << saux;
    }
    cout << endl;

  }
}
\endcode
*/



class gp_metadata_parser
{
  public:

    /// \brief Number (DIMension) of errors detected during the
    /// parsing process.
    /**
      \return The number of errors detected by the parser.
    */

    int    errors_dim                      (void) const;

    /// \brief Retrieve an error, identified by its index.
    /**
      \pre (errors_dim() > 0)
      \pre (at_position >= 0)
      \pre (at_position < errors_dim())
      \param at_position Index of the error to retrieve.
      \return The error message at position at_position.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

       Note that the list of errors may not be empty (errors_dim() > 0)
       or this method will throw an exception.

       Indexes are zero based. That is, at_position must be a value
       between 0 and the number of error messages (errors_dim()) minus
       1.
    */

    string errors_get                      (int at_position);

    /// \brief Default constructor.

           gp_metadata_parser              (void);

    /// \brief Destructor.

           ~gp_metadata_parser             (void);

    /// \brief Is it possible to parse the input file?
    /**
      \return True if the parser is ready to start the parsing of the
      input ASTROLABE metadata file, false otherwise.

      Used mainly in preconditions.
    */

    bool   is_ready                        (void) const;

    /// \brief Parse the contents of the metadata file.
    /**
      \pre (is_ready());
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Once the parser is ready (see methods set_parse_parameters() and
      set_parser_parameters_developer()) it is possible to parse
      the ASTROLABE input metadata file and store its contents in the
      gp_metadata object. Use this method to do it.

      Note that the parsing process may report errors If so, the
      contents of the gp_metadata object will not be valid and
      therefore unusable.

      Warnings may also be detected. In this case, the contents
      of the gp_metadata object will be valid; however, the calling
      module should warn that these minor problems have been
      detected.

      To check such situations, the calling module should use methods
      warnings_dim() and errors_dim() wich report, respectively, the
      number of warnings and errors detected. Methods warnings_get()
      and errors_get() may be used to retrieve the description of
      such warnings and errors.
    */

    void   parse                           (void);

    /// \brief \brief Set the parameters needed by the parser to proceed,
    /// production version.
    /**
      \pre (file_name_metadata.size() > 0)
      \pre (!file_name_units_database.empty())
      \pre (!file_name_schema.empty())
      \pre (metadata != NULL);
      \param file_name_metadata Full paths and names of the XML metadata
             files to parse.
      \param file_name_units_database Full path and name of the UDUNITS2 file
             defining its unit system.
      \param file_name_schema Full path and name of the XML schema containing
             the valid syntax of a ASTROLABE metadata file.
      \param metadata The object were the metadata read from the input
             metadata file will be stored.
      \post (is_ready())
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.
      \throw astrolabe_postcondition_violation One or more postconditions
             have been violated.

      Once a gp_metadata_parser has been instantiated, it is necessary
      to set the names of the metadata files to parse, that of the files containing
      the units database and the XML schema as well as the object that will store
      the metadata read from those when parsed.

      This method sets all the required parameters at once, making the parser
      become ready to parse the metadata file.

      Note that this parser is able to parse more than one metadata file
      at once, thus the possibility to set these names simultaneously.

      This method uses the UDUNITS2 library to check the validity of the
      syntax of the \<units\> elements found in many other objects present
      in ASTROLABE metadata. This means that it needs the full path to the
      file defining the UDUNITS2 units system (or database). Typically,
      this file is named "udunits2.xml". Thus, the full path (including the
      file name, just in case someone decided to change the aforementioned
      one) must be passed to this method.

      The XML schema file will let the parser check the correctness of the
      syntax of the input ASTROLABE metadata file.

      \warning It is possible not to set the name of the XML schema file thus
      ignoring the rigorous, schema-based check, and then parse the input
      file. This may be done using set_parser_parameters_developer() instead
      of this method.

      \warning However, this is not recommended in production environments;
      method parse() assumes that a series of checks have been performed
      prior to its execution via the XML schema. If the schema is not set,
      then parse() may fail when facing malformed metadata files.

      \warning Therefore, it is not recommended to use set_parser_parameters_developer()
      in production environments. Use this method instead.

      \see set_parser_parameters_developer()
    */

    void   set_parser_parameters           (vector<string>& file_name_metadata,
                                            string& file_name_units_database,
                                            string& file_name_schema,
                                            gp_metadata* metadata);

    /// \brief Set the parameters needed by the parser to proceed,
    /// developers only version.
    /**
      \pre (file_name_metadata.size() > 0)
      \pre (!file_name_units_database.empty())
      \pre (metadata != NULL)
      \param file_name_metadata Full paths and names of the XML metadata
             files to parse.
      \param file_name_units_database Full path and name of the UDUNITS2 file
             defining its unit system.
      \param metadata The object were the metadata read from the input
             metadata file will be stored.
      \post (is_ready())
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.
      \throw astrolabe_postcondition_violation One or more postconditions
             have been violated.

      \warning THIS METHOD SHOULD BE USED ONLY BY DEVELOPERS OF THIS
             VERY SAME CLASS, to speed up the development process.
             When used, no backing XML schema is available to check the
             correctness of the syntax of the metadata file to parse;
             therefore, the parse() method may fail, since it makes
             some assumptions about some work done by the schema validation
             process that is not made in this case.

      Once a gp_metadata_parser has been instantiated, it is necessary
      to set the names of the metadata files to parse, that of the files
      containing the units database as well as the object that will store
      the metadata read from it when parsed.
      
      Use this method to do it and make the parser become ready to work.

      Note that this parser is able to parse more than one metadata file
      at once, thus the possibility to set these names simultaneously.

      This method uses the UDUNITS2 library to check the validity of the
      syntax of the \<units\> elements found in many other objects present
      in ASTROLABE metadata. This means that it needs the full path to the
      file defining the UDUNITS2 units system (or database). Typically,
      this file is named "udunits2.xml". Thus, the full path (including the
      file name, just in case someone decided to change the aforementioned
      one) must be passed to this method.

      However, the use of this method is highly DISCOURAGED except when
      a developer is improving this very same class!!!

      Method parse() makes quite a number of assumptions about the correctness
      of the syntax of the metadata file to parse (as, for instance, about
      the presence / absence of mandatory tags) that are guaranteed by
      the use of an XML-schema based validation. If such validation does not
      exist (as in this case, where no schema is provided) and a
      syntactically incorrect metadata file is parsed, the parse()
      method will simply fail.

      However, if this class is being modified, for instance, because
      a change in the syntax of the metadata file has to be implemented,
      it may be tedious to modify the corresponding schema prior to
      the implementation of the changes. This is the only reason
      why this method is provided. By calling set_parser_parameters_developer()
      (this method) instead of set_parser_parameters() no schema is required, so
      developers may modify the software more quickly.

      Nevertheless, this is the only situation where this method,
      set_parser_parameters_developer(), should be use. Once the changes have
      been introduced, the XML schema conveniently adapted, and the
      software becomes again "ready for production", method set_parser_parameters()
      should be immediately used instead of this one.

      \see set_parser_parameters()
    */

    void   set_parser_parameters_developer (vector<string>& file_name_metadata,
                                            string&         file_name_units_database,
                                            gp_metadata*    metadata);

    /// \brief Number ((DIMension) of warning detected during the
    /// parsing process.
    /**
      \return The number of warnings detected by the parser.
    */

    int    warnings_dim                    (void) const;

    /// \brief Retrieve a warning, identified by its index.
    /**
      \pre (warnings_dim() > 0)
      \pre (at_position >= 0)
      \pre (at_position < warnings_dim())
      \param at_position Index of the warning to retrieve.
      \return The warning message at position at_position.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

       Note that the list of warnings may not be empty (warnings_dim() > 0)
       or this method will throw an exception.

       Indexes are zero based. That is, at_position must be a value
       between 0 and the number of warning messages (warnings_dim()) minus
       1.
    */

    string warnings_get                    (int at_position);

  protected:

    /// \brief Add the list of syntacticall errors detected by the
    /// internal adp_DOMparser to the list of errors.
    /**
      \param dp The internal adp_DOMparser used to parse the input
             file, which stores a list of parsing errors.

      This is an utility protected function that should never be
      used publicly.
    */

    void   queue_parsing_errors            (adp_DOMparser* dp);

    /// \brief Validate the correctness of the data loaded from the
    /// semantic standpoint.
    /**

      Once the metadata file has been correctly loaded by parse() it is
      necessary to look for incongruent combinations of values, as for
      instance, that the start processing time is less than or equal
      than the end processing time.

      This method takes care of all these checks. It is called by
      parse(), once that this method has been able to load the
      input file successfully.

      Any warnings or errors detected are stored in list_of_errors_
      and list_of_warnings_ respectively.
    */
    
    void   validate_semantics              (void);

  protected:

    /// \brief File names of the XML metadata files to parse.

    vector<string>   file_name_metadata_;

    /// \brief File name of the XML schema used to validate the syntax
    /// of the XML metadata file.

    string           file_name_schema_;

    /// \brief Full path and filename of the UDUNITS2 units database.

    string           file_name_units_database_;

    /// \brief List of parsing ERRORS detected.

    vector<string>   list_of_errors_;

    /// \brief List of parsing WARNINGS detected.

    vector<string>   list_of_warnings_;

    /// \brief The object where the metadata read from the input metadata file
    /// will be stored.

    gp_metadata*     metadata_;

    /// \brief Flag to indicate whether the parser is ready to proceed.

    bool             ready_;

    /// \brief A units validator, used to check the syntax of units according
    /// to the one defined by the UDUNITS2 library.

    gp_units_manager units_manager_;




};

#endif // _GP_METADATA_PARSER_HPP_
