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
      cout << "Usage: test_astrolabe_header_file_parser astrolabe_header_file [schema_file]" << endl;
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
      // We've got a socket port here. And we might have a host too.
      //
      //

      iaux2 = astrolabe_header_data.get_device_port();

      cout << "      format: socket" << endl;
      cout << "        port: " << iaux2 << endl;

      if (astrolabe_header_data.is_set_device_server())
      {
        saux = astrolabe_header_data.get_device_server();
        cout << "        host: " << saux << endl;
      }
    }

    cout << "Process finished" << endl;
  }
  return 0;
}
