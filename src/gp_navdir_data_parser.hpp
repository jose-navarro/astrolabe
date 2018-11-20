
/** \file gp_navdir_data_parser.hpp
\brief Parser for ASTROLABE navigation directory files.
\ingroup ASTROLABE_metadata
*/

#ifndef _GP_NAVDIR_DATA_PARSER_HPP_
#define _GP_NAVDIR_DATA_PARSER_HPP_

#include "astrolabe_exceptions.hpp"
#include "adp_DOMparser.hpp"

#include "gp_navdir_data.hpp"
#include "gp_utils.hpp"

#include <vector>
#include <string>

using namespace std;

/// \brief Parser for ASTROLABE navigation directory files.
/**

  Class gp_navdir_data_parser offers a very simple API to parse
  (read and load in memory as an object) ASTROLABE navigation
  directory files.

  The steps to perform such parsing are the following:

  - Instantiate a gp_navdir_data_parser object.
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
  These are only messages indicating that some combinations of values
  may be nonsense and that the default action has been taken.

  After a successful parsing, the data loaded is stored in a
  gp_navdir_data object, which is one of the parameters to
  set when configuring the parser. To retrieve data stored
  in the gp_navdir_data object, the interface (API) of
  this object must be used.

  As stated above, there are two different ways to set a
  parser:

  - A "production" mode, where an XML schema defining the
  syntax of ASTROLABE navigation directory files is rigorously
  defined and
  - A "development" mode, where such schema is not required.

  The two methods included in this class' interface to set
  up the parser are set_parser_parameters() ("production"
  mode) and set_parser_parameters_developer() ("developer"
  mode).

  The developer mode should be used only when modifying this
  class, that is, when changes are introduced to adapt
  the parser to any changes included in the ASTROLABE navigation
  directory files. Why?

  Method parse() has been developed assuming that many
  tests about the syntactical correctness of the
  input file have been made through the use of an
  XML schema. This is so because, in this way, the
  final parsing code is much simpler that if every
  possible error (not controlled by an schema validation)
  should be tested by the parser itself.

  Therefore, when no XML schema is present, a syntactically
  incorrect input navigation directory file may lead to
  undesired, uncontrolled errors.

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
  retrieve the data loaded in the gp_navdir_data
  object once the parsing is done. For more information about
  this process, see the documentation of class
  gp_navdir_data.

\code
#include "gp_navdir_data_parser.hpp"
#include "gp_navdir_data.hpp"

#include <iostream>
using namespace std;

void print_lineage(int indent, lineage_data* lineage);

int main
(int   argC,
  char* argV[])
{
  {

    // The parser.

    gp_navdir_data_parser navdir_parser;

    //
    // The navdir data object that will hold the values parsed from the
    // input file.
    //

    gp_navdir_data        navdir_data;

    // Input navigation directory and XML schema file names.

    string                navdir_file;
    string                xml_schema;

    //
    // Auxiliary pointers / objects used to retrieve the metadata
    // once loaded.
    //

    lineage_data*         lineage;
    string                saux;

    // Error and warning handling variables.

    int                   error_total;
    int                   error_index;
    string                error_message;
    int                   warning_total;
    int                   warning_index;
    string                warning_message;

    // Auxiliar variables used in loops.

    int                   i;
    int                   total_items;

    //
    // Test the number of parameters. We must receive an input navigation
    // directory file!
    // argV[0] is the name of the program and argV[1] is the name of the
    // input file. We may also receive the name of a schema in argV[2];
    //

    if (argC < 2)
    {
      cout << "Usage: test_navdir_parser navigation_directory_file [schema_file]" << endl;
      return 1;
    }

    //
    // Depending on the number of parameters, we'll have just a metadata file
    // or, additionally, an schema to validate the syntax of the former.
    //
    // Set up the parser depending on that.
    //

    navdir_file = argV[1];

    if (argC == 2)
    {
      // Developer's setup (NO schema to help validating the input file)

      navdir_parser.set_parser_parameters_developer(navdir_file, &navdir_data);
    }
    else
    {
      // Production set up, with a schema included.

      xml_schema = argV[2];
      navdir_parser.set_parser_parameters(navdir_file, xml_schema, &navdir_data);
    }

    // Parse the input file.

    navdir_parser.parse();

    // Check if the parsing process produced warnings or errors.

    error_total = navdir_parser.errors_dim();

    if (error_total > 0)
    {
      cout << "Parsing finished with errors!" << endl << endl;

      for (error_index = 0; error_index < error_total; error_index++)
      {
        error_message = navdir_parser.errors_get(error_index);
        cout << error_message << endl;
      }
    }
    else
    {
      cout << "Parsing finished with no errors!" << endl;
    }

    warning_total = navdir_parser.warnings_dim();

    if (warning_total > 0)
    {
      cout << "Parsing finished with warnings!" << endl << endl;

      for (warning_index = 0; warning_index < warning_total; warning_index++)
      {
        warning_message = navdir_parser.warnings_get(warning_index);
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

    // The mandatory lineage.

    lineage = navdir_data.get_lineage_ptr();
    print_lineage(0, lineage);

    // The optional input options file and its attributes.

    saux = navdir_data.get_input_options_file_name();
    if (!saux.empty())
    {
      cout << "input options file: " << saux << endl;

      saux = navdir_data.get_input_options_file_format();
      cout << "  format: " << saux << endl;

      saux = navdir_data.get_input_options_file_version();
      if (!saux.empty()) cout << "  version: " << saux << endl;
    }

    // The mandatory input navigation metadata files. There may be several!

    cout << "input metadata" << endl;

    total_items = navdir_data.get_dim_input_metadata_navigation_files();

    for (i = 0; i < total_items; i++)
    {
      saux = navdir_data.get_input_navigation_metadata_file(i);
      cout << "  navigation metadata file: " << saux << endl;
    }

    //
    // Input observations. There will be an observations file, and,
    // optionally, an observations correlations file.
    //

    cout << "input observations" << endl;

    saux = navdir_data.get_input_observations_file_name();
    cout << "  observations file: " << saux << endl;

    saux = navdir_data.get_input_observations_correlations_file_name();
    if (!saux.empty()) cout << "  observations correlations file: " << saux << endl;

    // Optional input instrument file.

    saux = navdir_data.get_input_instruments_file_name();
    if (!saux.empty()) cout << "input instruments file: " << saux << endl;

    // Output optional log file.

    saux = navdir_data.get_output_log_file_name();
    if (!saux.empty()) cout << "output log file: " << saux << endl;

    //
    // Output states. There will be a states file, and,
    // optionally, a states correlations file.
    //

    cout << "output states" << endl;

    saux = navdir_data.get_output_states_file_name();
    cout << "  states file: " << saux << endl;

    saux = navdir_data.get_output_states_correlations_file_name();
    if (!saux.empty()) cout << "  states correlations file: " << saux << endl;

    //
    // Output residuals. There may be a residuals and a residuals
    // correlations files. 
    //

    saux = navdir_data.get_output_residuals_file_name();
    if (!saux.empty())
    {
      cout << "output residuals" << endl;
      cout << "  residuals file: " << saux << endl;

      saux = navdir_data.get_output_residuals_correlations_file_name();
      if (!saux.empty()) cout << "  residuals correlations file: " << saux << endl;
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
\endcode
*/

class gp_navdir_data_parser
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
    between 0 and the number of error messages (errors_dim()) minus 1.
  */

  string errors_get                      (int at_position);

  /// \brief Default constructor.
         gp_navdir_data_parser           (void);

  /// \brief Destructor.
        ~gp_navdir_data_parser           (void);

  /// \brief Is it possible to parse the input file?
  /**
    \return True if the parser is ready to start the parsing of the
    input ASTROLABE navigation directory file, false otherwise.

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
    the ASTROLABE navigation directory file and store its contents in the
    gp_navdir_data object. Use this method to do it.

    Note that the parsing process may report errors If so, the
    contents of the gp_navdir_data object will not be valid and
    therefore unusable.

    Warnings may also be detected. In this case, the contents
    of the gp_navdir_data object will be valid; however, the calling
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
    \pre (!file_name_navdir.empty())
    \pre (!file_name_schema.empty())
    \pre (navdir_data != NULL);
    \param file_name_navdir Full path and name of the navigation directory
           file to parse.
    \param file_name_schema Full path and name of the XML schema containing
           the valid syntax of an ASTROLABE navigation directory file.
    \param navdir_data The object were the navigation data stored in the file
           will be read into.
    \post (is_ready())
    \throw astrolabe_precondition_violation One or more preconditions
           have been violated.
    \throw astrolabe_postcondition_violation One or more postconditions
           have been violated.

    Once a gp_navdir_data_parser has been instantiated, it is necessary
    to set the name of the navigation directory file to parse, that of
    the file containing the XML schema as well as the object that will
    store the navigation data read from the first one when parsed.

    The XML schema file will let the parser check the correctness of the
    syntax of the input ASTROLABE navigation data file.

    This method sets all the required parameters at once, making the parser
    become ready to parse the metadata file.

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

  void   set_parser_parameters           (string&         file_name_navdir,
                                          string&         file_name_schema,
                                          gp_navdir_data* navdir_data);

  /// \brief Set the parameters needed by the parser to proceed,
  /// developers only version.
  /**
    \pre (!file_name_navdir.empty())
    \pre (navdir_data != NULL);
    \param file_name_navdir Full path and name of the navigation data
           directory file to parse.
    \param navdir_data The object were the navigation data stored in the file
           will be read into.
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

    Once a gp_navdir_data_parser has been instantiated, it is necessary
    to set the name of the navigation directory file to parse as well as the
    object that will store the navigation data read from it when parsed.

    Use this method to do it and make the parser become ready to work.

    However, the use of this method is highly DISCOURAGED except when
    a developer is improving this very same class!!!

    Method parse() makes quite a number of assumptions about the correctness
    of the syntax of the navigation directory file to parse (as, for instance, about
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

  void   set_parser_parameters_developer (string&         file_name_navdir,
                                          gp_navdir_data* navdir_data);

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
    between 0 and the number of warning messages (warnings_dim()) minus 1.
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

    Once the navigation directory file has been correctly loaded by parse(),
    it is necessary to look for incongruent combinations of values.

    This method takes care of all these checks. It is called by
    parse(), once that this method has been able to load the
    input file successfully.

    Any warnings or errors detected are stored in list_of_errors_
    and list_of_warnings_ respectively.
  */

  void   validate_semantics(void);

protected:

  /// \brief File name of the XML schema used to validate the syntax
  /// of the XML metadata file.

  string           file_name_schema_;

  /// \brief File name of the navigation data file to parse.

  string           file_name_navdir_data_;

  /// \brief List of parsing ERRORS detected.

  vector<string>   list_of_errors_;

  /// \brief List of parsing WARNINGS detected.

  vector<string>   list_of_warnings_;

  /// \brief The object where the navigation data will be read to.

  gp_navdir_data*  navdir_data_;

  /// \brief Flag to indicate whether the parser is ready to proceed.

  bool             ready_;
};

#endif // _GP_NAVDIR_DATA_PARSER_HPP_
