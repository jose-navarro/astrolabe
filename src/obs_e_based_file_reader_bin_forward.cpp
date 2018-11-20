/** \file obs_e_based_file_reader_bin_forward.cpp 
  \brief Implementation file for obs_e_based_file_reader_bin_forward.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "obs_e_based_file_reader_bin_forward.hpp"

obs_e_based_file_reader_bin_forward::
obs_e_based_file_reader_bin_forward
(void)
{
  {
    data_buffer_bytes_   = 0;
    data_buffer_current_ = 0;
  }
}

obs_e_based_file_reader_bin_forward::
~obs_e_based_file_reader_bin_forward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
obs_e_based_file_reader_bin_forward::
move_remaining_up
(void)
{
  {
    size_t bytes_to_move;
    void*  destination;
    void*  source;

    //
    // Compute the number of bytes to move. This is computing
    // substracting the first non-read position in the buffer
    // (data_buffer_current_) to the total number of bytes
    // available in the same buffer (data_buffer_bytes_).
    //

    bytes_to_move = data_buffer_bytes_ - data_buffer_current_;

    //
    // If the number of bytes to move is zero, there's no need to
    // copy at all. Just readjust sizes and pointers and return.
    //

    if (bytes_to_move == 0)
    {
      data_buffer_current_ = 0;
      data_buffer_bytes_   = 0;
      return 0;
    }

    //
    // Move the bytes in the queue of the buffer to its beginning.
    // Note that we must move bytes_to_move bytes starting at
    // position data_buffer_current_ to the very beginning of the
    // data buffer.
    //

    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)data_buffer_;

    memcpy(destination, source, bytes_to_move);

    // Recompute sizes and pointers in the data buffer.

    data_buffer_current_ = 0;
    data_buffer_bytes_   = (int)bytes_to_move;

    // Return the number of bytes moved.

    return (int)bytes_to_move;
  }
}

int
obs_e_based_file_reader_bin_forward::
open
(const char*  file_name)
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

    file_size_ = ftell(file_file_);

    //
    // Reset the fseek position, to be able to read from the start of the
    // file.
    //

    fseek(file_file_, 0 , SEEK_SET);

    // Try to create the buffer used for block reads.

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
    //

    read_status = read_next_chunk(0);


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
obs_e_based_file_reader_bin_forward::
read_active_flag
(bool& active)
{
  {
    int   bytes_moved;
    int   bytes_to_eat;
    char  cactive;
    int   read_status;

    //
    // Check preconditions. This function works only when (1) the type
    // tag has been read and (2) no other component in the record has
    // been read neither.
    //

    if (!read_type_)        return 3;
    if ( read_active_flag_) return 3;

    //
    // Check if we are going beyond the limits of the data buffer. Note
    // that active flags are represented using a single char.
    //

    bytes_to_eat = sizeof(char);

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

    // Get the active flag (a char with two possible values, 0 and 1).

    cactive = data_buffer_[data_buffer_current_];

    // Set parameter active using the cactive char just read.

    active = (cactive == 1);

    // Update last record active flag.

    last_record_is_active_ = active;

    // Update the current pointer and set flags accordingly.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;
    read_active_flag_ = true;

    // That's all.

    return 0;
  }
}

int
obs_e_based_file_reader_bin_forward::
read_identifier
(char*& identifier,
 int    id_len)
{
  {
    int   bytes_moved;
    int   bytes_to_eat;
    void* destination;
    void* source;
    int   read_status;
    int   sizeof_identifier;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Check preconditions. This function works only when (1) the active
    // flag has been read, (2) no other component in the record has
    // been read neither, (3) the buffer for the identifier is non-null.
    //

    if (!read_active_flag_) return 3;
    if (read_identifier_)   return 3;
    if (identifier == NULL) return 4;

    //
    // Retrieve the size of the identifier stored in the input file.
    // It is an int value. Note that we must be sure that there's
    // still enough information in the input buffer...
    //

    bytes_to_eat = (int) (sizeof(int));

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

    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)&sizeof_identifier;

    memcpy(destination, source, sizeof(int));
    data_buffer_current_ = data_buffer_current_ + (int)sizeof(int);

    //
    // If the size of the identifier in the input file exceeds that of the
    // identifier parameter, we must report an error.
    //

    if (id_len <= sizeof_identifier) return 4;

    //
    // Check AGAIN if we are going beyond the limits of the data buffer. Note
    // that the actual length of the identifier is sizeof_identifier, which we've
    // just read.
    //

    bytes_to_eat = (int) (sizeof_identifier * sizeof(char));

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

    // Get the identifier (sizeof_identifier chars).

    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)identifier;

    memcpy(destination, source, (size_t)bytes_to_eat);
    identifier[bytes_to_eat] = 0; // Ending NULL character.

    // Update the current pointer and set flags accordingly.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    read_identifier_ = true;

    // That's all.

    return 0;
  }
}

int
obs_e_based_file_reader_bin_forward::
read_instance_id
(int& instance_identifier)
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
    // Check preconditions. This function works only when (1) the identifier
    // has been read, (2) no other component in the record has been read
    // neither and (3) we are reading an l-type record.
    //

    if (!read_identifier_) return 3;
    if ( read_iid_)        return 3;
    if (!reading_l_)       return 3;

    //
    // Check if we are going beyond the limits of the data buffer. Note
    // that instance identifiers are represented using an integer.
    //

    bytes_to_eat = sizeof(int);

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

    // Get the instance identifier (an integer).

    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)&instance_identifier;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update the current pointer and set flags accordingly.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    read_iid_ = true;

    // That's all.

    return 0;
  }
}

int
obs_e_based_file_reader_bin_forward::
read_l_data
(int      n_tag_values,
 double*& the_tags,
 int      n_expectation_values,
 double*& the_expectations,
 int&     n_covariance_values_found,
 double*& the_covariance_values)
{
  {
    int   bytes_moved;
    int   bytes_to_eat;
    void* destination;
    void* source;
    int   read_status;

    int   n;
    int   n_tags_found;
    int   n_expectations_found;

    // Check that the file is open!

    if (!file_is_open_) return 2;

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
    // Check if we are going beyond the limits of the data buffer. Note
    // that we only can compute the amount of bytes needed by (a) the three
    // sizes of the three arrays plus the number of elements in the tags and
    // expectations arrays. We have NO idea about how many covariance matrix
    // values we'll find, so we'll check this later, once that we're sure
    // that we can read all the former items just described...
    //

    bytes_to_eat = 3 * sizeof(int) + (n_tag_values + n_expectation_values) * sizeof(double);

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
    // All right. We may read (a) the number of tags stored, (b) the tags
    // themselves (if any), (c) the number of expectation values, (d) the
    // expectation values themselves and (e) the number of covariance matrix
    // items. NO MORE. We have no guarantees that the covariance values
    // fit in the current buffer...
    //

    // Get the number of stored tags.

    bytes_to_eat = sizeof(int);
    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)&n_tags_found;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    // Check that the number of tags stored correspond to those requested.

    if (n_tags_found != n_tag_values) return 7;

    // Get the tags themselves, if any.

    if (n_tag_values > 0)
    {
      bytes_to_eat = n_tag_values * sizeof(double);
      source      = (void*)&(data_buffer_[data_buffer_current_]);
      destination = (void*)the_tags;

      memcpy(destination, source, (size_t)bytes_to_eat);

      // Update pointers.

      data_buffer_current_ = data_buffer_current_ + bytes_to_eat;
    }

    // Get the number of stored expectations.

    bytes_to_eat = sizeof(int);
    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)&n_expectations_found;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    // Check that the number of tags stored correspond to those requested.

    if (n_expectations_found != n_expectation_values) return 7;

    // Get the expectations themselves.

    bytes_to_eat = n_expectation_values * sizeof(double);
    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)the_expectations;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    // Get the number of stored covariance values.

    bytes_to_eat = sizeof(int);
    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)&n_covariance_values_found;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    // Check that the number of covariance values stored is legal

    n = n_expectations_found;

    if ((n_covariance_values_found != 0) &
        (n_covariance_values_found != n) &
        (n_covariance_values_found != ((n * (n + 1)) / 2))) return 7;

    //
    // At this point we KNOW how many covariance values must be read,
    // since we've just retrieved the actual number of stored covariances.
    //
    // Let's check if we have gone beyond our buffer or not... But only
    // if we DO have to read anything (the number of stored covariances
    // may be zero).
    //

    if (n_covariance_values_found != 0)
    {
      bytes_to_eat = n_covariance_values_found * sizeof(double);

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
      // Well, at this point, it is guaranteed that we may retrieve the
      // covariance matrix values. Let's go for it!
      //

      source      = (void*)&(data_buffer_[data_buffer_current_]);
      destination = (void*)the_covariance_values;

      memcpy(destination, source, (size_t)bytes_to_eat);

      // Update pointers.

      data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    }

    //
    // Update flags.
    //

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
obs_e_based_file_reader_bin_forward::
read_next_chunk
(int start_at)
{
  {
    void*  buffer_start_address;
    size_t bytes_read;
    size_t bytes_to_read;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Read as much bytes as possible (maximum: data_buffer_size_)
    // from the underlying binary file.
    // - The starting position in the destination buffer is given by
    //   parameter start_at.
    // - The number of bytes to read is computed substracting the starting
    //   position (start_at, first free position) to the total amount
    //   of bytes available in the data buffer.
    // - If the computed number of bytes to read is exactly 0 (this might
    //   happen) this would mean that the previous call to this method
    //   read exactly until the end of the file. Therefore, we must
    //   arise NOW a end-of-file condition to signal that no more data
    //   is available.
    //


    // Number of bytes to read.

    bytes_to_read = data_buffer_size_ - start_at;

    if (bytes_to_read == 0)
    {
      // No more data in the file. Arise an end-of-file condition.

      return 1;
    }

    // Start position in the data buffer.

    buffer_start_address = (void*) &(data_buffer_[start_at]);

    // Read the underlying file.

    bytes_read = fread(buffer_start_address, 1, bytes_to_read, file_file_);

    //
    // Check the number of bytes read. If it does not match the number of
    // bytes we wanted to read, we may be facing two different situations:
    //
    // (a) - An I/O error or,
    // (b) - An end-of-file condition. If this is the case, we will ignore
    //       the end-of-file, since the the next call to this method will
    //       re-detect it.
    //

    if (bytes_read != bytes_to_read)
    {
      // Check first the error status.

      if (ferror(file_file_)) return 2;

      //
      // Then, if the number of bytes read is exactly zero, we'll raise
      // an end of file condition. This is so (test zero bytes) because if
      // we have read at least one byte, it means that, although we've
      // exhausted the file, the data buffer has information just read.
      //

      if (bytes_read == 0)  return 1;
    }

    // Recompute the number of bytes available in the data buffer.

    data_buffer_bytes_ = start_at + (int)bytes_read;

    // That's all.

    return 0;
  }
}
