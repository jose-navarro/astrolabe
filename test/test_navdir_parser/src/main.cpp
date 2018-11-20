#include "gp_navdir_data_writer.hpp"
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

    // A writer object, to make a copy of our input file.

    gp_navdir_data_writer navdir_writer;



    // Input navigation directory and XML schema file names.

    string                navdir_file;
    string                xml_schema;

    // Output navigation directory file.

    string               navdir_output_file;

    //
    // Auxiliary pointers / objects used to retrieve the metadata
    // once loaded.
    //

    lineage_data*         lineage;
    string                saux;

    // Error and warning handling variables.

    int                   error_code;
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
    // Test the number of parameters. We need 4. 
    // argV[0] is the name of the program and argV[1] is the name of the
    // input file. The name of a schema is stored in argV[2] and, finally,
    // argv[3] will keep the name of the output (copied) input file.
    //

    if (argC < 4)
    {
      cout << "Usage: test_navdir_parser input_navdir_file navdir_schema_file output_navdir_file" << endl;
      return 1;
    }

    //
    // We need a total of 4 parameters (program name, input file, schema, output file)
    // to work correctly.
    //

    navdir_file = argV[1];

      // Production set up, with a schema included.

    xml_schema = argV[2];
    navdir_parser.set_parser_parameters(navdir_file, xml_schema, &navdir_data);

    // Copy the name of the output file.

    navdir_output_file = argV[3];

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

    cout << endl << endl;

    // Write a copy of the navigation directory file using our writer.
    
    cout << "WRITING " << navdir_output_file.c_str() << endl;

    error_code = navdir_writer.write(navdir_data, navdir_output_file);

    if (error_code != 0) cout << "Errors detected while writing " << navdir_output_file << endl;

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
