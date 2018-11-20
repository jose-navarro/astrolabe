#include <iostream>
#include <string>
#include "r_matrix_reader.hpp"
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

    svalue = "Sample correlation matrix file";
    lineage.name_set(svalue);

    // Set the author

    svalue = "Correlation matrix tester program";
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
(int argc, char** argv)
{
  //
  // Input ASTROLABE header file, the schema defining its
  // syntax.
  //

  string           header_filename;
  string           schema_full_path_and_filename;

  //
  // Names of (1) the ASTROLABE header file describing
  // (2) the output file, where we will copy the information
  // read to check that the process works properly.
  //

  string           output_filename;
  string           external_base_name;

  // Synthetic (nonsensical) lineage to add to the ASTROLABE header file.

  lineage_data     output_file_lineage;

  // An ASTROLABE universal r_matrix reader

  r_matrix_reader  reader;

  //
  // An ASTROLABE universal r_matrix writer. Used to write somewhere
  // the ASTROLABE data read by this program.
  //

  r_matrix_writer  writer;

  //
  // Data arrays for r-records. Related
  // dimensions.
  //

  double*          the_correlation_values;
  int              the_correlation_values_size;
  int              n_correlation_values;

  // The remaining components found in r-records.

  char             type;
  bool             active;
  double           time_tag;

  // Flags, relevant counters.

  int              n_r_records;
  int              read_mode;
  bool             server_socket;
  int              socket_mode;

  // Auxiliary variables for loops, return codes and miscellaneous purposes.

  int              io_status;
  list<string>     problems;
  int              total_errors;
  int              total_warnings;


  // --------------------------------------------------------------

  //
  // ASK THE USER.
  //
  // Get the name of the ASTROLABE header file describing the data
  // stream that we'll read. Later, once the metadata contained in
  // this header is read, we'll ask more questions if necessary.
  //
  // Get also the FULL PATH to the schema defining the syntax of
  // ASTROLABE header files.
  //

  cout << "Please, type the name of the input ASTROLABE header file: ";
  cin >> header_filename;

  cout << "Type now the FULL PATH and file name of the schema defining their syntax: ";
  cin >> schema_full_path_and_filename;  

  // --------------------------------------------------------------

  //
  // PARSE the ASTROLABE header file
  //
  // This will load the metadata. If errors are detected (the header
  // file might be incorrect) these will be reported.
  //

  //
  // First, tell the reader where is located the schema file defining
  // the syntax of ASTROLABE header files. This will make the parsing
  // process much more robust.
  //

  io_status = reader.set_schema_file(schema_full_path_and_filename.c_str());

  if (io_status != 0)
  {
    cout << "[ERROR] set_schema_file: " << io_status << endl;
    return 1;
  }

  //
  // Tell the reader what's the file to read so it may parse the
  // metadata defining how actual data is stored.
  //

  io_status = reader.set_header_file(header_filename.c_str());

  if (io_status != 0)
  {
    cout << "[ERROR] set_header_file: " << io_status << endl;
    
    //
    // Error code 3 is a special one. It indicates that warnings, errors
    // or both have been detected when parsing the metadata. In such situation,
    // it is possible to retrieve the textual description of these problems
    // using methods get_metadata_warnings() and get_metadata_errors().
    //
    // Any other error code means that we must stop.
    //

    if (io_status != 3)
    {
      return 1;
    }
    else
    {
      //
      // Problems parsing metadata. If we've got only warnings, we may
      // continue; in the case of errors, we must stop. We'll print
      // every problem (either warnings or errors) and then decide
      // if we must exit or not.
      //

      total_warnings = reader.get_metadata_warnings(problems);

      if (total_warnings > 0)
      {
        cout << "  List of metadata warnings" << endl;
        while (!problems.empty())
        {
          cout << "  [WARNING] " << problems.front()  << endl;
          problems.pop_front();
        }
        cout << endl;
      }

      total_errors = reader.get_metadata_errors(problems);

      if (total_errors > 0)
      {
        cout << "  List of metadata errors" << endl;
        while (!problems.empty())
        {
          cout << "  [ERROR] " << problems.front()  << endl;
          problems.pop_front();
        }
        cout << endl;
      }

      // Exit only when metadata ERRORS have been detected.

      if (total_errors > 0) return 1;
    }
  }

  // --------------------------------------------------------------

  //
  // ASK THE USER.
  //
  // Depending on the type of data channel to deal with, ask the
  // appropriate questions to the user.
  //
  // If the channel is a socket, we'll ask whether to work in client
  // or server mode.
  //

  server_socket = true;

  if (!reader.data_channel_is_socket())
  {
    // Files.

    cout << "The input data channel is a file." << endl;
    cout << "Do you wish to read it in forward (0) or backwards (1) direction?";
    cin  >> read_mode;
  }
  else
  {
    // Sockets.

    cout << "The input data channel is a TCP socket." << endl;
    cout << "Do you wish this socket to behave as a client (0) or as a server (1) socket? ";
    cin >> socket_mode;

    server_socket = (socket_mode == 1);

    read_mode = 0; // Sockets are always read in forward direction.
  }

  // --------------------------------------------------------------

  //
  // SET THE READER'S CHANNEL.
  //
  // This is, in fact, necessary only when socket channels are used.
  // Nonetheless, setting the channel when files are used is
  // harmless, so, to simplify the code, the channel is always set.
  //

  reader.set_data_channel(server_socket);

  // --------------------------------------------------------------

  //
  // ASK THE USER. SET THE OUTPUT TEST FILE.
  //
  // Although this example intends to show how the r_matrix READER class
  // works, it is necessary to write the information read to somewhere
  // else, so it is possible to check that everything went right.
  //
  // Ask the user what's the name of the output ASTROLABE header file
  // as well as the base name for the external text files actually
  // storing data.
  //
  // Then, we'll use an r_matrix writer to write an output text file.
  // For a detailed explanation on how to set up and write an ASTROLABE
  // header + r_matrix file, check example test_rmatrix_writer.
  //

  cout << "Please, type the name of the output ASTROLABE header file: ";
  cin >> output_filename;

  cout << "Now type the base file name for the external text files: ";
  cin >> external_base_name;

  //
  // Build a synthetic, fake lineage. Real applications must provide with
  // proper values for the different fields in such lineage.
  //

  set_synthetic_lineage_data(output_file_lineage);

  //
  // Configure the writer using the information just obtained; other parameters
  // are just hardwired to make things easier.
  //

  io_status = writer.set_data_channel(output_filename.c_str(),
                                      external_base_name.c_str(),
                                      false, // Not binary, that is, text.
                                      900, // Split every 900 epochs.
                                      9, // Digits in the names of the split files.
                                      &output_file_lineage); // The syntetic lineage.

  // Open the output file.

  io_status = writer.open();
  if (io_status != 0)
  {
    cout << "[ERROR] open output: " << io_status << endl;
    return 1;
  }


  // --------------------------------------------------------------

  // AUXILIARY WORK.

  //
  // We will instantiate the correlation values array to be able to hold
  // the correlations of covariance matrices up to 100 * 100 values. Therefore,
  // the dimension of this array will be (100 * 99) / 2.
  //
  // Any correlation matrix bigger than this dimension will make crash
  // this program.
  //

  the_correlation_values_size = (100 * 99) / 2; // Read comments above.
  the_correlation_values      = new double[the_correlation_values_size];

  // --------------------------------------------------------------

  //
  // READ THE FILE
  //
  // Open it and iterate until no more data is available. Remember that
  // simultaneously this data will be written to another ASTROLABE file.
  //

  // Try to open the reader.

  io_status = reader.open((read_mode != 0));
  if (io_status != 0)
  {
    delete the_correlation_values;

    writer.close();

    cout << "[ERROR] open: " << io_status << endl;
    return 1;
  }

  cout << "The input file will be read in";

  if (read_mode == 0) cout << " forward direction" << endl;
  else                cout << " backward direction" << endl;

  // Iterate while there are data available.

  n_r_records = 0;

  while (true)
  {
    // Read the tag type.

    io_status = reader.read_type(type);
    if (io_status != 0)
    {
      //
      // read_type is the method that, legally, may return an error
      // code stating that the file is over. Of course, other errors
      // might appear. However, when an error condition is detected,
      // the first thing to do is check if, in fact, an error condition
      // is active, since this would simply mean that the process
      // of reading the input file is over.
      //
      // Note, additionally, that the ASTROLABE parameter reader will
      // always return an "l" type, since no o-records are allowed
      // in parameter data channels.
      //

      if (reader.is_eof())
      {
        // Legal end-of-file condition.
        break;
      }
      else
      {
        //
        // The error reported by read_type is NOT an end-of-file condition.
        // Therefore, we must stop.
        //

        cout << "[ERROR] read_type: " << io_status << endl;
        break;
      }
    }

    // Read the active flag.

    io_status = reader.read_active_flag(active);
    if (io_status != 0)
    {
      cout << "[ERROR] read_active_flag: " << io_status << endl;
      break;
    }

    // Read the time tag.

    io_status = reader.read_time(time_tag);
    if (io_status != 0)
    {
      cout << "[ERROR] read_time: " << io_status << endl;
      break;
    }

    //
    // Read, the remaining data in the r-record.
    //
    // NOTE THAT THE DIMENSIONS OF THE DATA ARRAY has been set to cope
    // with a "big enough" case (100 x 100 covariance matrices). If the
    // data stored in the file goes beyond this limit ((100 * 99) /2, 
    // the actual number of correlation values in a 100x100 covariance
    // matrix) the program will crash.
    //

    // Read the r-record data.

    io_status = reader.read_r_data(n_correlation_values, the_correlation_values);
    if (io_status != 0)
    {
      cout << "[ERROR] read_r_data: " << io_status << endl;
      break;
    }

    // Write, at once, the r-record to our test output file.

    io_status = writer.write_r (active,               time_tag,
                                n_correlation_values, the_correlation_values);
    if (io_status != 0)
    {
      cout << "Error writing r record. Error code in write_r(): " << io_status << endl;
      break;
    }

    // Increment the number of r-records read.

    n_r_records++;

  }

  // Try to close the input file.

  io_status = reader.close();
  if (io_status != 0)
  {
    delete the_correlation_values;

    cout << "[ERROR] close: " << io_status << endl;

    io_status = writer.close();
    if (io_status != 0) cout << "[ERROR] close output: " << io_status << endl;

    return 1;
  }

  delete the_correlation_values;

  io_status = writer.close();
  if (io_status != 0)
  {
    cout << "[ERROR] close output: " << io_status << endl;
    return 1;
  }

  //
  // Dump some statistics to screen.
  //

  cout << "Total of r-records read: " << n_r_records << endl;

  return 0;
}

