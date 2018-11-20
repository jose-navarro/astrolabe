#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "observation_file_writer_bin.hpp"
#include "segmented_file_utils.hpp"

using namespace std;

bool
next_id
(bool first_time, int max_size, char* id, bool& end_block)
{
  {
    int i, j;

    end_block = false;

    if (first_time)
    {
        // First ID is "AA...A";

      for (i = 0; i < max_size; i++) id[i] = 65;

      return true;
    }

    for (i = max_size-1; i >= 0; i--)
    {
      id[i] = id[i] + 1;

      if (id[max_size - 1] == 90) end_block = true;

      if (id[i] <= 90) break;

      if (i == 0) return false;

      for (j = i; j < max_size; j++) id[j] = 65;
    }

    return true;
  }
}

int
main
(int argv, char** argc)
{
  {
    bool     active;
    char     cactive;
    string   chunk_filename;
    int      chunk_number;
    char*    code;
    int      code_width;
    bool     end_block;
    bool     force_active;
    bool     force_l;
    int      i;
    int      line_index;
    int      lines_per_chunk[3];
    string   outfile_name;
    string   outfile_txt_name;
    int      random_value;
    int      io_status;
    double   time_tag;
    int      total_ids;
    int      total_lines;
    int      total_tags;
    ofstream txt_file;
    char*    txt_line;
    double*  val_d;
    int*     val_i;
    bool     write_l;

    observation_file_writer_bin* bwriter;
    segmented_file_utils         sfu;

    // Get the name of the output file.

    cout << "Name of the output synthetic binary file: ";
    cin >> outfile_name;

    cout << "Name of the output, equivalent text file: ";
    cin >> outfile_txt_name;

    // Prepare the chunk names generator.

    sfu.set_mode_write(outfile_name, 1);

    // Open the text file, used for visual validation purposes only.

    txt_line = new char[200];
    txt_file.open(outfile_txt_name);

    // Assign / instantiate some values.

    code_width           = 3;

    code                 = new char[code_width + 1];
    code[code_width]     = 0;
    total_ids            = 0;
    time_tag             = 1.0;
    total_tags           = 0;
    val_d                = new double[4];
    val_i                = new int[3];
    write_l              = false;

    // The number of events (l or o) to create is 26^code_width.

    total_lines = (int)pow(26.0, code_width);

    //
    // We want to split these lines in three files. Compute the
    // approximate number of lines (always a multiple of 26) for
    // every chunk. All chunks will have the same number of lines
    // but the last one.
    //

    i = ((total_lines / 3) / 26 ) * 26;

    lines_per_chunk[0] = i;
    lines_per_chunk[1] = 2*i;
    lines_per_chunk[2] = total_lines;

    // Iterate as many times as chunks we want to create.

    line_index = 0;

    for (chunk_number = 0; chunk_number < 3; chunk_number++)
    {

      sfu.get_next_filename(chunk_filename);

      // Instantiate the binary writer.

      try
      {
        bwriter = new observation_file_writer_bin();
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

      bwriter->set_buffer_size(5 * 1024); // 5 Kbyte.

      // Open the underlying file.

      io_status = bwriter->open(chunk_filename.c_str());
      if (io_status != 0)
      {
        cout << "Error opening output file. Error code in open(): " << io_status << endl;
        return 1;
      }

      force_l = true; // Beginning of chunk and beginning of epoch.

      while (line_index < lines_per_chunk[chunk_number])
      {

        // Get a fake code

        next_id((line_index == 0), code_width, code, end_block);

        //
        // Decide (randomly) whether to write an o or an l record. Note that when
        // starting a new block l's are always written first and with an active
        // status.
        //

        if (force_l)
        {
          write_l = true;
          force_active = true;
          active = true;
        }
        else
        {
          random_value = rand() % 100 + 1;
          write_l = (random_value <= 50);

          if (!force_active)
          {
            random_value = rand() % 100 + 1;
            active = (random_value <= 95);
          }
        }

        //
        // Write either an l or o record depending on what's been decided
        // just above.
        //

        if (write_l)
        {
          //
          // Write a synthetic l record.
          //

          //
          // Generate synthetic values for the o data. We'll use the same array
          // for the tags (2 values), the observation (1 value) and the covariances
          // (just 1 value, a standard deviation).
          //

          for (i =0; i < 4; i++) val_d[i] = total_ids + i;

          io_status = bwriter->write_l(active, code, total_ids, time_tag, 2, val_d,
                                                                          1, &(val_d[2]),
                                                                          1, &(val_d[3]));
          if (io_status != 0)
          {
            cout << "Error writing l record. Error code in write_l(): " << io_status << endl;
            break;
          }

          // Write the same data to the text validation file.

          if (active) cactive = 'a';
          else        cactive = 'r';

          sprintf(txt_line, "<l l_id=\"%s\" s=\"%c\"> %10ld %18e %15e %15e %15e %15e </l>\n",
                  code, cactive, total_ids, time_tag, val_d[0], val_d[1], val_d[2], val_d[3]);
          txt_file << txt_line;
        }
        else
        {
          //
          // Write a synthetic o record.
          //

          //
          // Generate synthetic values for the o data. Just one
          // identifier for parameters and another for observationsand instruments.
          // We'll use the same array.
          //

          for (i =0; i < 3; i++) val_i[i] = total_ids + i;

          // Write!

          io_status = bwriter->write_o(active, code, time_tag, 1, val_i,
                                                               1, &(val_i[1]),
                                                               1, &(val_i[2]));
          if (io_status != 0)
          {
            cout << "Error writing o record. Error code in write_o(): " << io_status << endl;
            break;
          }

          // Write the same data to the text validation file.

          if (active) cactive = 'a';
          else        cactive = 'r';

          sprintf(txt_line, "<o o_id=\"%s\" s=\"%c\"> %18e %10ld %10ld %10ld </o>\n",
                  code, cactive, time_tag, val_i[0], val_i[1], val_i[2]);
          txt_file << txt_line;
        }

        force_l = false;
        force_active = false;

        if (end_block)
        {
          time_tag ++;
          force_l = true;
          total_tags++;
        }

        total_ids++;

        line_index++;
      }

      // Close the writer.

      io_status =  bwriter->close();
      if (io_status != 0)
      {
        cout << "Error closing file. Error code in close(): " << io_status;
        delete bwriter;
        bwriter = NULL;
        return 1;
      }
      delete bwriter;

    }

    // Close the text validation file.

    txt_file.close();
    delete txt_line;

    // Remove garbage.

    delete code;
    delete val_d;
    delete val_i;

    cout << "Total IDs written: "   << total_ids << endl;
    cout << "Total tags written: "  << total_tags << endl;

  }

  return 0;
}
