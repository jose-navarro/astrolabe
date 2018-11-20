/** \file observation_file_reader_bin_forward.cpp 
  \brief Implementation file for observation_file_reader_bin_forward.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "observation_file_reader_bin_forward.hpp"

observation_file_reader_bin_forward::
observation_file_reader_bin_forward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

observation_file_reader_bin_forward::
~observation_file_reader_bin_forward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
observation_file_reader_bin_forward::
read_o_data
(int   n_parameter_iids,
 int*& the_parameter_iids,
 int   n_observation_iids,
 int*& the_observation_iids,
 int   n_instrument_iids,
 int*& the_instrument_iids)
{
  {
    int   bytes_moved;
    int   bytes_to_eat;
    void* destination;
    void* source;
    int   read_status;

    int   n_pars_found;
    int   n_obs_found;
    int   n_inst_found;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Check preconditions. This function works only when the time
    // tag has been read and we are reading an o-type record.
    //

    if (!read_time_)      return 3;
    if ( read_completed_) return 3;
    if (!reading_o_)      return 3;

    // More preconditions: sizes of the output instance identifier arrays.

    if (n_parameter_iids   <= 0) return 6;
    if (n_observation_iids <= 0) return 6;
    if (n_instrument_iids  <  0) return 6;

    //
    // Check if we are going beyond the limits of the data buffer. The
    // theoretical amount of bytes that we need is the sum of all these:
    //
    // - 3 * sizeof(int) (the three counters stating how many identifiers
    //                    of each type we have).
    // - n_parameter_iids * sizeof(int) (the parameter instance identifiers)
    // - n_observation_iids * sizeof(int) (the observation instance identifiers)
    // - n_instrument_iids * sizeof(int) (the instrument instance identifiers).
    //
    // Nonetheless, there may be errors: the calling module may be wrong
    // when stating the number of some kind of instance identifiers, so
    // the computation above may fail.
    //

    bytes_to_eat = (3 + n_parameter_iids + n_observation_iids + n_instrument_iids)  * sizeof(int);

    // Is all this room still available in our buffer? If not, load more data.

    if ((data_buffer_current_ + bytes_to_eat - 1) >= data_buffer_bytes_)
    {
      //
      // Reached the end of the data buffer. Move the remaining bytes at the
      // queue of the data buffer to its begining and read more data from the
      // input file.
      //

      bytes_moved = move_remaining_up();
      read_status = read_next_chunk(bytes_moved);

      if (read_status == 1)
      {
        // End-of-file condition detected.

        is_eof_ = true;
        return 1;
      }
      else if (read_status == 2)
      {
        // I/O error detected.
        return 2;
      }
    }

    //
    // Well, let's get the sizes of the different lists of instance identifiers
    // and these themselves as well.
    //

    //
    // Start with parameters.
    //

    // The number of instance identifiers.

    bytes_to_eat = sizeof(int);
    source       = (void*)&(data_buffer_[data_buffer_current_]);
    destination  = (void*)&n_pars_found;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update the current pointer;

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    // Check coherence.

    if (n_pars_found != n_parameter_iids) return 7;

    // The instance identifiers themselves.

    bytes_to_eat = n_pars_found * sizeof(int);
    source       = (void*)&(data_buffer_[data_buffer_current_]);
    destination  = (void*)the_parameter_iids;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update the current pointer.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    //
    // Now the observations.
    //

    // The number of instance identifiers.

    bytes_to_eat = sizeof(int);
    source       = (void*)&(data_buffer_[data_buffer_current_]);
    destination  = (void*)&n_obs_found;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update the current pointer;

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    // Check coherence.

    if (n_obs_found != n_observation_iids) return 7;

    // The instance identifiers themselves.

    bytes_to_eat = n_obs_found * sizeof(int);
    source       = (void*)&(data_buffer_[data_buffer_current_]);
    destination  = (void*)the_observation_iids;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update the current pointer.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    //
    // Finally, the instruments (if any).
    //

    // The number of instance identifiers.

    bytes_to_eat = sizeof(int);
    source       = (void*)&(data_buffer_[data_buffer_current_]);
    destination  = (void*)&n_inst_found;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update the current pointer;

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    // Check coherence.

    if (n_inst_found != n_instrument_iids) return 7;

    // The instance identifiers themselves, if there are any!!!

    if (n_inst_found > 0)
    {
      bytes_to_eat = n_inst_found * sizeof(int);
      source       = (void*)&(data_buffer_[data_buffer_current_]);
      destination  = (void*)the_instrument_iids;
      memcpy(destination, source, (size_t)bytes_to_eat);

      // Update the current pointer.

      data_buffer_current_ = data_buffer_current_ + bytes_to_eat;
    }

    // Update flags.

    read_type_             = false;
    read_active_flag_      = false;
    read_identifier_       = false;
    read_iid_              = false;
    read_time_             = false;
    read_l_data_           = false;
    read_o_data_           = false;

    reading_l_             = false;
    reading_o_             = false;

    read_completed_        = true;

    // That's all.

    return 0;
  }
}

int
observation_file_reader_bin_forward::
read_time
(double& time)
{
  {
    int   bytes_moved;
    int   bytes_to_eat;
    void* destination;
    void* source;
    int   read_status;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Check preconditions. This function works only in the following
    // situations:
    //
    //   - if reading an l-type record, the instance identifier and no other further
    //     component of the record must have been read.
    //   - if reading an o-type record, the identifier and no other further component
    //     of the record must have been read.
    //

    if (reading_l_)
    {
      if (!read_iid_)        return 3;
      if ( read_l_data_)     return 3;
    }
    else // reading_o_
    {
      if (!read_identifier_) return 3;
      if ( read_o_data_)     return 3;
    }

    //
    // Check if we are going beyond the limits of the data buffer. Note
    // that times are represented using a double.
    //

    bytes_to_eat = sizeof(double);

    if ((data_buffer_current_ + bytes_to_eat - 1) >= data_buffer_bytes_)
    {
      //
      // Reached the end of the data buffer. Move the remaining bytes at the
      // queue of the data buffer to its begining and read more data from the
      // input file.
      //

      bytes_moved = move_remaining_up();
      read_status = read_next_chunk(bytes_moved);

      if (read_status == 1)
      {
        // End-of-file condition detected.

        is_eof_ = true;
        return 1;
      }
      else if (read_status == 2)
      {
        // I/O error detected.
        return 2;
      }
    }

    // Get the time (a double).

    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)&time;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update the current pointer and set flags accordingly.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    read_time_ = true;

    //
    // Update last time / epoch related flags and values.
    //
    // Note that these flags are NOT changed when dealing
    // with INACTIVE records. These are read, but do NOT
    // change the status of the reader concerning epochs.
    //
    // Additionally, when an active record is read and
    // the epoch changes, we must check that we are reading
    // an l-record FIRST. This is the first moment that
    // we can check this...
    //

    if (last_record_is_active_)
    {
      if (last_epoch_time_available_)
      {
        if (last_epoch_time_ != time)
        {
          last_epoch_changed_      = true;
          last_epoch_time_         = time;
        }
        else
        {
          last_epoch_changed_      = false;
        }
      }
      else
      {
        last_epoch_changed_        = false;
        last_epoch_time_           = time;
        last_epoch_time_available_ = true;
      }

      // Whenever the epoch changes, we "loose" our first l-record.

      if (last_epoch_changed_) first_l_read_ = false;

      //
      // Check that when reading an o-record a previous l-record has
      // been obtained.
      //

      if ((last_epoch_changed_) || (reading_first_epoch_))
      {
        if (reading_o_)
        {
          if (!first_l_read_) return 5; // o-records may not start an epoch.
        }
        else // reading an l-record.
        {
          if (!first_l_read_) first_l_read_ = true;
        }
      }

      // If the epoch changes, we will no more be reading the first one.

      if (last_epoch_changed_) reading_first_epoch_ = false;
    }

    // That's all.

    return 0;
  }
}

int
observation_file_reader_bin_forward::
read_type
(char& record_type)
{
  {
    int bytes_for_backtracking;
    int bytes_moved;
    int bytes_to_eat;
    int read_status;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Check preconditions. This function works only if no other
    // read operation is on its way.
    //

    if (!read_completed_) return 3;

    //
    // Check if we are going beyond the limits of the data buffer.
    //
    // Note that a tag indicating that backtracking information follows might
    // appear now. Backtracking information is used by backwards readers, and
    // it is of no interest for forward readers (our case now), so it should
    // be skipped when found.
    //
    // Backtracking information consist of a single character tag (containing
    // either 'b' or 'B') plus an integer field containing byte counts.
    //
    // So, if backtracking information is on its way, we should be able to
    // analize, a sequence like the following one:
    //
    //   backtrack tag ('b' or 'B') +
    //   backtrack info (1 int)
    //
    // Obviously, if the next char in the buffer is just a "real" type tag
    // ('l' or 'o', no matter the lettercase), we'll need only a char to
    // decide what comes next, but it is harmless to ask for more.
    //
    // So to cover the backtracking situation we'll ask the buffer to guarantee
    // that an (hypothetical) backtrack tag + an (hypothetical) backtrack info
    // items are available.
    //
    // That's why the next sentence tries to guarantee that at least
    // sizeof(char) + sizeof(int) bytes are at hand in the data buffer.
    //
    // Note, however, that if we are at the end of the file, no "real" tag
    // will follow the backtracking information block. This means that
    // an eventual buffer reload will not read beyond this bactracking block
    // since no more information will be available. That's why this method
    // checks TWICE if the buffer is exhausted...
    //

    bytes_to_eat           = sizeof(char);// Space needed by a single type tag.
    bytes_for_backtracking = sizeof(int); // The backtracking info, when present.

    if ((data_buffer_current_ + bytes_to_eat + bytes_for_backtracking -1)
         >=
         data_buffer_bytes_)
    {
      //
      // Reached the end of the data buffer. Move the remaining bytes at the
      // queue of the data buffer to its begining and read more data from the
      // input file.
      //

      bytes_moved = move_remaining_up();
      read_status = read_next_chunk(bytes_moved);

      if (read_status == 1)
      {
        // End-of-file condition detected. Not legal at this point!

        is_eof_ = true;
        return 5;
      }
      else if (read_status == 2)
      {
        // I/O error detected.
        return 2;
      }
    }

    // Get the tag and see if it is a backtracking one.

    record_type = data_buffer_[data_buffer_current_];

    if (record_type == 'b')
    {
      //
      // We've found the backtracking tag + info block. Let's skip it
      // updating the current pointer as many positions to the right
      // as bytes use the whole backtracking information block.
      //

      data_buffer_current_ += bytes_to_eat + bytes_for_backtracking;
    }

    //
    // Check again (see the discussion above about end-of-file conditions)
    // if the buffer holds enough information as to include the "real" type
    // tag.
    //

    bytes_to_eat = sizeof(char);

    if ((data_buffer_current_ + bytes_to_eat -1) >= data_buffer_bytes_)
    {
      //
      // Reached the end of the data buffer. Move the remaining bytes at the
      // queue of the data buffer to its begining and read more data from the
      // input file.
      //

      bytes_moved = move_remaining_up();
      read_status = read_next_chunk(bytes_moved);

      if (read_status == 1)
      {
        // End-of-file condition detected.

        is_eof_ = true;
        return 1;
      }
      else if (read_status == 2)
      {
        // I/O error detected.
        return 2;
      }
    }

    //
    // There was more information in the file. So we can proceed to process
    // the "real" type tag.
    //

    record_type = data_buffer_[data_buffer_current_];

    // Check that a correct type tag has been read.

    if ((record_type != 'o') & (record_type != 'l')) return 4;

    // Update the current pointer and set flags accordingly.

    data_buffer_current_ += bytes_to_eat;


    if (record_type == 'o')
    {
      reading_o_ = true;
      reading_l_ = false;
    }
    else
    {
      reading_o_ = false;
      reading_l_ = true;
    }

    read_completed_        = false;
    read_type_             = true;

    // That's all.

    return 0;
  }
}
