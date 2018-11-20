/** \file obs_e_based_file_writer_txt.cpp 
  \brief  Implementation file for obs_e_based_file_writer_txt.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "obs_e_based_file_writer_txt.hpp"

int
obs_e_based_file_writer_txt::
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

obs_e_based_file_writer_txt::
obs_e_based_file_writer_txt
(void)
{
  {

    //
    // Instantiate and assign the fixed parts of the l- and o-records
    // formats. Doing this here will reduce drastically the amount of
    // instantiation / destruction operations needed by methods
    // build_l_format() and build_o_format().
    //
    // Note that this is the place to change default formats for
    // integer (o-records) or double (l-records) if ever needed as well
    // as any other format involved in the process or "text writing".
    //
    // WARNING - WARNING - WARNING - WARNING - WARNING - WARNING
    //
    // Do not forget to update the size in bytes that the different
    // part of the l- or o-records will use later on this same method.
    //
    // END OF WARNING - END OF WARNING - END OF WARNING - END OF WARNING
    //

    format_fixed_l_                    = strdup("<l s=\"%c\" id=\"%s\" n=\"%s\"> %23.15le");
    format_fixed_o_                    = strdup("<o s=\"%c\" id=\"%s\"> %23.15le");
    format_default_double_format_      = strdup(" %23.15le");
    format_default_int_format_         = strdup(" %10ld");

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

    sizeof_fixed_l_ = (2 + 1 + 3 + 1 + 1 + 4 + 1 + 1 +3 + 2) + 1 + // Literal characters.
                       1                                         + // s attribute value .
                      23;                                          // Time tag outside the opening tag.

    //
    // Fixed part of the o-record.
    //

    sizeof_fixed_o_ = (2 + 1 + 3 + 1 + 1 + 4 + 2) + 1 + // Literal characters.
                       1                              + // s attribute value.
                      23;                               // Time tag outside the opening tag.

    //
    // Size taken by a single double value (expectation, standard deviation
    // or auxiliar). Includes the whitespace preceding the double value itself.
    //

    sizeof_double_value_ = 1 + 23;

    //
    // Size taken by a single integer value (identifiers). Includes the
    // whitespace preceding the integer value itself.
    //

    sizeof_int_value_ = 1 + 10;

    //
    // Size taken by any (either l or o) tag. Includes the preceding whitespace,
    // the tag itself. The ending new line is also tallied; note that the
    // length of the newline character or characters will depend on the number
    // of these used by the different operating systems. See macro NEWLINE
    // defined in the header of this class.
    //

    sizeof_closing_tag_ = 1 + 4 + sizeof(NEWLINE);
  }
}

obs_e_based_file_writer_txt::
~obs_e_based_file_writer_txt
(void)
{
  {
    // Make sure that the file is closed, flushing any data not yet written.
    if (file_is_open_) close();

    // Destroy buffers (if needed).

    if (data_buffer_ != NULL) delete [] data_buffer_;
    data_buffer_ = NULL;

    free(format_fixed_l_);
    format_fixed_l_ = NULL;

    free(format_fixed_o_);
    format_fixed_o_ = NULL;

    free(format_default_double_format_);
    format_default_double_format_ = NULL;

    free(format_default_int_format_);
    format_default_int_format_ = NULL;
  }
}

int
obs_e_based_file_writer_txt::
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
    int   bytes_written;
    char  cactive;
    int   i;
    char* iidc;
    char  instance_id[20];
    int   n_double_elements;
    char* pchar;
    int   sizeof_identifier;
    int   size_l_record;
    int   status;
    int   total_bytes_written;

    // Won't work if the file is closed.

    if (!file_is_open_)        return 1;

    // identifier must be at least 1 char long excluding the terminating NULL char.

    if (identifier == NULL)    return 4;

    sizeof_identifier = (int)strlen(identifier);
    if (sizeof_identifier < 1) return 4;

    // The dimensions of the several double arrays must be correct.

    if (n_tags < 0)            return 5;
    if (n_expectations <= 0)   return 5;
    if ((n_covariance_values != 0)              &
        (n_covariance_values != n_expectations) &
        (n_covariance_values != (n_expectations*(n_expectations+1))/2))
                               return 5;

    //
    // Check if this is the first time we write to the file. If so, we
    // must give an initial value to the last time tag in use.
    //
    // Note that inactive records do not change flags at all. We
    // simply write to the output file, but it is as if we wouldn't
    // written nothing at all from the flags standpoint.
    //

    if (active && first_time_writing_)
    {
      first_time_writing_ = false;
      last_time_tag_      = time;
    }

    //
    // Compute the size needed to store the current l record in the buffer.
    // We'll use the different sizeof_* data members defined and initialized
    // in the constructor.
    //
    // Note that it is only here that we may measure the size of the instance
    // identifier (we don't want to write it in 10 positions, since it is
    // surrounded by quotes and whitespace would look pretty horrible!).
    //

    iidc = &(instance_id[0]);
    sprintf(iidc, "%d",instance_identifier);

    n_double_elements = n_tags + n_expectations + n_covariance_values;

    size_l_record = sizeof_fixed_l_                          +
                    sizeof_identifier                        +
                    (int)strlen(iidc)                        +
                    sizeof_double_value_ * n_double_elements +
                    sizeof_closing_tag_;


    //
    // Check if an epoch change is just arriving... Inactive records do
    // not change flags.
    //

    if (active && (time != last_time_tag_))
    {
      // We've got a new last_time_tag_ to compare to.

      last_time_tag_ = time;

      //
      // Update the first_l_written_ tag. This is not strictly necessary,
      // since we are writing an l-record here and this means that if we've
      // been the first write_x method called after an epoch change, it is
      // OK (an l-record must be the first one in an epoch). However,
      // the next assignment is made for clarity reasons. Later on, at
      // the end of the method, this flag is set again to its proper value, true,
      // since, obviously, an l-record will have been written as the first
      // one in an epoch.
      //

      first_l_written_ = false;
    }

    //
    // Check if enough room is still left in the data buffer. If not, flush
    // the buffer to disk and start storing data again from its beginning.
    //

    if ((data_buffer_cur_ + size_l_record ) > data_buffer_size_)
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
    // - The l tag (including the active / removed attribute), code, identifier and time tag first,
    // - the tags (if any), expectations and covariance values (again, if any),
    // - the closing l tag plus the newline.
    //

    total_bytes_written = 0;
    pchar = &(data_buffer_[data_buffer_cur_]);

    // l tag, code, identifier + time tag.

    if (active) cactive = 'a';
    else        cactive = 'r';

    bytes_written = sprintf(pchar, format_fixed_l_, cactive, identifier, iidc, time);
    total_bytes_written += bytes_written;
    pchar += bytes_written;

    // Tags (if any)

    for (i = 0; i < n_tags; i++ )
    {
      bytes_written = sprintf(pchar, format_default_double_format_, buffer_tags[i]);
      total_bytes_written += bytes_written;
      pchar+= bytes_written;
    }

    // Expectations

    for (i = 0; i < n_expectations; i++ )
    {
      bytes_written = sprintf(pchar, format_default_double_format_, buffer_expectations[i]);
      total_bytes_written += bytes_written;
      pchar+= bytes_written;
    }

    // Covariance values (if any)

    for (i = 0; i < n_covariance_values; i++ )
    {
      bytes_written = sprintf(pchar, format_default_double_format_, buffer_covariance[i]);
      total_bytes_written += bytes_written;
      pchar+= bytes_written;
    }

    //
    // Closing l tag plus newline. Note that the actual contents of NEWLINE,
    // a macro defined at the header of this class, will depend on the
    // operating system used when compiling this code.
    //

    bytes_written = sprintf(pchar, " </l>%s", NEWLINE);
    total_bytes_written += bytes_written;

    // Update pointers.

    data_buffer_cur_ += total_bytes_written;

    //
    // If we've just written an ACTIVE l-record, we don't need extra
    // l-records the next time data is written, so o-records are legal now.
    // Note that inactive records will never change flags.
    //

    if (active) first_l_written_ = true;

    // That's all.

    return 0;
  }
}
