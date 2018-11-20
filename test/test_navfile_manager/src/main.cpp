// IMPORTANT WARNING
//
// When compiling in Windows environments, define the
// macro WIN32_LEAN_AND_MEAN as a preprocessor directive
// in your IDE. This will avoid the redefinition of
// DOMDocument introduced by both Xerces and (Microsoft's)
// msxml.h.
//

#include "gp_navdir_data.hpp"
#include "gp_navfile_manager.hpp"
#include "gp_navdir_data_parser.hpp"

#include <stdio.h>
#include <iostream>

#include <string>
#include <vector>

using namespace std;

const vector<string> NAVDIR_FILENAMES({"nav_directory_fw.ndf",
                                       "nav_directory_bw.ndf",
                                       "nav_directory_cb.ndf"});

void
create_fake_file
(const string& filename)
{
  {
    const char* buffer;
    size_t      blen;
    FILE*       pFile;

    buffer = filename.c_str();
    blen   = filename.size();

    pFile = fopen (filename.c_str(), "w");
    fwrite (buffer, sizeof(char), blen, pFile);
    fclose (pFile);
    return;
  }
}

int main
(int   argC,
  char* argV[])
{
  {

    // The parser.

    gp_navdir_data_parser*  navdir_parser;

    //
    // The navdir data object that will hold the values parsed from the
    // input file.
    //

    gp_navdir_data*         navdir_data;

    // The mode used to create the navigation file.

    int                     mode;

    // Navigation directory and XML schema file names.

    string                  navdir_file;
    vector<string>          navdir_file_list;
    string                  xml_schema;

    //
    // Output navigation file. Path to extract the navigation
    // file once it has been created.
    //

    string                  navigation_file;
    string                  path_to_extract;

    //
    // The navigation file manager able to create such kind of
    // files.
    //

    gp_navfile_manager      navfile_manager;

    // List of fake files created.

    vector<string>          fake_files_to_delete;

    //
    // Auxiliary pointers / objects used to retrieve the metadata
    // once loaded.
    //

    string                  saux;

    // Error and warning handling variables.

    int                     error_code;
    int                     error_total;
    int                     error_index;
    string                  error_message;
    int                     warning_total;
    int                     warning_index;
    string                  warning_message;

    // Auxiliar variables used in loops.

    int                     end;
    int                     i;
    int                     j;
    int                     start;
    int                     total_items;

    // Let's start. Get the XML schema defining navigation directori files.

    cout << "test_navfile_manager" << endl;
    cout << "====================" << endl << endl;
    cout << "FULL path to navigation directory XML schema: ";
    cin >> xml_schema;

    //
    // Get the mode used to create the navigation file. Depending on this
    // value, we'll have to load 1 or 3 navigation directory files.
    //

    mode = -1;

    while ((mode < 0) | (mode > 3))
    {
      cout << endl;
      cout << "Navigation file mode?" << endl;
      cout << " 0 - Forward" << endl;
      cout << " 1 - Backwards" << endl;
      cout << " 2 - Combined (block)" << endl;
      cout << " 3 - Combined (Forward + Backwards + Smoothing)" << endl;
      cout << "?: ";
      cin  >> mode;
    }

    cout << endl;

    //
    // Tell the user about our needs and set up the loop limits depending
    // on the mode selected.
    //

    if (mode == 3)
    {
      start = 0;
      end = 2;
    }
    else
    {
      start = end = mode;
    }

    // Iterate as many times as necessary, depending on the mode selected.

    navdir_file_list.clear();
    fake_files_to_delete.clear();

    for (i = start; i <= end; i++)
    {

      //
      // Get the name of the ith navigation directory. It is
      // derived from the mode used.
      //

      navdir_file = NAVDIR_FILENAMES[i];
      navdir_file_list.push_back("./" + navdir_file);

      // Create a navigation directory file parser.

      navdir_parser = new gp_navdir_data_parser();

      // Production set up, with a schema included.

      navdir_data = new gp_navdir_data();

      navdir_parser->set_parser_parameters(navdir_file, xml_schema, navdir_data);

      // Parse the input file.

      navdir_parser->parse();

      // Check if the parsing process produced warnings or errors.

      error_total = navdir_parser->errors_dim();

      if (error_total > 0)
      {
        cout << "Parsing finished with errors!" << endl << endl;

        for (error_index = 0; error_index < error_total; error_index++)
        {
          error_message = navdir_parser->errors_get(error_index);
          cout << error_message << endl;
        }
      }

      warning_total = navdir_parser->warnings_dim();

      if (warning_total > 0)
      {
        cout << "Parsing finished with warnings!" << endl << endl;

        for (warning_index = 0; warning_index < warning_total; warning_index++)
        {
          warning_message = navdir_parser->warnings_get(warning_index);
          cout << warning_message << endl;
        }
      }

      //
      // If errors have been detected, stop the process here.
      //
      // Otherwise, create fake files for all those reported in
      // the navigation data object just obtained. These will
      // be part of the output (compressed) navigation file.
      //
      // By the way, we need the parser no more.
      //

      delete navdir_parser;
      navdir_parser = NULL;

      if (error_total > 0) return 0;

      // Let's create these fake files.

      // The optional input options file.

      saux = navdir_data->get_input_options_file_name();
      if (!saux.empty())
      {
        create_fake_file(saux);
        fake_files_to_delete.push_back(saux);
      }

      // The mandatory input navigation metadata files. There may be several!

      total_items = navdir_data->get_dim_input_metadata_navigation_files();

      for (j = 0; j < total_items; j++)
      {
        saux = navdir_data->get_input_navigation_metadata_file(j);
        create_fake_file(saux);
        fake_files_to_delete.push_back(saux);
      }

      //
      // Input observations. There will be an observations file, and,
      // optionally, an observations correlations file.
      //

      saux = navdir_data->get_input_observations_file_name();
      create_fake_file(saux);
      fake_files_to_delete.push_back(saux);

      saux = navdir_data->get_input_observations_correlations_file_name();
      if (!saux.empty())
      {
        create_fake_file(saux);
        fake_files_to_delete.push_back(saux);
      }

      // Optional input instrument file.

      saux = navdir_data->get_input_instruments_file_name();
      if (!saux.empty())
      {
        create_fake_file(saux);
        fake_files_to_delete.push_back(saux);
      }

      // Output optional log file.

      saux = navdir_data->get_output_log_file_name();
      if (!saux.empty())
      {
        create_fake_file(saux);
        fake_files_to_delete.push_back(saux);
      }

      //
      // Output states. There will be a states file, and,
      // optionally, a states correlations file.
      //

      saux = navdir_data->get_output_states_file_name();
      create_fake_file(saux);
      fake_files_to_delete.push_back(saux);

      saux = navdir_data->get_output_states_correlations_file_name();
      if (!saux.empty())
      {
        create_fake_file(saux);
        fake_files_to_delete.push_back(saux);
      }

      //
      // Output residuals. There may be a residuals and a residuals
      // correlations files. 
      //

      saux = navdir_data->get_output_residuals_file_name();
      if (!saux.empty())
      {
        create_fake_file(saux);
        fake_files_to_delete.push_back(saux);

        saux = navdir_data->get_output_residuals_correlations_file_name();
        if (!saux.empty())
        {
          create_fake_file(saux);
          fake_files_to_delete.push_back(saux);
        }
      }
    }

    // We need the navigation data object no more.

    delete navdir_data;
    navdir_data = NULL;

    //
    // Here we have all the information needed to create the output
    // (compressed) navigation file BUT its name...
    //

    cout << "Name of the output (compressed) navigation file: ";
    cin  >> navigation_file;

    error_code = navfile_manager.create(mode, navdir_file_list, fake_files_to_delete, navigation_file);

    if (error_code != 0) cout << "Error creating the navigation file: " << error_code << endl;

    //
    // We'll remove here all the fake files that we have created to
    // have something to compress...
    //

    for (i = 0; i < (int)fake_files_to_delete.size(); i++)
      remove(fake_files_to_delete[i].c_str());

    fake_files_to_delete.clear();

    //
    // Now we'll extract the files just included in the navigation file.
    // It is pretty simple; the only extra info we need is the path
    // to the folder where these files have to be decompressed...
    //

    cout << "FULL path to the folder were the navigation file will be extracted?" << endl;
    cout << "?: ";
    cin  >> path_to_extract;

    error_code = navfile_manager.extract(navigation_file, path_to_extract);

    if (error_code != 0) cout << "Error extracting the navigation file: " << error_code << endl;


    // THAT'S ALL.

    cout << "Process finished" << endl;

    return 0;
  }
}
