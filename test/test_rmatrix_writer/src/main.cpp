#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "r_matrix_writer.hpp"

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

    svalue = "Sample r-matrix file";
    lineage.name_set(svalue);

    // Set the author

    svalue = "r_matrix writer tester program";
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
    string  external_base_name;
    string  host;
    string  outfile_name;
    int     port;
    int     server_mode;

    //
    // Data arrays for r-records. Related
    // dimensions.
    //

    double* the_correlation_values;
    int     the_correlation_values_size;
    int     n_correlation_values;

    // Time tag (for r-records).

    double  time_tag;

    // The ASTROLABE generic r_matrix writer.

    r_matrix_writer writer;

    // Synthetic (nonsensical) lineage to add to the ASTROLABE header file.

    lineage_data file_lineage;


    // Other auxiliary variables, as error codes or loop indices.

    int     i;
    bool    file_is_binary;
    int     line_index;
    int     matrix_dimension;
    int     io_status;
    int     total_lines;

    // --------------------------------------------------------------

    // PRELIMINARY WORK.

    //
    // Initializations
    //


    //
    // For this example, the maximum dimension of covariance matrices will be
    // 10 x 10. Therefore, the maximum number of correlation values is
    // (10 * 9) / 2.
    //

    the_correlation_values_size = (10 * 9) / 2;
    the_correlation_values = new double[the_correlation_values_size];

    time_tag = 1.0;

    // --------------------------------------------------------------

    //
    // ASK THE USER.
    //
    // Get the format of the device (text or binary file, socket)
    // to deal with.
    //

    external_base_name = "";
    device_format = 0;

    while ((device_format <= 0) || (device_format > 4))
    {

      cout << "Type of output file to create" << endl << endl;
      cout << "  Binary file : 1" << endl;
      cout << "  Text file   : 2" << endl;
      cout << "  Socket      : 3" << endl;
      cout << "  QUIT!!!     : 4" << endl << endl;
      cout << "Your choice?  : ";
      cin >> device_format;
    }

    if (device_format == 4) return 0;

    // Set the writer channel according to its type.

    if (device_format == 3)
    {
      // Sockets.

      cout << "Port to use                      : ";
      cin >> port;

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
        cin >> host;
      }

      io_status = writer.set_data_channel(host.c_str(), port, server_mode == 0);
    }

    else
    {
      // Working with files.

      // Get the names of the several files involved in the process.

      cout << "Name of the ASTROLABE header file: ";
      cin >> outfile_name;

      cout << "Base name for the external splitted files: ";
      cin >> external_base_name;

      //
      // No matter the type of file we're dealing with, we must provide with a lineage,
      // which is part of the header included in the ASTROLABE header file.
      // We generate here a "synthetic" noe (including nonsensical values).
      //

      set_synthetic_lineage_data(file_lineage);


      // --------------------------------------------------------------

      //
      // SET THE DEVICE CHANNEL according to the data provided by the
      // user.
      //

      file_is_binary = (device_format == 1);


      //
      // We fix the maximum number of records per chunk to 900 and the suffix
      // witdh to 9. These values could be any within the legal ranges.
      //

      io_status = writer.set_data_channel(outfile_name.c_str(),
                                          external_base_name.c_str(),
                                          file_is_binary,  // Binary
                                          900,
                                          9,
                                          &file_lineage);
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
    // Insert 9000 records. The number of correlation values in each
    // record will range from (2 * 1) / 2, that is, 2x2 matrices, to
    // (10 x 9) /2, 10x10 matrices.
    //

    total_lines      = 9000;
    matrix_dimension = 2;

    for (line_index = 0; line_index < total_lines; line_index++)
    {

      // Compute the number of correlation values.

      n_correlation_values = (matrix_dimension * (matrix_dimension - 1)) / 2;
      
      matrix_dimension++;
      if (matrix_dimension > 10) matrix_dimension = 2;

      //
      // "Compute" the time tag. It is equal to the line index.
      //

      time_tag = (double)line_index;

      // Generate fake data for the correlation values.

      for (i = 0; i < n_correlation_values; i++) the_correlation_values[i] = ((double)(i + line_index ))/((double)total_lines * 2.0);

      // Write the r-record.

      io_status = writer.write_r (true, time_tag,
                                  n_correlation_values, the_correlation_values);
      if (io_status != 0)
      {
        cout << "Error writing r record. Error code in write_l(): " << io_status << endl;
        break;
      }
    }


    // Close the writer.

    io_status =  writer.close();
    if (io_status != 0) cout << "Error closing file. Error code in close(): " << io_status;

    // Cleaning.

    delete the_correlation_values;

  }

  return 0;
}
