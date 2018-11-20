/** \file observation_file_reader_bin_backward.cpp 
  \brief Implementation file for observation_file_reader_bin_backward.cpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "observation_file_reader_bin_backward.hpp"

observation_file_reader_bin_backward::
observation_file_reader_bin_backward
(void)
{
  {
    epoch_cur_backtrack_index_ = -1;
    epoch_cur_offset_          = -1;
    epoch_remaining_bytes_     = -1;
    chunk_start_index_         = -1;
    file_bytes_unread_         =  0;
  }
}

observation_file_reader_bin_backward::
~observation_file_reader_bin_backward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
observation_file_reader_bin_backward::
open
(const char* file_name)
{
  {
    int read_status;

    // Won't accept open requests if the file is already open.

    if (file_is_open_) return 1;

    // Try to open the file.

    file_file_ = fopen(file_name, "rb");

    // If the open command failed, report that and go away.

    if (file_file_ == NULL) return 2;

    // Ascertain how big is the file.

    if (fseek(file_file_, 0 , SEEK_END) != 0)
    {
      return 5; // I/O error.
    }

    file_size_         = ftell(file_file_);
    file_bytes_unread_ = file_size_;

    // Try to create the buffer used for buffered reads.

    try
    {
      //
      // Instantiate a data buffer with the proper size. If the size of the
      // file is smaller than the maximum authorized size, then data_buffer_size_
      // will be readjusted to use less memory. Otherwise, the requested
      // data buffer size will be preserved.
      //
      // If everything works, the file is reported as open.
      //

      if (file_size_ < data_buffer_size_) data_buffer_size_ = file_size_;

      data_buffer_ = new char[data_buffer_size_];
      file_is_open_ = true;
    }
    catch (...)
    {
      //
      // If there's not enough memory, close again the file, flag it
      // as not open and return the error code.
      //

      close();
      file_is_open_ = false;
      return 3;
    }

    //
    // Read as much bytes as possible (maximum: data_buffer_size_)
    // from the underlying binary file.
    // Since this is the first read operation, we must guarantee that
    // the fseek pointer indicates the end of the file.
    //

    fseek(file_file_, 0, SEEK_END);

    read_status = read_prev_chunk();


    // Check errors.

    if (read_status == 1) return 4; // End of file just starting: empty file.

    if (read_status == 2) return 5; // I/O error.

    // Set the name of the file just open.

    file_name_ = strdup(file_name);

    // Successful completion.

    return 0;
  }
}

int
observation_file_reader_bin_backward::
read_active_flag
(bool& active)
{
  {
    int  bytes_to_eat;
    char cactive;

    //
    // Check preconditions. This function works only when (1) the type
    // tag has been read and (2) no other component in the record has
    // been read neither.
    //

    if (!read_type_)        return 3;
    if ( read_active_flag_) return 3;

    ///Get the active flag in char form.

    bytes_to_eat = sizeof(char);
    cactive       = data_buffer_[epoch_cur_offset_];

    active = (cactive == 1);

    // Update last record active flag.

    last_record_is_active_ = active;

    // Update pointers, sizes and flags.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    read_active_flag_ = true;

    // That's all.

    return 0;
  }
}

int
observation_file_reader_bin_backward::
read_identifier
(char*& identifier,
 int    id_len)
{
  {
    int   bytes_to_eat;
    void* destination;
    int   sizeof_identifier;
    void* source;

    //
    // Check preconditions. This function works only when (1) the active
    // flag has been read, (2) no other component in the record has
    // been read neither, (3) the buffer for the identifier is non-null.
    //

    if (!read_active_flag_) return 3;
    if ( read_identifier_)  return 3;
    if (identifier == NULL) return 4;

    //
    // Retrieve the size of the identifier stored in the input file.
    // It is an int value.
    //

    bytes_to_eat = (int) (sizeof(int));

    source      = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination = (void*)&sizeof_identifier;

    memcpy(destination, source, sizeof(int));

    // Update pointers and sizes.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    //
    // If the size of the identifier in the input file exceeds that of the
    // identifier parameter, we must report an error.
    //

    if (id_len <= sizeof_identifier) return 4;

    //
    // Get the identifier (sizeof_identifier chars). We are sure that we
    // have data, since the type has already been read and read_type()
    // guarantees that.
    //

    bytes_to_eat = sizeof_identifier * sizeof(char);

    source      = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination = (void*)identifier;

    memcpy(destination, source, (size_t)bytes_to_eat);
    identifier[bytes_to_eat] = 0; // Ending NULL character.

    // Update pointers, sizes and flags.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    read_identifier_ = true;

    // That's all.

    return 0;
  }
}

int
observation_file_reader_bin_backward::
read_instance_id
(int& instance_identifier)
{
  {
    int   bytes_to_eat;
    void* destination;
    void* source;

    //
    // Check preconditions. This function works only when (1) the identifier
    // tag has been read, (2) no other component in the record has
    // been read neither and (3) we are reading an l-type record.
    //

    if (!read_identifier_) return 3;
    if ( read_iid_)        return 3;
    if (!reading_l_)       return 3;

    //
    // Get the instance identifier (an integer). We are sure that there's data in the
    // buffer since the type has already been read using read_type() and
    // this method guarantees that.
    //

    bytes_to_eat = sizeof(int);
    source       = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination  = (void*)&instance_identifier;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers, sizes and flags.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    read_iid_ = true;

    // That's all.

    return 0;
  }
}

int
observation_file_reader_bin_backward::
read_l_data
(int      n_tag_values,
 double*& the_tags,
 int      n_expectation_values,
 double*& the_expectations,
 int&     n_covariance_values_found,
 double*& the_covariance_values)
{
  {
    int   bytes_to_eat;
    void* destination;
    void* source;

    int   n;
    int   n_tags_found;
    int   n_expectations_found;

    //
    // Check preconditions. This function works only when the time
    // tag has been read and we are reading an l-type record.
    //

    if (!read_time_)               return 3;
    if ( read_completed_)          return 3;
    if (!reading_l_)               return 3;

    // Check dimensions.

    if (n_tag_values < 0)          return 6;
    if (n_expectation_values <= 0) return 6;

    //
    // Below, the number of tags, tags if any, number of expectations,
    // expectations, number of covariance values and covariance values,
    // if any, will be read. We're sure that we're not going beyond
    // the data buffer because when read_type() whas executed, it was
    // guaranteed that a full record was present there.
    //

    // Get the number of stored tags.

    bytes_to_eat  = sizeof(int);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)&n_tags_found;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    // Check that the number of tags stored correspond to those requested.

    if (n_tags_found != n_tag_values) return 7;

    // Get the tags themselves, if there are any.

    if (n_tag_values > 0)
    {
      bytes_to_eat  = n_tag_values * sizeof(double);
      source        = (void*)&(data_buffer_[epoch_cur_offset_]);
      destination   = (void*)the_tags;

      memcpy(destination, source, (size_t)bytes_to_eat);

      // Update pointers and sizes.

      epoch_cur_offset_      += bytes_to_eat;
      epoch_remaining_bytes_ -= bytes_to_eat;
    }

    // Get the number of stored expectations.

    bytes_to_eat  = sizeof(int);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)&n_expectations_found;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    // Check that the number of expectations stored correspond to those requested.

    if (n_expectations_found != n_expectation_values) return 7; // Invalid number of expectations.

    // Get the expectations themselves.

    bytes_to_eat  = n_expectation_values * sizeof(double);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)the_expectations;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    // Get the number of stored covariance values.

    bytes_to_eat  = sizeof(int);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)&n_covariance_values_found;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    // Check that the number of covariance values stored is legal

    n = n_expectations_found;

    if ((n_covariance_values_found != 0) &
        (n_covariance_values_found != n) &
        (n_covariance_values_found != ((n * (n + 1)) / 2))) return 7;

    // Get the covariance values themselves, if there are any.

    if (n_covariance_values_found > 0)
    {
      bytes_to_eat  = n_covariance_values_found * sizeof(double);
      source        = (void*)&(data_buffer_[epoch_cur_offset_]);
      destination   = (void*)the_covariance_values;

      memcpy(destination, source, (size_t)bytes_to_eat);

      // Update pointers and sizes.

      epoch_cur_offset_      += bytes_to_eat;
      epoch_remaining_bytes_ -= bytes_to_eat;
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
observation_file_reader_bin_backward::
read_o_data
(int   n_parameter_iids,
 int*& the_parameter_iids,
 int   n_observation_iids,
 int*& the_observation_iids,
 int   n_instrument_iids,
 int*& the_instrument_iids)
{
  {
    int   bytes_to_eat;
    void* destination;
    void* source;

    int   n_pars_found;
    int   n_obs_found;
    int   n_inst_found;

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
    // Read the different lists of identifiers (parameters, observations
    // and, eventually, instruments. We know that there's data enough in
    // the buffer since the type has already been read using read_type()
    // and this method guarantees that.
    //

    //
    // The number of parameter instance identifiers and these
    // themselves.
    //

    bytes_to_eat  = sizeof(int);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)&n_pars_found;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.
    
    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    // Check coherence

    if (n_pars_found != n_parameter_iids) return 7;

    // Read the instance identifiers array.

    bytes_to_eat  = n_pars_found * sizeof(int);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)the_parameter_iids;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    //
    // The number of observation instance identifiers and these
    // themselves.
    //

    bytes_to_eat  = sizeof(int);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)&n_obs_found;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.
    
    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    // Check coherence.

    if (n_obs_found != n_observation_iids) return 7;

    // Read the instance identifiers array.

    bytes_to_eat  = n_obs_found * sizeof(int);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)the_observation_iids;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    //
    // The number of instrument instance identifiers and these
    // themselves.
    //

    bytes_to_eat  = sizeof(int);
    source        = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination   = (void*)&n_inst_found;
    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.
    
    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    // Check coherence.

    if (n_inst_found != n_instrument_iids) return 7;

    // Only if there are instrument instance identifier we'll read these.

    if (n_inst_found > 0)
    {
      bytes_to_eat  = n_inst_found * sizeof(int);
      source        = (void*)&(data_buffer_[epoch_cur_offset_]);
      destination   = (void*)the_instrument_iids;
      memcpy(destination, source, (size_t)bytes_to_eat);

      // Update pointers and sizes.

      epoch_cur_offset_      += bytes_to_eat;
      epoch_remaining_bytes_ -= bytes_to_eat;
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
observation_file_reader_bin_backward::
read_prev_chunk
(void)
{
  {
    int    backtrack_index;
    int    bytes_in_epoch;
    long   bytes_read;
    long   bytes_to_read;
    int    offset;
    int    new_offset;
    int    previous_backtrack_index;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    // Check that there's still something to read.

    if (file_bytes_unread_ == 0)
    {
      is_eof_ = true;
      return 1;
    }

    // Ascertain how many bytes may be read at once.

    bytes_to_read = data_buffer_size_;
    if (bytes_to_read > file_bytes_unread_) bytes_to_read = file_bytes_unread_;

    if (bytes_to_read == 0)
    {
      is_eof_ = true;
      return 1;
    }

    //
    // Compute the number of bytes that won't be used in the data buffer. This
    // amount will be used as an offset to put data in this buffer.
    //

    offset = data_buffer_size_ - (int)bytes_to_read;

    //
    // Move the current position in the file bytes_to_read bytes backwards to
    // be able to read forward later on. Note that we ASSUME that the file
    // is positioned ("fseek-ed") at the beginning of the data block that
    // has already been processed, starting with an 'l' tag (or just at
    // the end of the file when no data has been read yet).
    //

    fseek(file_file_, -bytes_to_read, SEEK_CUR);

    // Read bytes_to_read bytes.

    bytes_read = (int)fread((void*)&(data_buffer_[offset]), 1, bytes_to_read, file_file_);
    if (bytes_read != bytes_to_read)
    {
      //
      // This should never happen; we never ask for more data that is stored
      // in the file, since we always know how much information is left there.
      //
      // Therefore, ff the number of bytes read do not match with the total
      // of bytes to read, this means that some kind of I/O error has arised.
      //

      return 2;
    }

    //
    // When bytes_to_read is exactly the amount of information that
    // had to be read or, in other words, when we have EXHAUSTED the
    // file, we do not need to use the backtracking tags to find the first
    // epoch in the buffer just read; when this situation arises, we KNOW
    // that all the information in the buffer contains an integral
    // number of epochs. Therefore, we may skip the strategy required
    // in the opposite case. This means that offset points to the correct
    // place (the beginning of an epoch) and that no corrections
    // need to be applied to this variable.
    //

    epoch_cur_backtrack_index_ = data_buffer_size_ - sizeof(int);

    if (bytes_read != file_bytes_unread_)
    {
      //
      // The file has not been exhausted (see comments above). We need
      // to readjust the offset to guarantee an integral number of
      // epochs in the current data buffer.
      //
      // Use the backtracking tags to ascertain how many full epochs are stored
      // in the buffer just read.
      //

      backtrack_index = epoch_cur_backtrack_index_;

      while (true)
      {
        //
        // Retrieve the number of bytes for the current epoch. Note that this
        // amount does not include the byte used to store the 'b' tag.
        //

        bytes_in_epoch = (*((int*)(&data_buffer_[backtrack_index])));

        // Add the byte used the 'b' tag.

        bytes_in_epoch++;

        //
        // Compute the address of the previous backtrack tag to check whether it
        // is still included in the current buffer just read. Note that
        // this is done:
        // - Substracting the bytes used by the current epoch (including the
        //   b-tag) from the current backtrack index and
        // - substracting the size of an integer (the type used to store the
        //   size of the epoch in bytes).
        // The resulting value is an index inside the buffer where the previous
        // backtracking tag STARTS. If this index goes beyond the origin of
        // the current buffer, we are over.
        //

        previous_backtrack_index = backtrack_index - bytes_in_epoch - sizeof(int);

        if (previous_backtrack_index < offset) break;

        // Make previous_backtrack_index the current one and try again.

        backtrack_index = previous_backtrack_index;
      }

      //
      // At this point, backtrack_index points to the LAST backtracking tag
      // fitting completely in the buffer. Only the information stored in
      // the data buffer that is located AFTER this tag contain full epochs of
      // o and l data.
      //
      // Therefore, we'll set the char just AFTER this tag as the first usable
      // chunk of information in the buffer. Also, we'll compute how
      // many bytes are we going to reject when moving forward the offset.
      //

      new_offset = backtrack_index + sizeof(int);
      bytes_read -= (new_offset - offset);
      offset = new_offset;
    }

    //
    // Update the number of bytes that we still have to read from the
    // input files. Note that we substract the number of USEFUL bytes
    // in the current buffer!
    //

    file_bytes_unread_ -= bytes_read;

    //
    // We must update the fseek position, so the next time this method
    // is invoked the fseek pointer points to the beginning of the
    // buffer we've just read. Now, it is pointing to its END, so we
    // must fseek -bytes_read backward.
    //

    fseek(file_file_, -bytes_read, SEEK_CUR);

    //
    // Now we know that we have an integral set of epochs starting at
    //   data_buffer_[offset]
    // and ending at
    //   data_buffer_[data_buffer_size_-1]
    // This is our work window.
    //
    // Note that, now, offset points to the first useful byte in the epoch,
    // which will contain a 'l' tag and the last 5 bytes of the buffer
    // will contain a 'b' tag (1 byte) plus the size of the last epoch (4 bytes).
    //
    // Set the members controlling:
    //   - The position in the data buffer where actual data starts.
    //   - The number of bytes (used for actual data, not for backtracking)
    //     in the last epoch.
    //   - The position in the data buffer where the last epoch starts.
    //

    chunk_start_index_      = offset;
    epoch_remaining_bytes_  = *((int*)(&(data_buffer_[epoch_cur_backtrack_index_])));
    epoch_cur_offset_       = epoch_cur_backtrack_index_ - epoch_remaining_bytes_ - 1;

    // That's all.

    return 0;
  }
}

int
observation_file_reader_bin_backward::
read_time
(double& time)
{
  {
    int   bytes_to_eat;
    void* destination;
    void* source;

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
    // Get the time (one double). We are sure that there's data in the
    // buffer since the type has already been read using read_type() and
    // this method guarantees that.
    //

    bytes_to_eat = sizeof(double);
    source       = (void*)&(data_buffer_[epoch_cur_offset_]);
    destination  = (void*)&time;

    memcpy(destination, source, (size_t)bytes_to_eat);

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

    // Update pointers, sizes and flags.

    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;

    read_time_ = true;

    // That's all.

    return 0;
  }
}

int
observation_file_reader_bin_backward::
read_type
(char& record_type)
{
  {
    int bytes_to_eat;
    int status;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Check preconditions. This function works only if no other
    // read operation is on its way.
    //

    if (!read_completed_) return 3;

    //
    // Check if we are going beyond the limits of the current epoch. If so,
    // we must look for the previous one. Note that requesting a new epoch might
    // trigger actual I/O operations (which might produce I/O errors) or
    // even a beginning-of-file condition. These situations are conveniently
    // signaled by the return code of update_epoch_pointers() .
    //

    if (epoch_remaining_bytes_ == 0)
    {
      //
      // Check always if the epoch info must be updated. If not necessary,
      // update_epoch_pointers() does nothing at all.
      //

      status = update_epoch_pointers();
      if (status != 0) return status;
    }

    //
    // We are sure now that we may retrieve the type data. Get it and
    // check that we are facing an 'l' or 'o' tag and no other.
    //

    record_type = data_buffer_[epoch_cur_offset_];

    if ((record_type != 'o') & (record_type != 'l')) return 4;

    // Update the current pointer and set flags accordingly.

    bytes_to_eat             = sizeof(char);
    epoch_cur_offset_      += bytes_to_eat;
    epoch_remaining_bytes_ -= bytes_to_eat;


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

int
observation_file_reader_bin_backward::
update_epoch_pointers
(void)
{
  {
    int bytes_to_go_back;
    int prev_epoch_info_index;

    //
    // If there are still bytes available in the current epoch, there's
    // nothing to do but report that everything's OK.
    //

    if (epoch_remaining_bytes_> 0) return 0;

    //
    // We've EXHAUSTED the current epoch. It is necessary to navigate
    // backwards to fetch the previous one.
    //

    //
    // Compute the amount of bytes necessary to find the previous backtracking
    // tag. This includes:
    //
    // - The size of the current epoch (excluding the backtracking info), plus
    // - the size of the integer containing the size of the current epoch, plus
    // - one byte to skip the 'b' tag itself.
    //
    // Substracting this amount from the current backtrack index we'll obtain
    // the position of the previous backtrack info (in fact, the integer
    // containing the size of the previous epoch).
    //

    bytes_to_go_back = *((int*)(&(data_buffer_[epoch_cur_backtrack_index_])))
                     + 1
                       + sizeof(int);

    prev_epoch_info_index = epoch_cur_backtrack_index_ - bytes_to_go_back;

    //
    // If this offset still leads to a position inside the current buffer,
    // we need to readjust some navigation pointers. Otherwise, we have
    // exhausted the current buffer and a new one must be read from disk.
    //

    if (prev_epoch_info_index > chunk_start_index_)
    {
      //
      // We are still inside the current buffer but changing epoch. Update
      // the relevant pointers and sizes.
      //

      epoch_cur_backtrack_index_ = prev_epoch_info_index;
      epoch_remaining_bytes_     = (*((int*)(&data_buffer_[ epoch_cur_backtrack_index_])));
      epoch_cur_offset_          = epoch_cur_backtrack_index_ - epoch_remaining_bytes_ - 1;

      return 0;
    }

    // We have exhausted the current buffer. We must read a new one from disk.

    return read_prev_chunk();

  }
}
