#include <iostream>
#include <string>
#include "observation_reader.hpp"
#include "observation_writer.hpp"

#include "RecordStructure.hpp"

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

    svalue = "Sample observation-events file";
    lineage.name_set(svalue);

    // Set the author

    svalue = "Event writer tester program";
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
  // Parameters defining the behaviour of the reader.

  string             header_filename;
  string             schema_full_path_and_filename;
  int                read_mode;

  //
  // Names of (1) the ASTROLABE header file describing
  // (2) the output file, where we will copy the information
  // read to check that the process works properly.
  //

  string             output_filename;
  string             external_base_name;

  // Synthetic (nonsensical) lineage to add to the ASTROLABE header file.

  lineage_data       output_file_lineage;

  // An ASTROLABE universal observations reader

  observation_reader reader;

  //
  // An ASTROLABE universal observation writer. Used to write somewhere
  // the ASTROLABE data read by this program.
  //

  observation_writer writer;

  //
  // Data arrays for l-records. Related
  // dimensions.
  //

  double*         the_tags;
  double*         the_expectations;
  double*         the_covariance_values;

  int             the_tags_size;
  int             the_expectations_size;
  int             the_covariance_values_size;

  int             n_tags;
  int             n_expectations;
  int             n_covariance_values;

  //
  // Data arrays for o-records. Related
  // dimensions.
  //

  int*            the_par_iids;
  int*            the_obs_iids;
  int*            the_ins_iids;

  int             the_par_size;
  int             the_obs_size;
  int             the_ins_size;

  int             n_par_iids;
  int             n_obs_iids;
  int             n_ins_iids;

  // The remaining components found in l- and o-records.

  char            type;
  bool            active;
  char*           identifier;
  int             identifier_width;
  int             instance_identifier;
  double          time_tag;

  // Flags, relevant counters.

  bool            epoch_changed;
  bool            forward_reader;
  int             n_epochs;
  int             n_l_records;
  int             n_o_records;
  bool            server_socket;

  // Auxiliary variables for loops, return codes and miscellaneous purposes.

  int             i;
  int             io_status;
  list<string>    problems;
  string          sid;
  int             total_errors;
  int             total_warnings;

  //
  // The auxiliary class used to describe the catalogue
  // of fake records we'll read.
  //

  RecordStructure record_structure_catalogue;

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
  // In the case of files, we'll ask whether to read these in forward
  // or backward direction. If the channel is a socket, we'll ask
  // whether to work in client or server mode.
  //

  forward_reader = true;
  server_socket  = true;

  if (!reader.data_channel_is_socket())
  {
    // Files.

    cout << "The input data channel is a file." << endl;
    cout << "Do you wish to read data in forward (0) or backwards (1) mode?: " ;
    cin >> read_mode;

    forward_reader = (read_mode == 0);
  }
  else
  {
    // Sockets.

    cout << "The input data channel is a TCP socket." << endl;
    cout << "Do you wish this socket to behave as a client (0) or as a server (1) socket? ";
    cin >> read_mode;

    server_socket = (read_mode == 1);
  }

  // --------------------------------------------------------------

  //
  // SET THE READER'S CHANNEL.
  //
  // This is, in fact, necessary only when socket channels are used.
  // Nonetheless, setting the channel when files are used is
  // harmless, so, to simplify the code, the channel is set always.
  //

  reader.set_data_channel(server_socket);

  // --------------------------------------------------------------

  //
  // ASK THE USER. SET THE OUTPUT TEST FILE.
  //
  // Although this example intends to show how the event READER class
  // works, it is necessary to write the information read to somewhere
  // else, so it is possible to check that everything went right.
  //
  // Ask the user what's the name of the output ASTROLABE header file
  // as well as the base name for the external text files actually
  // storing data.
  //
  // Then, we'll use an event writer to write an output text file.
  // For a detailed explanation on how to set up and write an ASTROLABE
  // header + events file, check example test_event_writer.
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
                                      1000000, // Split every 1 million epochs.
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
  // We will dimension the different variable length data
  // arrays according to the maximum dimension needed. To
  // do so, we'll ask the RecordStructure object about
  // the properties of all the fake records it contain.
  //
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


  the_par_size = 1;
  the_obs_size = 1;
  the_ins_size = 1;

  for (i = 0; i < record_structure_catalogue.n_o_record_types(); i++)
  {
    record_structure_catalogue.o_record_data(i, sid, n_par_iids, n_obs_iids, n_ins_iids);

    if (n_par_iids        > the_par_size     ) the_par_size = n_par_iids;
    if (n_obs_iids        > the_obs_size     ) the_obs_size = n_obs_iids;
    if (n_ins_iids        > the_ins_size     ) the_ins_size = n_ins_iids;
    if ((int)sid.length() > identifier_width ) identifier_width = (int)sid.length();
  }

  the_par_iids = new int[the_par_size];
  the_obs_iids = new int[the_obs_size];
  the_ins_iids = new int[the_ins_size];

  identifier_width++; // Take care of ending NULL characters.

  identifier = new char[identifier_width];

  // --------------------------------------------------------------

  //
  // READ THE FILE
  //
  // Open it and iterate until no more data is available. Remember that
  // simultaneously this data will be written to another ASTROLABE file.
  //

  // Try to open the reader, saying whether we need a reversed reader or not.

  io_status = reader.open(!forward_reader);
  if (io_status != 0)
  {
    delete the_tags;      
    delete the_expectations;
    delete the_covariance_values;
    delete the_par_iids;
    delete the_obs_iids;
    delete the_ins_iids;

    delete identifier;

    writer.close();

    cout << "[ERROR] open: " << io_status << endl;
    return 1;
  }

  // Iterate while there are data available.

  n_epochs    = 0;
  n_l_records = 0;
  n_o_records = 0;

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

    // Read the identifier.

    io_status = reader.read_identifier(identifier, identifier_width);
    if (io_status != 0)
    {
      cout << "[ERROR] read_identifier: " << io_status << endl;
      break;
    }

    // Only if we are reading an "l" record, read the instance identifier.

    if (type == 'l')
    {
      io_status = reader.read_instance_id(instance_identifier);
      if (io_status != 0)
      {
        cout << "[ERROR] read_instance_id: " << io_status << endl;
        break;
      }
    }

    // Read (always) the time tag.

    io_status = reader.read_time(time_tag);
    if (io_status != 0)
    {
      cout << "[ERROR] read_time: " << io_status << endl;
      break;
    }

    //
    // At this point it is possible to ask the reader whether AN EPOCH
    // CHANGE has taken place. It is exactly after calling read_time
    // that this situation may be checked. To to so, use method
    // epoch_changed() as shown below.
    //
    // Note that the first epoch (the beginning of the data stream) is
    // never detected as an epoch change.
    //

    epoch_changed = reader.epoch_changed();

    //
    // Our only response to an epoch change in this example will be to
    // increment our counter tallying the number of these.
    //

    if (epoch_changed) n_epochs++;

    //
    // Read, depending on the type of the current record, the right
    // type of data.
    //
    // NOTE THAT THE DIMENSIONS OF ARRAYS have been set before
    // checking the dimensions of all the possible (fake) l- and
    // o-records used in this example (by means of the "record catalogue").
    //
    // A real reader should ascertain the actual dimensions of
    // these arrays using a "mechanism" that would return these dimensions
    // using the identifier previously read in this example.
    //
    // Note, also that our fake record catalog tell us how many
    // tags or expectations (l-records) or elements in the
    // lists of parameter, observation and instrument instance identifiers
    // are there for each type of observation or observation equation
    // being read. This is something that must also be solved
    // somehow by a real reader (and, again, the identifier is
    // the key to solve this problem).
    //


    if (type == 'l')
    {
      // Get the number of elements in the record using the fake catalogue.

      record_structure_catalogue.l_record_data(identifier, n_tags, n_expectations);

      // Read the l-record data.

      io_status = reader.read_l_data(n_tags, the_tags, n_expectations, the_expectations, n_covariance_values, the_covariance_values);
      if (io_status != 0)
      {
        cout << "[ERROR] read_l_data: " << io_status << endl;
        break;
      }

      // Write, at once, the l-record to our test output file.

      io_status = writer.write_l (active, identifier, instance_identifier, time_tag,
                                  n_tags,              the_tags,
                                  n_expectations,      the_expectations,
                                  n_covariance_values, the_covariance_values);
      if (io_status != 0)
      {
        cout << "Error writing l record. Error code in write_l(): " << io_status << endl;
        break;
      }

      // Increment the number of l-records read.

      n_l_records++;

    }
    else // (type == 'o')
    {
      // Get the number of elements in the record using the fake catalogue.

      record_structure_catalogue.o_record_data(identifier,n_par_iids, n_obs_iids, n_ins_iids);

      io_status = reader.read_o_data(n_par_iids, the_par_iids, n_obs_iids, the_obs_iids, n_ins_iids, the_ins_iids);
      if (io_status != 0)
      {
        cout << "[ERROR] read_o_data: " << io_status << endl;
        break;
      }

      // Write, at once, the o-record to our test output file.

      io_status = writer.write_o(active, identifier, time_tag,
                                 n_par_iids, the_par_iids,
                                 n_obs_iids, the_obs_iids,
                                 n_ins_iids, the_ins_iids);

      if (io_status != 0)
      {
        cout << "Error writing o record. Error code in write_o(): " << io_status << endl;
        break;
      }

      // Increment the number of o-records read.

      n_o_records++;

    }
  }

  // Try to close the input file.

  io_status = reader.close();
  if (io_status != 0)
  {
    delete the_tags;      
    delete the_expectations;
    delete the_covariance_values;
    delete the_par_iids;
    delete the_obs_iids;
    delete the_ins_iids;

    delete identifier;

    cout << "[ERROR] close: " << io_status << endl;

    io_status = writer.close();
    if (io_status != 0) cout << "[ERROR] close output: " << io_status << endl;

    return 1;
  }

  delete the_tags;      
  delete the_expectations;
  delete the_covariance_values;
  delete the_par_iids;
  delete the_obs_iids;
  delete the_ins_iids;

  delete identifier;

  io_status = writer.close();
  if (io_status != 0)
  {
    cout << "[ERROR] close output: " << io_status << endl;
    return 1;
  }

  //
  // Dump some statistics to screen. Note how the number of epochs is
  // incremented by one. This is so because our reader is able to
  // detect the transition from one epoch to the next one; the first
  // epoch is never detected as a transition, so it is not in the
  // tally.
  //

  cout << "Total records read: " << n_l_records + n_o_records << endl;
  cout << "of which, there were" << endl;
  cout << "  l-records: " << n_l_records << endl;
  cout << "  o-records: " << n_o_records << endl;
  cout << "Distributed in " << n_epochs + 1 << " epochs." << endl;

  return 0;
}

