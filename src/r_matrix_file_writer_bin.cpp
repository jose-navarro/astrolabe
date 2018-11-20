/** \file r_matrix_file_writer_bin.cpp 
  \brief Implementation file for r_matrix_file_writer_bin.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_file_writer_bin.hpp"

int
r_matrix_file_writer_bin::
close
(void)
{
  {
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

r_matrix_file_writer_bin::
r_matrix_file_writer_bin
(void)
{
  {
    // Intentionally left blank.
  }
}

r_matrix_file_writer_bin::
~r_matrix_file_writer_bin
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
r_matrix_file_writer_bin::
write_r
(bool    active,
 double  time,
 int     n_correlation_values,
 double* the_correlation_values)
{
  {
    char  cactive;
    int   i;
    char* pchar;
    int   size_b_record;
    int   size_r_record;
    int   size_total;
    int   status;

    // Won't work if the file is closed.

    if (!file_is_open_)      return 1;

    // The number of correlations must be strictly positive.

    if (n_correlation_values <= 0) return 3;

    //
    // Compute the size needed to store the current r record in the buffer.
    // We'll always take into account the backtracking tags since an r-record
    // corresponds to a full epoch, and epochs are the limits where b-records
    // must be written.
    //

    size_b_record = 0; // No backtracking info until detected as needed.

    size_r_record =   sizeof(char)                           // type ('r')
                    + sizeof(char)                           // active flag
                    + sizeof(double)                         // time stamp
                    + sizeof(int)                            // number of correlation values
                    + sizeof(double) * n_correlation_values; // the correlation values.

    size_total = size_r_record;


    //
    // We need to write the backtracking information for the previous
    // sub-block.
    //

    size_b_record =   sizeof(char) // type ('b')
                    + sizeof(int); // total of bytes in block.

    size_total += size_b_record;


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

    //
    // Copy the required info to the data buffer.
    //

    // r-tag

    data_buffer_[data_buffer_cur_] = 'r';
    data_buffer_cur_++;

    // Active flag.

    if (active) cactive = 1;
    else        cactive = 0;

    data_buffer_[data_buffer_cur_] = cactive;
    data_buffer_cur_++;

    // Time stamp.

    pchar = (char*)&time;

    for(i = 0; i < (int)sizeof(double); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // Number of correlation values.

    pchar = (char*)&n_correlation_values;

    for (i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // The correlation values themselves.

    pchar = (char*)the_correlation_values;

    for(i = 0; i < (int)(n_correlation_values *sizeof(double)); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // backtracking tag.

    data_buffer_[data_buffer_cur_] = 'b';
    data_buffer_cur_++;

    //
    // bytes in sub-block. These are exactly the ones taken by a single
    // r-record (in r_matrix files, a full epoch is made of a single
    // r-record).
    //
    pchar = (char*)&size_r_record;

    for(i = 0; i < (int)sizeof(int); i++)
    {
      data_buffer_[data_buffer_cur_] = pchar[i];
      data_buffer_cur_++;
    }

    // That's all.

    return 0;
  }
}
