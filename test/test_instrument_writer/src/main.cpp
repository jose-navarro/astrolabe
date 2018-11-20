#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "RecordStructure.hpp"
#include "RecordSequence.hpp"

#include "instrument_writer.hpp"

using namespace std;

void
set_synthetic_lineage_data
(lineage_data& lineage)
{
  {
    string svalue;

    // Set the identifier.

    svalue = "ID0";
    lineage.id_set(svalue);

    // Set the name

    svalue = "Sample instrument (obs-e) file";
    lineage.name_set(svalue);

    // Set the author

    svalue = "Instrument writer tester program";
    lineage.author_item_add(svalue);

    // Set the organization

    svalue = "The name of the organization goes here";
    lineage.organization_set(svalue);

    // Set the department.

    svalue = "The name of the department goes here";
    lineage.department_set(svalue);

    // Set the project

    svalue = "The name of the project goes here";
    lineage.project_set(svalue);

    // Set the task

    svalue = "The name of the task goes here";
    lineage.task_set(svalue);

    // Set the reference documents

    svalue = "As many entries as desired may be added to refer to related documentation";
    lineage.ref_document_item_add(svalue);

    // Set the remarks

    svalue = "Any interesting remarks go here";
    lineage.remarks_set(svalue);

  }
}

int
main
(int argv, char** argc)
{
  {
    //
    // Data obtained from the user, which will be used to
    // select and configure our data channel.
    //

    int     device_format;
    string  host;
    string  instruments_file_name;
    string  outfile_name;
    int     port;
    int     server_mode;

    //
    // Data arrays for l-records. Related
    // dimensions.
    //

    double* the_tags;
    double* the_expectations;
    double* the_covariance_values;

    int     the_tags_size;
    int     the_expectations_size;
    int     the_covariance_values_size;

    int     n_tags;
    int     n_expectations;
    int     n_covariance_values;

    // Metadata to generate fake l- and o- records.

    string  sid;
    char*   identifier;
    int     identifier_width;
    double  time_tag;
    int     record_id_index;
    int     cycle;

    // The ASTROLABE generic instrument writer.

    instrument_writer writer;

    // Synthetic (nonsensical) lineage to add to the ASTROLABE header file.

    lineage_data file_lineage;

    //
    // The auxiliary classes that will generate record
    // sequence hints and will describe the structure
    // or these records.
    //

    RecordSequence  the_record_sequence;
    RecordStructure record_structure_catalogue;

    // Other auxiliary variables, as error codes or loop indices.

    int     i;
    int     line_index;
    int     random_value;
    int     io_status;
    int     total_lines;

    // --------------------------------------------------------------

    // PRELIMINARY WORK.

    //
    // We will dimension the different variable length data
    // arrays according to the maximum dimension needed. To
    // do so, we'll ask the RecordStructure object about
    // the properties of all the fake records it contain.
    //

    identifier_width           = 0;
    the_tags_size              = 1;
    the_expectations_size      = 1;
    the_covariance_values_size = 1;

    for (i = 0; i < record_structure_catalogue.n_l_record_types(); i++)
    {
      record_structure_catalogue.l_record_data(i, sid, n_tags, n_expectations);

      if (n_tags            > the_tags_size         ) the_tags_size = n_tags;
      if (n_expectations    > the_expectations_size ) the_expectations_size = n_expectations;
      if ((int)sid.length() > identifier_width      ) identifier_width = (int)sid.length();
    }

    the_covariance_values_size = (the_expectations_size * (the_expectations_size + 1)) / 2;

    the_tags              = new double[the_tags_size];
    the_expectations      = new double[the_expectations_size];
    the_covariance_values = new double[the_covariance_values_size];

    identifier = new char[identifier_width + 1];

    identifier[identifier_width] = 0;
    time_tag                     = 1.0;

    // --------------------------------------------------------------

    //
    // ASK THE USER.
    //
    // Get the format of the device (text file or socket ONLY)
    // to deal with.
    //

    instruments_file_name = "";
    device_format = 0;

    while ((device_format <= 0) || (device_format > 3))
    {

      cout << "Type of output file to create" << endl << endl;
      cout << "  Text file   : 1" << endl;
      cout << "  Socket      : 2" << endl;
      cout << "  QUIT!!!     : 3" << endl << endl;
      cout << " Your choice? : ";
      cin >> device_format;
    }

    if (device_format == 3) return 0;

    // Set the writer channel according to its type.

    if (device_format == 1)
    {
      // Text file. Get its name and that for the ASTROLABE header file.

      cout << "Name of the ASTROLABE header file: ";
      cin >> outfile_name;

      cout << "Name of the file with actual instrument data: ";
      cin >> instruments_file_name;

      //
      // When working with files, we must provide with a lineage,
      // which is part of the header included in the ASTROLABE header file.
      // We generate here a "synthetic" noe (including nonsensical values).
      //

      set_synthetic_lineage_data(file_lineage);

    }
    else
    {
      // Sockets.

      cout << "Port to use                      : ";
      cin  >> port;

      server_mode = -1;
      while ((server_mode < 0) | (server_mode > 1))
      {
        cout << "Work as server (0) or client (1) : ";
        cin >> server_mode;
      }

      host = "";
      if (server_mode == 1)
      {
        cout << "Host or IP address to connect to : ";
        cin  >> host;
      }
    }

    // --------------------------------------------------------------

    //
    // SET THE DEVICE CHANNEL according to the data provided by the
    // user.
    //

    switch (device_format)
    {
      case 1:

        // Text files.

        io_status = writer.set_data_channel(outfile_name.c_str(),
                                            instruments_file_name.c_str(),
                                            &file_lineage);

        break;

      case 2:

        // Sockets.

        io_status = writer.set_data_channel(host.c_str(), port, (server_mode == 0));

        break;
    }

    // Check that the data channel has been correctly set.

    if (io_status != 0)
    {
      cout << "set_data_channel reported error code " << io_status << endl;
      return 0;
    }


    // --------------------------------------------------------------

    //
    // WRITE DATA to the selected channel, whatever it is.
    //

    // Open the underlying channel.

    io_status = writer.open();
    if (io_status != 0)
    {
      cout << "open() reported error code: " << io_status << endl;
      return 1;
    }


    //
    // Insert a bunch of records, as many as different ones has our
    // fake catalog.
    //

    total_lines = the_record_sequence.cycle_length();

    for (line_index = 0; line_index < total_lines; line_index++)
    {

      //
      // Get the next record index.
      //

      the_record_sequence.get_next(record_id_index, cycle);

      time_tag = (double)cycle;

      //
      // Retrieve the structural information for the l-record
      // whose index we've just generated. Then, generate fake
      // data for the l-record and write it to the output file.
      //

      record_structure_catalogue.l_record_data(record_id_index, sid, n_tags, n_expectations);

      // Identifier.

      identifier = strcpy(identifier, sid.c_str());

      //
      // Decide randomly the shape of the covariance matrix: none, only
      // standard deviations, full matrix (including correlations).
      // Generate fake values at the same time.
      //

      random_value = rand() % 100 + 1;

      if (random_value <= 5)
      {
        n_covariance_values = 0;
      } else if (random_value <= 95)
      {
        n_covariance_values = n_expectations;
        for (i = 0; i < n_covariance_values; i++)
        {
          the_covariance_values[i] = (double)(i + line_index);
        }
      } else
      {
        n_covariance_values = (n_expectations * (n_expectations + 1)) / 2;
        for (i = 0; i < n_covariance_values; i++)
        {
          the_covariance_values[i] = (double)(i + line_index);
        }
        for (i = n_expectations; i < n_covariance_values; i++)
        {
          the_covariance_values[i] = (double)i / 100;
        }
      }

      // Generate fake data for the tags and expectations.

      for (i = 0; i < n_tags;         i++) the_tags[i]         = (double)(i + line_index);
      for (i = 0; i < n_expectations; i++) the_expectations[i] = (double)(i + line_index + 1);

      // Write the l-record.

      io_status = writer.write_l (true, identifier, line_index, time_tag,
                                  n_tags,              the_tags,
                                  n_expectations,      the_expectations,
                                  n_covariance_values, the_covariance_values);
      if (io_status != 0)
      {
        cout << "Error writing l record. Error code in write_l(): " << io_status << endl;
        break;
      }
    }


    // Close the writer.

    io_status =  writer.close();
    if (io_status != 0) cout << "Error closing file. Error code in close(): " << io_status;

    // Cleaning.

    delete identifier;
    delete the_tags;        
    delete the_expectations;
    delete the_covariance_values;

  }

  return 0;
}
