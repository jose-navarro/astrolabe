/** \file observation_file_writer_bin.cpp 
  \brief Implementation file for observation_file_writer_bin.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "observation_file_writer_bin.hpp"


observation_file_writer_bin::
observation_file_writer_bin
(void)
{
  {
    // Intentionally left blank.
  }
}

observation_file_writer_bin::
~observation_file_writer_bin
(void)
{
  {
    // Intentionally left blank.
  }
}

int
observation_file_writer_bin::
write_o
(bool         active,
 char*        identifier,
 double       time,
 int          n_parameter_iids,
 int*         the_parameter_iids,
 int          n_observation_iids,
 int*         the_observation_iids,
 int          n_instrument_iids,
 int*         the_instrument_iids)
{
  {
    char  cactive;
    int   i;
    int   n_iidentifiers;
    char* pchar;
    int   size_b_record;
    int   size_identifier;
    int   size_o_record;
    int   size_total;
    int   status;
    bool  write_backtrack_info;

    // Won't work if the file is closed.

    if (!file_is_open_) return 1;

    // identifier must be at least 1 chars long excluding the terminating NULL char.

    if (identifier == NULL) return 4;

    size_identifier = (int)strlen(identifier);

    if (size_identifier < 1) return 4;

    //
    // Won't work neither if no l-records have been written yet for the
    // current time tag.
    //

    if (!first_l_written_) return 3;

    //
    // We'll still refuse to do anything if the number of identifier does not
    // match with what we're expecting.
    //

    if (n_parameter_iids   <= 0) return 7;
    if (n_observation_iids <= 0) return 7;
    if (n_instrument_iids  <  0) return 7;

    //
    // Compute the size needed to store the current o record in the buffer.
    // We'll take into account the backtracking tags only when a sub-block
    // (set of records with the same time tag) has just finished.
    //

    size_b_record = 0; // No backtracking info until detected as needed.

    n_iidentifiers = n_parameter_iids + n_observation_iids + n_instrument_iids;

    size_o_record =   sizeof(char)                        // type
                    + sizeof(char)                        // active flag
                    + sizeof(int)                         // length in chars of the identifier
                    + sizeof(char) * size_identifier      // identifier
                    + sizeof(double)                      // time tag
                    + sizeof(int) * 3                     // The number of parameter, observation
                                                          // and instrument instance identifiers
                    + sizeof(int) * n_iidentifiers;       // The whole set of instance identifiers

    size_total = size_o_record;

    write_backtrack_info = false;

    if (time != last_time_tag_)
    {
      //
      // We need to write the backtracking information for the previous
      // sub-block.
      //

      size_b_record =   sizeof(char) // type ('b')
                      + sizeof(int); // total of bytes in block.

      size_total += size_b_record;

      // We must write the backtracking info.

      write_backtrack_info = true;

      // We've got a new last_time_tag_ to compare to.

      last_time_tag_ = time;
    }

    //
    // Check if enough room is still left in the data buffer. If not, flush
    // the buffer to disk and start storing data again from its beginning.
    //

    if ((data_buffer_cur_ + size_total) > data_buffer_size_)
    {
      status = flush();

      // I/O error if status is not 0.

      if (status != 0) return 2;
    }

    // Copy to the data buffer the required info.

    if (write_backtrack_info)
    {
      // We must dump last sub-block backtracking data.

      // backtracking tag.
      data_buffer_[data_buffer_cur_] = 'b';
      data_buffer_cur_++;

      // bytes in sub-block.
      pchar = (char*)&bytes_in_current_block_;

      for(i = 0; i < (int)sizeof(int); i++)
      {
        data_buffer_[data_buffer_cur_] = pchar[i];
        data_buffer_cur_++;
      }

      // Reset the number of bytes in the current sub-block.

      bytes_in_current_block_ = 0;

      // We'll expect an l-record the next time a write operation is called.

      first_l_written_ = false;
    }

    // o-tag

    data_buffer_[data_buffer_cur_] = 'o';
    data_buffer_cur_++;

    // Active flag.

    if (active) cactive = 1;
    else        cactive = 0;

    data_buffer_[data_buffer_cur_] = cactive;
    data_buffer_cur_++;

    // Length of the identifier.

    pchar = (char*)&size_identifier;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    //
    // The identifier itself (variable length, that's why we wrote its
    // length before).
    //

    for (i = 0; i < size_identifier; i++)
    {
      data_buffer_[data_buffer_cur_] = identifier[i];
      data_buffer_cur_++;
    }

    // Time tag.

    pchar = (char*)&time;

    for(i = 0; i < (int)sizeof(double); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    //
    // Instance identifiers. First, parameters, then observations and,
    // finally, instruments (if any, these are optional).
    //
    // For each type of instance identifiers we'll write first the
    // counter (stating how may instance identifiers will follow) and
    // the instance identifiers themselves.
    //

    // The counter (parameters)

    pchar = (char*)&n_parameter_iids;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The instance identifiers (parameters)

    pchar = (char*)the_parameter_iids;

    for(i = 0; i < (int)(n_parameter_iids *sizeof(int)); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The counter (observations)

    pchar = (char*)&n_observation_iids;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The instance identifiers (observations)

    pchar = (char*)the_observation_iids;

    for(i = 0; i < (int)(n_observation_iids *sizeof(int)); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The counter (instruments)

    pchar = (char*)&n_instrument_iids;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The instance identifiers (instruments). IF ANY.

    if (n_instrument_iids > 0)
    {
      pchar = (char*)the_instrument_iids;

      for(i = 0; i < (int)(n_instrument_iids *sizeof(int)); i++)
      {
        data_buffer_[data_buffer_cur_] = pchar[i];
        data_buffer_cur_++;
      }
    }

    // Update the total number of bytes in this block.

    bytes_in_current_block_ += size_o_record;

    // That's all.

    return 0;
  }
}
