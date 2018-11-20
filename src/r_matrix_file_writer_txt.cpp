/** \file r_matrix_file_writer_txt.cpp 
  \brief  Implementation file for r_matrix_file_writer_txt.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_file_writer_txt.hpp"

int
r_matrix_file_writer_txt::
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

r_matrix_file_writer_txt::
r_matrix_file_writer_txt
(void)
{
  {

    //
    // Instantiate and assign the fixed parts of the r-records
    // formats. Doing this here will reduce drastically the amount of
    // instantiation / destruction operations needed by methods
    // build_r_format().
    //
    // Note that this is the place to change default formats for
    // double values - if ever needed - as well as any other format
    // involved in the process or "text writing".
    //
    // WARNING - WARNING - WARNING - WARNING - WARNING - WARNING
    //
    // Do not forget to update the size in bytes that the different
    // part of the l- or o-records will use later on this same method.
    //
    // END OF WARNING - END OF WARNING - END OF WARNING - END OF WARNING
    //

    format_fixed_r_                    = strdup("<r s=\"%c\"> %23.15le");
    format_default_double_format_      = strdup(" %23.15le");

    //
    // Compute the actual size that different types of data will use when
    // written in text format.
    //
    // WARNING - WARNING - WARNING - WARNING - WARNING - WARNING
    //
    // These sizes must be set taking into acount the formats defined above!!!
    //
    // END OF WARNING - END OF WARNING - END OF WARNING - END OF WARNING
    //
    // These sizes are used to compute how many characters will need a new
    // output l- or o-record and compare this amount to the remaining
    // space still left in the I/O buffer and the make possible to decide
    // if an I/O flush operation is needed.
    //
    // The different size elements are explained below, when these are
    // computed. Each description includes the different components taken
    // into account to perform the computation.
    //

    //
    // Fixed par of the l-record.
    //

    sizeof_fixed_r_ = (2 + 1 + 3 + 2) + 1 + // Literal characters.
                       1                  + // s attribute value.
                      23;                   // Time tag outside the opening tag.

    //
    // Size taken by a single double value (correlation values)
    // Includes the whitespace preceding the double value itself.
    //

    sizeof_double_value_ = 1 + 23;

    //
    // Size taken by any the r tag. Includes the preceding whitespace,
    // the tag itself. The ending new line is also tallied; note that the
    // length of the newline character or characters will depend on the number
    // of these used by the different operating systems. See macro NEWLINE
    // defined in the header of this class.
    //

    sizeof_closing_tag_ = 1 + 4 + sizeof(NEWLINE);
  }
}

r_matrix_file_writer_txt::
~r_matrix_file_writer_txt
(void)
{
  {
    // Make sure that the file is closed, flushing any data not yet written.
    if (file_is_open_) close();

    // Destroy buffers (if needed).

    if (data_buffer_ != NULL) delete [] data_buffer_;
    data_buffer_ = NULL;

    free(format_fixed_r_);
    format_fixed_r_ = NULL;

    free(format_default_double_format_);
    format_default_double_format_ = NULL;
  }
}

int
r_matrix_file_writer_txt::
write_r
(bool     active,
 double   time,
 int      n_correlation_values,
 double*  the_correlation_values)
{
  {
    int   bytes_written;
    char  cactive;
    int   i;
    char* pchar;
    int   size_r_record;
    int   status;
    int   total_bytes_written;

    // Won't work if the file is closed.

    if (!file_is_open_)            return 1;

    // The number of correlations must be strictly positive.

    if (n_correlation_values <= 0) return 3;

    //
    // Compute the size needed to store the current r record in the buffer.
    // We'll use the sizeof_double data member defined and initialized
    // in the constructor.
    //


    size_r_record = sizeof_fixed_r_                             +
                    sizeof_double_value_ * n_correlation_values +
                    sizeof_closing_tag_;

    //
    // Check if enough room is still left in the data buffer. If not, flush
    // the buffer to disk and start storing data again from its beginning.
    //

    if ((data_buffer_cur_ + size_r_record ) > data_buffer_size_)
    {
      status = flush();

      // I/O error if status is not 0.

      if (status != 0) return 2;
    }

    //
    // "Print" to the data buffer the required info.
    //
    // This is done in several steps:
    //
    // - The r tag (including the active / removed attribute) and time tag first,
    // - the correlation values and
    // - the closing r tag plus the newline.
    //

    total_bytes_written = 0;
    pchar = &(data_buffer_[data_buffer_cur_]);

    // r tag, active attribute and time tag.

    if (active) cactive = 'a';
    else        cactive = 'r';

    bytes_written = sprintf(pchar, format_fixed_r_, cactive, time);
    total_bytes_written += bytes_written;
    pchar += bytes_written;

    // Correlation values

    for (i = 0; i < n_correlation_values; i++ )
    {
      bytes_written = sprintf(pchar, format_default_double_format_, the_correlation_values[i]);
      total_bytes_written += bytes_written;
      pchar+= bytes_written;
    }

    //
    // Closing r tag plus newline. Note that the actual contents of NEWLINE,
    // a macro defined at the header of this class, will depend on the
    // operating system used when compiling this code.
    //

    bytes_written = sprintf(pchar, " </r>%s", NEWLINE);
    total_bytes_written += bytes_written;

    // Update pointers.

    data_buffer_cur_ += total_bytes_written;

    // That's all.

    return 0;
  }
}
