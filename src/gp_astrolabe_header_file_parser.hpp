/** \file gp_astrolabe_header_file_parser.hpp
  \brief Parser for ASTROLABE header files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_ASTROLABE_HEADER_FILE_PARSER_HPP_
#define _GP_ASTROLABE_HEADER_FILE_PARSER_HPP_

#include "astrolabe_exceptions.hpp"
#include "adp_DOMparser.hpp"

#include "gp_astrolabe_header_file_data.hpp"
#include "gp_utils.hpp"

#include <vector>
#include <string>
#include <set>

#include <sstream>

#include <algorithm>

using namespace std;

/// \brief Parser for ASTROLABE header files.
/**

  Class gp_astrolabe_header_file_parser offers a very simple API to parse
  (read and load in memory as an object) ASTROLABE header files.

  The steps to perform such parsing are the following:

  - Instantiate a gp_astrolabe_header_file_parser object.
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
  gp_astrolabe_header_file_data object, which is one of the parameters to
  set when configuring the parser. To retrieve data stored
  in the gp_astrolabe_header_file_data object, the interface (API) of
  this object must be used.

  As stated above, there are two different ways to set a
  parser:

    - A "production" mode, where an XML schema defining the
        syntax of ASTROLABE header files is rigorously defined and
    - A "development" mode, where such schema is not required.

  The two methods included in this class' interface to set
  up the parser are set_parser_parameters() ("production"
  mode) and set_parser_parameters_developer() ("developer"
  mode).

  The developer mode should be used only when modifying this
  class, that is, when changes are introduced to adapt
  the parser to any changes included in the ASTROLABE header
  files. Why?

  Method parse() has been developed assuming that many
  tests about the syntactical correctness of the
  input file have been made through the use of an
  XML schema. This is so because, in this way, the
  final parsing code is much simpler that if every
  possible error (not controlled by an schema validation)
  should be tested by the parser itself.

  Therefore, when no XML schema is present, a syntactically
  incorrect input header file may lead to undesired,
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
  retrieve the data loaded in the gp_astrolabe_header_file_data
  object once the parsing is done. For more information about
  this process, see the documentation of class
  gp_astrolabe_header_file_data.

\code
#include "gp_astrolabe_header_file_data.hpp"
#include "gp_astrolabe_header_file_parser.hpp"

#include <iostream>
using namespace std;

int main
(int argC, char* argV[])
{
  {

    // The parser for astrolabe header files.

    gp_astrolabe_header_file_parser astrolabe_header_parser;

    //
    // The data object that will hold the values parsed from the
    // input file.
    //

    gp_astrolabe_header_file_data   astrolabe_header_data;

    // Input astrolabe header and XML schema file names.

    string                          astrolabe_header_file;
    string                          xml_schema;

    //
    // Auxiliary pointers / objects used to retrieve the metadata
    // once loaded.
    //

    int                             iaux1;
    int                             iaux2;
    lineage_data*                   lineage;
    string                          saux;
    string                          saux2;

    // Error and warning handling variables.

    int                             error_total;
    int                             error_index;
    string                          error_message;
    int                             warning_total;
    int                             warning_index;
    string                          warning_message;

    // Auxiliar variables used in loops.

    int                             j;
    int                             list_dim;

    //
    // Test the number of parameters. We must receive an input
    // astrolabe header file! argV[0] is the name of the program and
    // argV[1] is the name of the astrolabe header input file. We may
    // also receive the name of a validating XML schema in argV[2];
    //

    if (argC < 2)
    {
      cout << "Usage: test_astrolabe_header_parser astrolabe_header_file [schema_file]" << endl;
      return 1;
    }

    //
    // Depending on the number of parameters, we'll have just an observations file
    // or, additionally, an schema to validate the syntax of the former.
    //
    // Set up the parser depending on that.
    //

    astrolabe_header_file = argV[1];

    if (argC == 2)
    {
      // Developer's setup (NO schema to help validating the input file)


      astrolabe_header_parser.set_parser_parameters_developer(astrolabe_header_file, &astrolabe_header_data);
    }
    else
    {
      // Production set up, with a schema included.

      xml_schema = argV[2];
      astrolabe_header_parser.set_parser_parameters(astrolabe_header_file, xml_schema, &astrolabe_header_data);
    }

    // Parse the input file.

    astrolabe_header_parser.parse();

    // Check if the parsing process produced warnings or errors.

    error_total = astrolabe_header_parser.errors_dim();

    if (error_total > 0)
    {
      cout << "Parsing finished with errors!" << endl << endl;

      for (error_index = 0; error_index < error_total; error_index++)
      {
        error_message = astrolabe_header_parser.errors_get(error_index);
        cout << error_message << endl;
      }
    }
    else
    {
      cout << "Parsing finished with no errors!" << endl;
    }

    warning_total = astrolabe_header_parser.warnings_dim();

    if (warning_total > 0)
    {
      cout << "Parsing finished with warnings!" << endl << endl;

      for (warning_index = 0; warning_index < warning_total; warning_index++)
      {
        warning_message = astrolabe_header_parser.warnings_get(warning_index);
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
    // Otherwise, dump the data to screen. This will show how to
    // access the information stored in the gp_astrolabe_header_file_data
    // object.
    //

    if (error_total > 0) return 0;

    cout << "astrolabe-header_file" << endl;

    // Dump the lineage

    lineage = astrolabe_header_data.get_lineage_ptr();

    cout << "  lineage" << endl;

    saux = lineage->id_get();
    cout << "    id: " << saux << endl;

    saux = lineage->name_get();
    if (!saux.empty()) cout << "    name: " << saux << endl;

    list_dim = lineage->author_item_dim();
    if (list_dim > 0)
    {
      cout << "    author" << endl;
      for (j = 0; j < list_dim; j++)
      {
        saux = lineage->author_item_get(j);
        cout << "      item: " << saux << endl;
      }
    }

    saux = lineage->organization_get();
    if (!saux.empty()) cout << "    organization: " << saux << endl;

    saux = lineage->department_get();
    if (!saux.empty()) cout << "    department: " << saux << endl;

    saux = lineage->date_time_get();
    if (!saux.empty()) cout << "    date_time: " << saux << endl;

    list_dim = lineage->ref_document_item_dim();
    if (list_dim > 0)
    {
      cout << "    ref_document" << endl;
      for (j = 0; j < list_dim; j++)
      {
        saux = lineage->ref_document_item_get(j);
        cout << "      item: " << saux << endl;
      }
    }

    saux = lineage->project_get();
    if (!saux.empty()) cout << "    project: " << saux << endl;

    saux = lineage->task_get();
    if (!saux.empty()) cout << "    task: " << saux << endl;

    saux = lineage->remarks_get();
    if (!saux.empty()) cout << "    remarks: " << saux << endl;

    //
    // Dump the astrolabe_header_file_data object.
    //

    cout << "  data" << endl;
    cout << "    device" << endl;

    // Get the type of data being stored or managed.

    saux = astrolabe_header_data.get_device_type();

    cout << "      type: " << saux << endl;

    // Get the format used to store / handle data (text or binary files, client or server sockets).

    iaux1 = astrolabe_header_data.get_device_format();

    if (iaux1 == gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE)
    {
      // We've got a binary file.

      saux = astrolabe_header_data.get_device_file_name();

      cout << "      format: binary file" << endl;
      cout << "        file name: " << saux << endl;

    } else if (iaux1 == gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE)
    {
      // Data is stored in a text file.

      saux = astrolabe_header_data.get_device_file_name();

      cout << "      format: text file" << endl;
      cout << "        file name: " << saux << endl;

    } else
    {
      //
      // We've got a socket port here. Nonetheless, it may be either a CLIENT
      // sender or SERVER (listener) socket.
      //
      //
      // Server sockets need only a port number. Client sockets require,
      // additionally, the server's name or IP address.
      //
      // Note that both need a port number...
      //

      iaux2 = astrolabe_header_data.get_device_port();

      if (iaux1 == gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET_SERVER)
      {
        // Server socket. Just a port number.

        cout << "      format: server socket" << endl;
        cout << "        port: " << iaux2 << endl;
      }
      else
      {
        // Client socket.

        saux = astrolabe_header_data.get_device_server();

        cout << "      format: client socket" << endl;
        cout << "        port  : " << iaux2 << endl;
        cout << "        server: " << saux << endl;
      }
    }

    cout << "Process finished" << endl;
  }
  return 0;
}

\endcode
*/

class gp_astrolabe_header_file_parser
{
  public:

    /// \brief Add one more error to the list of errors.
    /**
      \param the_error The error to add to the list of errors.

      This method is included in order to allow other modules
      to extend the list of errors detected when parsing an
      astrolabe header file. Such errors must be, obviously,
      of semantic kind, since those originating in syntax
      problems are taken care by the this parser itself.

      For instance, if an application can not accept binary
      files as output, it may indicate such condition adding
      an error to this list instead of having to copy it
      completely and adding the new problem to it.
    */

    void   errors_add                       (string& the_error);

    /// \brief Number (DIMension) of errors detected during the
    /// parsing process.
    /**
      \return The number of errors detected by the parser.
    */

    int    errors_dim                       (void) const;

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

    string errors_get                       (int at_position);

    /// \brief Is it possible to parse the input file?
    /**
      \return True if the parser is ready to start the parsing of the
            input ASTROLABE observation-events file, false otherwise.

      Used mainly in preconditions.
    */

    bool   is_ready                         (void) const;

    /// \brief Default constructor.

           gp_astrolabe_header_file_parser  (void);

    /// \brief Destructor.

           ~gp_astrolabe_header_file_parser (void);

    /// \brief Parse the contents of the observation-events file.
    /**
      \pre (is_ready());
      \throw astrolabe_exception Errors parsing the lineage object.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Once the parser is ready (see methods set_parse_parameters() and
      set_parser_parameters_developer()) it is possible to parse
      the ASTROLABE observation-events input file. Use this method
      to do it.

      Note that the parsing process may report errors If so, the
      data loaded from the input file will not be valid and
      therefore unusable.

      Warnings may also be detected. In this case, the contents
      of the loaded data will be valid; however, the calling
      module should warn that these minor problems have been
      detected.

      To check such situations, the calling module should use methods
      warnings_dim() and errors_dim() wich report, respectively, the
      number of warnings and errors detected. Methods warnings_get()
      and errors_get() may be used to retrieve the description of
      such warnings and errors.
    */

    void   parse                            (void);

    /// \brief \brief Set the parameters needed by the parser to proceed,
    /// production version.
    /**
      \pre (!file_name_header.empty())
      \pre (!file_name_schema.empty())
      \pre (header_data != NULL);
      \param file_name_header Full path and name of the
             ASTROLABE header file to parse.
      \param file_name_schema Full path and name of the XML schema containing
             the valid syntax of an observations-events file.
      \param header_data The object were the data read from the input
             ASTROLABE header data file will be stored.
      \post (is_ready())
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.
      \throw astrolabe_postcondition_violation One or more postconditions
             have been violated.

      Once a gp_astrolabe_header_file_parser has been instantiated, it is necessary
      to set the name of the file name to parse, that of the file containing
      the XML schema as well as the object that will store the data read
      from it when parsed.

      The XML schema file will let the parser check the correctness of the
      syntax of the input ASTROLABE heaader file.

      This method sets all the required parameters at once, making the parser
      become ready to parse the input file.

      \warning It is possible not to set the name of the XML schema file thus
      ignoring the rigorous, schema-based check, and then parse the input
      file. This may be done using set_parser_parameters_developer() instead
      of this method.

      \warning However, this is not recommended in production environments;
      method parse() assumes that a series of checks have been performed
      prior to its execution via the XML schema. If the schema is not set,
      then parse() may fail when facing malformed input files.

      \warning Therefore, it is not recommended to use set_parser_parameters_developer()
      in production environments. Use this method instead.

      \see set_parser_parameters_developer()
    */

    void   set_parser_parameters            (string& file_name_header,
                                             string& file_name_schema,
                                             gp_astrolabe_header_file_data* header_data);

    /// \brief Set the parameters needed by the parser to proceed,
    /// developers only version.
    /**
      \pre (!file_name_header.empty());
      \pre (header_data != NULL);
      \param file_name_header Full path and name of the
             ASTROLABE header file to parse.
      \param header_data The object were the data read from the input
             ASTROLABE header file will be stored.
      \post (is_ready())
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.
      \throw astrolabe_postcondition_violation One or more postconditions
             have been violated.

      \warning THIS METHOD SHOULD BE USED ONLY BY DEVELOPERS OF THIS
             VERY SAME CLASS, to speed up the development process.
             When used, no backing XML schema is available to check the
             correctness of the syntax of the input file to parse;
             therefore, the parse() method may fail, since it makes
             some assumptions about some work done by the schema validation
             process that is not made in this case.

      Once a gp_astrolabe_header_file_parser has been instantiated, it is necessary
      to set the name of the file name to parse as well as the object that
      will store the data read from it when parsed.
      
      Use this method to do it and make the parser become ready to work.

      However, the use of this method is highly DISRECOMMENDED except when
      a developer is improving this very same class!!!

      Method parse() makes quite a number of assumptions about the correctness
      of the syntax of the input file to parse (as, for instance, about
      the presence / absence of mandatory tags) that are guaranteed by
      the use of an XML-schema based validation. If such validation does not
      exist (as in this case, where no schema is provided) and a
      syntactically incorrect input file is parsed, the parse()
      method will simply fail.

      However, if this class is being modified, for instance, because
      a change in the syntax of the input file has to be implemented,
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

    void   set_parser_parameters_developer  (string& file_name_header,
                                             gp_astrolabe_header_file_data* header_data);

    /// \brief Add one more warning to the list of warnings.
    /**
      \param the_warning The warning to add to the list of warnings.

      This method is included in order to allow other modules
      to extend the list of warnings detected when parsing an
      astrolabe header file. Such warnings must be, obviously,
      of semantic kind, since those originating in syntax
      problems are taken care by the this parser itself.

      For instance, if an application can not accept binary
      files as output, it may indicate such condition adding
      a warning to this list instead of having to copy it
      completely and adding the new problem to it.
    */

    void   warnings_add                     (string& the_warning);

    /// \brief Number ((DIMension) of warning detected during the
    /// parsing process.
    /**
      \return The number of warnings detected by the parser.
    */

    int    warnings_dim                     (void) const;

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

    string warnings_get                     (int at_position);

  protected:

    /// \brief Parse a string that presumably contains a server and
    /// port specification.
    /**
      \param socket_spec The string containing the server and port
             specification according to ASTROLABE's rules. Read only.
      \param server On output, it will contain the server's name or
             IP address.
      \param port On output, it will contain the server's port.
      \return Error code. Error code values:
              - 0: Successful completion.
              - 1: Invalid syntax. No server name or IP address found in spite
                   of having found a colon to separate server name and port
                   number.
              - 2: Invalid syntax. Invalid or unexistent port number. It must
                   be an integer value.

    */

    int    parse_socket_spec                (string& socket_spec,
                                             string& server,
                                             int&    port);

    /// \brief Add the list of syntacticall errors detected by the
    /// internal adp_DOMparser to the list of errors.
    /**
      \param dp The internal adp_DOMparser used to parse the input
             file, which stores a list of parsing errors.

      This is an utility protected function that should never be
      used publicly.
    */

    void   queue_parsing_errors             (adp_DOMparser* dp);

    /// \brief Validate the correctness of the data loaded from the
    /// semantic standpoint.
    /**

      Once the input file has been correctly loaded by parse() it is
      necessary to look for incongruent combinations of values, as for
      instance, that the value of a socket port is within the
      valid range.

      This method takes care of all these checks. It is called by
      parse(), once that this method has been able to load the
      input file successfully.

      Any warnings or errors detected are stored in list_of_errors_
      and list_of_warnings_ respectively.
    */
    
    void   validate_semantics               (void);

  protected:

    /// \brief File name of the ASTROLABE header file to parse.

    string                         file_name_header_;

    /// \brief File name of the XML schema used to validate the syntax
    /// of the XML metadata file.

    string                         file_name_schema_;

    /// \brief List of parsing ERRORS detected.

    vector<string>                 list_of_errors_;

    /// \brief List of parsing WARNINGS detected.

    vector<string>                 list_of_warnings_;

    /// \brief The object where the metadata read from the ASTROLABE header file
    /// will be stored.

    gp_astrolabe_header_file_data* header_data_;

    /// \brief Flag to indicate whether the parser is ready to proceed.

    bool                           ready_;


};

#endif // _GP_ASTROLABE_HEADER_FILE_PARSER_HPP_
