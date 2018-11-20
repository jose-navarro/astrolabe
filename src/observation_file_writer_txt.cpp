/** \file observation_file_writer_txt.cpp 
  \brief  Implementation file for observation_file_writer_txt.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "observation_file_writer_txt.hpp"

observation_file_writer_txt::
observation_file_writer_txt
(void)
{
  {
    // Intentionally left blank.
  }
}

observation_file_writer_txt::
~observation_file_writer_txt
(void)
{
  {
    // Intentionally left blank.
  }
}

int
observation_file_writer_txt::
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
    int   bytes_written;
    char  cactive;
    int   i;
    int   n_iidentifiers;
    char* pchar;
    int   size_o_record;
    int   sizeof_identifier;
    int   status;
    int   total_bytes_written;

    // Won't work if the file is closed.

    if (!file_is_open_) return 1;

    //
    // Won't work neither if no l-records have been written yet for the
    // current time tag. This test, however, makes sense only when
    // writing ACTIVE o-records. Inactive ones are always written,
    // no matter what the situation is.
    //

    if (active && (!first_l_written_)) return 3;

    // identifier must contain something.

    if (identifier == NULL) return 4;

    sizeof_identifier = (int)strlen(identifier);
    if (sizeof_identifier < 1) return 4;

    //
    // We'll still refuse to do anything if the number of identifier does not
    // match with what we're expecting.
    //

    if (n_parameter_iids   <= 0) return 7;
    if (n_observation_iids <= 0) return 7;
    if (n_instrument_iids  <  0) return 7;

    //
    // Check if an epoch change is just arriving... Here, the problem
    // of the no l-records written could arise too.
    //
    // Note that this only matters when the record is active. Otherwise,
    // we simply write data but change NO flags at all.
    //

    if (active && (time != last_time_tag_))
    {
      //
      // Ooops! This is the first record in an epoch and we are trying to
      // write an o-record. This is an error!
      //

      return 3;
    }

    //
    // Compute the size needed to store the current o record in the buffer.
    // We'll use the different sizeof_* data members defined and initialized
    // in the constructor.
    //

    n_iidentifiers = n_parameter_iids + n_observation_iids + n_instrument_iids;

    size_o_record = sizeof_fixed_o_                    +
                    sizeof_identifier                  +
                    sizeof_int_value_ * n_iidentifiers +
                    sizeof_closing_tag_;


    //
    // Check if enough room is still left in the data buffer. If not, flush
    // the buffer to disk and start storing data again from its beginning.
    //

    if ((data_buffer_cur_ + size_o_record) > data_buffer_size_)
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
    // - The o tag (including the active / removed attribute),
    //   identifier and time tag first,
    // - the different lists of instance identifiers, using several loops then, and
    // - the closing o tag plus the newline, to finish.
    //

    total_bytes_written = 0;
    pchar = &(data_buffer_[data_buffer_cur_]);

    // o tag, identifier and time tag.

    if (active) cactive = 'a';
    else        cactive = 'r';

    bytes_written = sprintf(pchar, format_fixed_o_, cactive, identifier, time);
    total_bytes_written += bytes_written;
    pchar += bytes_written;

    // The list of parameter instance identifiers

    for (i = 0; i < n_parameter_iids; i++ )
    {
      bytes_written = sprintf(pchar, format_default_int_format_, the_parameter_iids[i]);
      total_bytes_written += bytes_written;
      pchar+= bytes_written;
    }

    // The list of observation instance identifiers

    for (i = 0; i < n_observation_iids; i++ )
    {
      bytes_written = sprintf(pchar, format_default_int_format_, the_observation_iids[i]);
      total_bytes_written += bytes_written;
      pchar+= bytes_written;
    }

    // The list of instrument instance identifiers, if any.

    if (n_instrument_iids > 0)
    {
      for (i = 0; i < n_instrument_iids; i++ )
      {
        bytes_written = sprintf(pchar, format_default_int_format_, the_instrument_iids[i]);
        total_bytes_written += bytes_written;
        pchar+= bytes_written;
      }
    }

    //
    // Closing o tag plus newline. Note that the actual contents of NEWLINE,
    // a macro defined at the header of this class, will depend on the
    // operating system used when compiling this code.
    //

    bytes_written = sprintf(pchar, " </o>%s", NEWLINE);
    total_bytes_written += bytes_written;

    // Update pointers.

    data_buffer_cur_ += total_bytes_written;

    // That's all.

    return 0;
  }
}
