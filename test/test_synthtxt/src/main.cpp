#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "RecordStructure.hpp"
#include "RecordSequence.hpp"

#include "observation_file_writer_txt.hpp"

using namespace std;

int
main
(int argc, char** argv)
{
  { 
    // The name of the output file.

    string  outfile_name;

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

    //
    // Data arrays for o-records. Related
    // dimensions.
    //

    int*    the_par_iids;
    int*    the_obs_iids;
    int*    the_ins_iids;

    int     the_par_size;
    int     the_obs_size;
    int     the_ins_size;

    int     n_par_iids;
    int     n_obs_iids;
    int     n_ins_iids;

    // Metadata to generate fake l- and o- records.

    string  sid;
    char*   identifier;
    int     identifier_width;
    double  time_tag;
    int     record_type;
    int     record_id_index;
    int     cycle;

    // The ASTROLABLE observation text writer.

    observation_file_writer_txt* writer;

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

    //
    // Check the number of parameters. We need at least one (besides
    // the name of the program).
    //

    if (argc < 2)
    {
      cout << "usage: test_synthtxt output_txt_file_name" << endl;
      return 1;
    }

    outfile_name = argv[1];

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

    identifier[identifier_width] = 0;
    time_tag                     = 1.0;


    // Instantiate the text writer.

    try
    {
      writer = new observation_file_writer_txt();
    }
    catch (...)
    {
      cout << "Error instantiating writer. Not enough memory" << endl;
      return 1;
    }

    //
    // Set the size in bytes of the buffer used by the binary writer. Note,
    // however, that it is not strictly necessary to set this size at all;
    // if no size is set (that is, the next method call is not made)
    // the writer will use a default (internally set) size that may be
    // queried using method _default_buffer_size().
    //

    writer->set_buffer_size(5 * 1024); // 5 Kbyte.

    // Open the underlying file.

    io_status = writer->open(outfile_name.c_str());
    if (io_status != 0)
    {
      cout << "Error opening output file. Error code in open(): " << io_status << endl;
      return 1;
    }

    //
    // Insert 1000 full cycles of the sequence of records provided by
    // our RecordSequence object. Each cycle corresponds to a full epoch.
    //

    total_lines = 1000 * the_record_sequence.cycle_length();

    for (line_index = 0; line_index < total_lines; line_index++)
    {

      //
      // Get the type of record to generate as well as its identifier
      // index.
      //

      the_record_sequence.get_next(record_type, record_id_index, cycle);

      time_tag = (double)cycle;

      //
      // Retrieve the structural information for the kind of record
      // just generated. Then, generate fake data for the proper
      // record type and write it to the output file.
      //

      if (record_type == 1)
      {
        // This is an l-record.

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

        io_status = writer->write_l (true, identifier, line_index, time_tag,
                                   n_tags,              the_tags,
                                   n_expectations,      the_expectations,
                                   n_covariance_values, the_covariance_values);
        if (io_status != 0)
        {
          cout << "Error writing l record. Error code in write_l(): " << io_status << endl;
          break;
        }

      }
      else
      {
        // record_type == 0, meaning an o-record.

        record_structure_catalogue.o_record_data(record_id_index, sid,
                                                 n_par_iids, n_obs_iids, n_ins_iids);

        // Identifier.

        identifier = strcpy(identifier, sid.c_str());

        // Generate fake parameter, observation and instrument instance identifiers.

        for (i = 0; i < n_par_iids; i++) the_par_iids[i] = i + line_index;
        for (i = 0; i < n_obs_iids; i++) the_obs_iids[i] = i + line_index + 1;
        for (i = 0; i < n_ins_iids; i++) the_ins_iids[i] = i + line_index + 2;

        // Write the o-record.

        io_status = writer->write_o(true, identifier, time_tag,
                                  n_par_iids, the_par_iids,
                                  n_obs_iids, the_obs_iids,
                                  n_ins_iids, the_ins_iids);
        if (io_status != 0)
        {
          cout << "Error writing o record. Error code in write_o(): " << io_status << endl;
          break;
        }
      }
    }


    // Close the writer.

    io_status =  writer->close();
    if (io_status != 0)
    {
      cout << "Error closing file. Error code in close(): " << io_status;
      delete writer;
      writer = NULL;
      return 1;
    }

    delete identifier;

    delete writer;


  }

  return 0;
}
