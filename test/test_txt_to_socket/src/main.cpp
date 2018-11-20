#include <iostream>
#include <string>
#include <sstream>

#include "RecordStructure.hpp"

#include "observation_file_reader_txt_forward.hpp"
#include "observation_socket_writer.hpp"

using namespace std;

int
main
(int argc, char** argv)
{

  // The name of the input file. Server and port
  // of the destination socket.

  string                               infile_name;
  string                               server;
  int                                  port;

  // The text reader (forward) and binary writer objects.

  observation_file_reader_txt_forward* text_reader;
  observation_socket_writer*           socket_writer;

  // Auxiliary data. Return codes, counters for loops.

  int                                  io_status;
  int                                  i;
  string                               sid;

  // Variables to hold l- or o-records.

  char                                 type;
  bool                                 active;
  char*                                identifier;
  int                                  identifier_width;
  int                                  instance_identifier;
  double                               time;
  int                                  n_tags;
  int                                  the_tags_size;
  double*                              the_tags;
  int                                  n_expectations;
  int                                  the_expectations_size;
  double*                              the_expectations;
  int                                  n_covariance_values;
  int                                  the_covariance_values_size;
  double*                              the_covariance_values;
  int                                  n_par_iids;
  int                                  the_par_size;
  int*                                 the_par_iids;
  int                                  n_obs_iids;
  int                                  the_obs_size;
  int*                                 the_obs_iids;
  int                                  n_ins_iids;
  int                                  the_ins_size;
  int*                                 the_ins_iids;

  //
  // The auxiliary class that describes the structure
  // or the fake records.
  //

  RecordStructure                      record_structure_catalogue;

  //
  // Check the number of parameters. We need at least three (besides
  // the name of the program): one for the input text file, one more
  // for the server name (or IP address) and, finally, one last parameter
  // for the port number.
  //

  if (argc < 4)
  {
    cout << "usage: test_txt_to_socket input_txt_file_name server_name_or_IP_address port_number" << endl;
    return 1;
  }

  infile_name  = argv[1];
  server       = argv[2];

  // Must change the port number from string to int form.

  istringstream ss(argv[3]);
  if (!(ss >> port))
  {
    cout << "[ERROR] " << argv[3] << " is not a valid port number " << endl;
    return 1;
  }

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

  identifier = new char[identifier_width + 1];
  identifier_width++; // Takes into account the ending NULL char.

  // Instantiate the new text forward reader.

  text_reader = new observation_file_reader_txt_forward();

  //
  // Set the maximum size of the reader's buffer in BYTES.
  // If not set, the reader will use a default buffer size,
  // that may be checked using query _default_buffer_size()
  //

  text_reader->set_buffer_size(5 * 1024); // Just 5 Kbytes.

  // Try to open the file.

  io_status = text_reader->open(infile_name.c_str());
  if (io_status != 0)
  {
    delete identifier;
    delete the_tags;
    delete the_expectations;
    delete the_covariance_values;
    delete the_par_iids;
    delete the_obs_iids;
    delete the_ins_iids;

    cout << "[ERROR] open (text): " << io_status << endl;
    return 1;
  }

  // Instantiate the new socket writer.

  socket_writer = new observation_socket_writer();

  // Try to open the socket writer.

  io_status = socket_writer->open(server.c_str(), port, false);
  if (io_status != 0)
  {
    delete identifier;
    delete the_tags;
    delete the_expectations;
    delete the_covariance_values;
    delete the_par_iids;
    delete the_obs_iids;
    delete the_ins_iids;

    cout << "[ERROR] open (socket writer): " << io_status << endl;
    return 1;
  }

  //
  // Try to read the whole contents of the text file and, simultaneously,
  // write it to (send it through) the socket writer one.
  //

  while (true)
  {
    // Read the tag type.

    io_status = text_reader->read_type(type);
    if (io_status != 0)
    {
      //
      // read_type is the method that, legally, may return an error
      // code stating that the file is over. Of course, other errors
      // might apppear. However, when an error condition is detected,
      // the first thing to do is check if, in fact, an error condition
      // is active, since this would simply mean that the process
      // of reading the input file is over.
      //

      if (text_reader->is_eof())
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

    io_status = text_reader->read_active_flag(active);
    if (io_status != 0)
    {
      cout << "[ERROR] read_active_flag: " << io_status << endl;
      break;
    }

    // Read the identifier.

    io_status = text_reader->read_identifier(identifier, identifier_width);
    if (io_status != 0)
    {
      cout << "[ERROR] read_identifier: " << io_status << endl;
      break;
    }

    // Only if we are reading an "l" record, read the instance identifier.

    if (type == 'l')
    {
      io_status = text_reader->read_instance_id(instance_identifier);
      if (io_status != 0)
      {
        cout << "[ERROR] read_instance_id: " << io_status << endl;
        break;
      }
    }

    // Read (always) the time tag.

    io_status = text_reader->read_time(time);
    if (io_status != 0)
    {
      cout << "[ERROR] read_time: " << io_status << endl;
      break;
    }

    //
    // Read, depending on the type of the current record, the right
    // type of data. Use our fake RecordStructure class to determine
    // the dimensions of the tags and expectations (l-records) or
    // parameter, observation and instrument instance identifier lists.
    //

    if (type == 'l')
    {
      // Get the dimensions for an l-record whose identifier is identifier.

      record_structure_catalogue.l_record_data(identifier, n_tags, n_expectations);

      //
      // Read the l-record. Note that method read_l_data below will tell us
      // how many covariance values have been found.
      //

      io_status = text_reader->read_l_data(n_tags,              the_tags,
                                           n_expectations,      the_expectations,
                                           n_covariance_values, the_covariance_values);

      if (io_status != 0)
      {
        cout << "[ERROR] read_l_data: " << io_status << endl;
        break;
      }

      // OK, let's write the l-record to the binary file.

      io_status = socket_writer->write_l(active, identifier, instance_identifier, time,
                                         n_tags, the_tags,
                                         n_expectations, the_expectations,
                                         n_covariance_values, the_covariance_values);

      if (io_status != 0)
      {
        cout << "[ERROR] write_l: " << io_status << endl;
        break;
      }
    }
    else // (type == 'o')
    {
      // Get the dimensions for an l-record whose identifier is identifier.

      record_structure_catalogue.o_record_data(identifier, n_par_iids, n_obs_iids, n_ins_iids);

      // Read the l-record.

      io_status = text_reader->read_o_data(n_par_iids, the_par_iids,
                                           n_obs_iids, the_obs_iids,
                                           n_ins_iids, the_ins_iids);
      if (io_status != 0)
      {
        cout << "[ERROR] read_o_data: " << io_status << endl;
        break;
      }

      // OK, let's write the o-record to the binary file.

      io_status = socket_writer->write_o(active, identifier, time,
                                         n_par_iids, the_par_iids,
                                         n_obs_iids, the_obs_iids,
                                         n_ins_iids, the_ins_iids);

      if (io_status != 0)
      {
        cout << "[ERROR] write_o: " << io_status << endl;
        break;
      }
    }
  }

  // Try to close the input file.

  i = 0;

  io_status = text_reader->close();
  if (io_status != 0)
  {
    cout << "[ERROR] close (text): " << io_status << endl;
    i = 1;
  }

  // Try to close the socket writer.

  io_status = socket_writer->close();
  if (io_status != 0)
  {
    cout << "[ERROR] close (socket writer): " << io_status << endl;
    i = 1;
  }

  delete text_reader;
  delete socket_writer;
  delete identifier;
  delete the_tags;
  delete the_expectations;
  delete the_covariance_values;
  delete the_par_iids;
  delete the_obs_iids;
  delete the_ins_iids;

  return i;
}

