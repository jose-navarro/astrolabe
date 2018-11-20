/** \file obs_e_based_file_writer_bin.cpp 
  \brief Implementation file for obs_e_based_file_writer_bin.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "obs_e_based_file_writer_bin.hpp"

int
obs_e_based_file_writer_bin::
close
(void)
{
  {
    int   i;
    char* pchar;
    int   status;

    // Do not complain if the file is already closed.
    if (!file_is_open_) return 0;

    // If there's still data in the buffer, dump it to disk.

    status = flush();

    if (status != 0)
    {
      fclose(file_file_);
      return 2;
    }

    // We must write the last sub-block tag

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

    status = flush();

    if (status != 0)
    {
      fclose(file_file_);
      return 2;
    }

    // Close the file.

    status = fclose(file_file_);
    if (status != 0) return 1;

    //
    // The file is closed now. Update the corresponding status members and
    // destroy buffers.
    //

    delete [] data_buffer_;
    data_buffer_ = NULL;

    file_is_open_ = false;

    // Successful completion.

    return 0;
  }
}

obs_e_based_file_writer_bin::
obs_e_based_file_writer_bin
(void)
{
  {
    bytes_in_current_block_ = 0;
  }
}

obs_e_based_file_writer_bin::
~obs_e_based_file_writer_bin
(void)
{
  {
    // Make sure that the file is closed, flushing any data not yet written.
    if (file_is_open_) close();

    // Destroy buffers (if needed).

    if (data_buffer_ != NULL) delete [] data_buffer_;
    data_buffer_ = NULL;
  }
}

int
obs_e_based_file_writer_bin::
write_l
(bool         active,
 char*        identifier,
 int          instance_identifier,
 double       time,
 int          n_tags,
 double*      buffer_tags,
 int          n_expectations,
 double*      buffer_expectations,
 int          n_covariance_values,
 double*      buffer_covariance)
{
  {
    char  cactive;
    int   i;
    char* pchar;
    int   size_b_record;
    int   size_identifier;
    int   size_l_record;
    int   size_total;
    int   status;
    bool  write_backtrack_info;

    // Won't work if the file is closed.

    if (!file_is_open_)      return 1;

    // code must be at least 1 chars long excluding the terminating NULL char.

    if (identifier == NULL)  return 4;

    size_identifier = (int)strlen(identifier);

    if (size_identifier < 1) return 4;

    // The dimensions of the several double arrays must be correct.

    if (n_tags < 0)          return 5;
    if (n_expectations <= 0) return 5;
    if ((n_covariance_values != 0)              &
        (n_covariance_values != n_expectations) &
        (n_covariance_values != (n_expectations*(n_expectations+1))/2))
                             return 5;

    //
    // Check if this is the first time we write to the file. If so, we
    // must give an initial value to the last time tag in use.
    //

    if (first_time_writing_)
    {
      first_time_writing_ = false;
      last_time_tag_      = time;
    }

    //
    // Compute the size needed to store the current l record in the buffer.
    // We'll take into account the backtracking tags only when a sub-block
    // (set of records with the same time tag) has just finished.
    //

    size_b_record = 0; // No backtracking info until detected as needed.

    size_l_record =   sizeof(char)                          // type
                    + sizeof(char)                          // active flag
                    + sizeof(int)                           // length in chars of the identifier
                    + sizeof(char) * size_identifier        // identifier
                    + sizeof(int)                           // instance identifier
                    + sizeof(double)                        // time stamp
                    + sizeof(int)                           // number of tags
                    + sizeof(double) * n_tags               // the tags
                    + sizeof(int)                           // number of expectations
                    + sizeof(double) * n_expectations       // the expectations
                    + sizeof(int)                           // number of covariance values
                    + sizeof(double) * n_covariance_values; // the covariance values.

    size_total = size_l_record;

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

      // We must write the backtrack info.

      write_backtrack_info = true;

      // We've got a new last_time_tag_ to compare to.

      last_time_tag_ = time;
    }

    //
    // Check if enough room is still left in the data buffer. If not, flush
    // the buffer to disk and start storing data again from its beginning.
    //

    if ((data_buffer_cur_ + size_total ) > data_buffer_size_)
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

    // l-tag

    data_buffer_[data_buffer_cur_] = 'l';
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

    // Instance identifier.

    pchar = (char*)&instance_identifier;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // Time stamp.

    pchar = (char*)&time;

    for(i = 0; i < (int)sizeof(double); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // Number of tags.

    pchar = (char*)&n_tags;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The tags themselves.

    pchar = (char*)buffer_tags;

    for(i = 0; i < (int)(n_tags *sizeof(double)); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // Number of expectations.

    pchar = (char*)&n_expectations;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The expectations themselves.

    pchar = (char*)buffer_expectations;

    for(i = 0; i < (int)(n_expectations *sizeof(double)); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // Number of covariance matrix elements.

    pchar = (char*)&n_covariance_values;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The covariance matrix values themselves.

    pchar = (char*)buffer_covariance;

    for(i = 0; i < (int)(n_covariance_values *sizeof(double)); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // Update the total number of bytes in this block.

    bytes_in_current_block_ += size_l_record;

    //
    // We don't need an l-record the next time data is written.
    // Note that inactive records DO NOT CHANGE the situation,
    // since, although written, these records do not count as
    // "real" ones.
    //

    if (active) first_l_written_ = true;

    // That's all.

    return 0;
  }
}
