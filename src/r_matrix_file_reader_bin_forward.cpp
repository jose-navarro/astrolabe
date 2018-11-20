/** \file r_matrix_file_reader_bin_forward.cpp 
  \brief Implementation file for r_matrix_file_reader_bin_forward.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_file_reader_bin_forward.hpp"

int
r_matrix_file_reader_bin_forward::
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
r_matrix_file_reader_bin_forward::
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

r_matrix_file_reader_bin_forward::
r_matrix_file_reader_bin_forward
(void)
{
  {
    data_buffer_bytes_            = 0;
    data_buffer_current_          = 0;
    invalid_backtrack_info_found_ = false;
  }
}

r_matrix_file_reader_bin_forward::
~r_matrix_file_reader_bin_forward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
r_matrix_file_reader_bin_forward::
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
r_matrix_file_reader_bin_forward::
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

int
r_matrix_file_reader_bin_forward::
read_r_data
(int&     n_correlation_values_found,
 double*& the_correlation_values)
{
  {
    int   bytes_moved;
    int   bytes_to_eat;
    void* destination;
    void* source;
    int   status;
    int   read_status;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Check preconditions. This function works only when the time
    // tag and no other elements in the r-record has been read.
    //

    if (!read_time_)               return 3;
    if ( read_completed_)          return 3;

    //
    // Check if we are going beyond the limits of the data buffer. Note
    // that we only can check now if there are enough room left in the
    // data buffer to hold the number of correlations stored.
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

    // At this point we may read the number of correlation values safely, but no more.

    bytes_to_eat = sizeof(int);
    source       = (void*)&(data_buffer_[data_buffer_current_]);
    destination  = (void*)&n_correlation_values_found;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    //
    // At this point we KNOW how many correlation values must be read,
    // since we've just retrieved the actual number of these that have
    // actually been stored in the r-record.
    //
    // Let's check if we have gone beyond our buffer or not... 
    //

    bytes_to_eat = n_correlation_values_found * sizeof(double);

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
    // correlation matrix values. Let's go for it!
    //

    source      = (void*)&(data_buffer_[data_buffer_current_]);
    destination = (void*)the_correlation_values;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers.

    data_buffer_current_ = data_buffer_current_ + bytes_to_eat;

    //
    // Check that a valid b (backtracking) tag comes right after the
    // data we've read.
    //

    status = skip_backtrak_tag();

    if (status != 0)
    {
      if (status == 3) invalid_backtrack_info_found_ = true;
      else             return status;
    }

    //
    // Well, the backtracking info was where it should be. We're
    // allmost over. Update flags and say goodbye.
    //

    read_type_         = false;
    read_active_flag_  = false;
    read_time_         = false;
    read_completed_    = true;

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_bin_forward::
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
    // Check preconditions. This function works only when the active
    // flag element and no more has been read.
    //

    if (!read_active_flag_) return 3;
    if (read_time_)         return 3;

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

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_bin_forward::
read_type
(char& record_type)
{
  {
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
    // Check if a former call to the read_* methods found that no backtracking
    // information was present.
    //

    if (invalid_backtrack_info_found_) return 4;

    // Check if the buffer holds enough information as to include the type tag.

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
    // the type tag.
    //

    record_type = data_buffer_[data_buffer_current_];

    // Check that a correct type tag has been read.

    if (record_type != 'r')  return 4;

    // Update the current pointer and set flags accordingly.

    data_buffer_current_ += bytes_to_eat;

    read_completed_  = false;
    read_type_       = true;

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_bin_forward::  
skip_backtrak_tag
(void)
{
  {
    char backtrack_tag;
    int  bytes_for_backtracking;
    int  bytes_moved;
    int  bytes_to_eat;
    int  read_status;

    //
    // Check if we are going beyond the limits of the data buffer.
    //
    // Note that a tag indicating that backtracking information follows should
    // appear now. Backtracking information is used by backwards readers, and
    // it is of no interest for forward readers (our case now), so it must
    // be skipped when found.
    //
    // Backtracking information consist of a single character tag (containing
    // a 'b') plus an integer field containing byte counts.
    //
    // So, since backtracking information is on its way, we must be able to
    // analize, a sequence like the following one:
    //
    //   backtrack tag ('b') +
    //   backtrack info (1 int)
    //
    // So to cover the backtracking situation we'll ask the buffer to guarantee
    // that an (hypothetical) backtrack tag + an (hypothetical) backtrack info
    // items are available.
    //
    // That's why the next sentence tries to guarantee that at least
    // sizeof(char) + sizeof(int) bytes are at hand in the data buffer.
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
        return 1;
      }
      else if (read_status == 2)
      {
        // I/O error detected.
        return 2;
      }
    }

    // Get the tag and see if it is a backtracking one.

    backtrack_tag = data_buffer_[data_buffer_current_];

    if (backtrack_tag != 'b') return 3; 

    //
    // We've found the backtracking tag + info block. Let's skip it
    // updating the current pointer as many positions to the right
    // as bytes use the whole backtracking information block.
    //

    data_buffer_current_ += bytes_to_eat + bytes_for_backtracking;

    // That's all.

    return 0;
  }
}
