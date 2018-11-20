/** \file r_matrix_file_reader_bin_backward.cpp 
  \brief Implementation file for r_matrix_file_reader_bin_backward.cpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_file_reader_bin_backward.hpp"

int
r_matrix_file_reader_bin_backward::
backtrack_to_previous_record
(void)
{
  {
    int bytes_to_go_back;
    int prev_record_info_index;

    //
    // Compute the amount of bytes necessary to find the previous backtracking
    // tag. This includes:
    //
    // - The size of the current record (excluding the backtracking
    //   information), plus
    // - the size of the integer containing the size of the current record,
    // - plus one byte to skip the 'b' tag itself.
    //
    // Substracting this amount from the current backtrack index we'll obtain
    // the position of the previous backtrack info record (in fact, the integer
    // containing the size of the previous record).
    //

    bytes_to_go_back = *((int*)(&(data_buffer_[record_cur_backtrack_index_])))
                     + 1
                     + sizeof(int);

    prev_record_info_index = record_cur_backtrack_index_ - bytes_to_go_back;

    //
    // If this position is still inside the current buffer, it means that
    // we have a full record in the buffer that may be read without problems.
    // Otherwise, we have exhausted the current buffer and a new one must
    // be read from disk.
    //

    if (prev_record_info_index > chunk_start_index_)
    {
      //
      // We are still inside the current buffer. Update the relevant
      // pointers and sizes.
      //

      record_cur_backtrack_index_ = prev_record_info_index;
      record_remaining_bytes_     = (*((int*)(&data_buffer_[ record_cur_backtrack_index_])));
      record_cur_offset_          = record_cur_backtrack_index_ - record_remaining_bytes_ - 1;

      return 0;
    }

    // We have exhausted the current buffer. We must read a new one from disk.

    return read_prev_chunk();

  }
}

r_matrix_file_reader_bin_backward::
r_matrix_file_reader_bin_backward
(void)
{
  {
    backtrack_status_           =  0;
    record_cur_backtrack_index_ = -1;
    record_cur_offset_          = -1;
    record_remaining_bytes_     = -1;
    chunk_start_index_          = -1;
    file_bytes_unread_          =  0;
  }
}

r_matrix_file_reader_bin_backward::
~r_matrix_file_reader_bin_backward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
r_matrix_file_reader_bin_backward::
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
    //
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
r_matrix_file_reader_bin_backward::
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
    cactive      = data_buffer_[record_cur_offset_];

    active = (cactive == 1);

    // Update last record active flag.

    last_record_is_active_ = active;

    // Update pointers, sizes and flags.

    record_cur_offset_      += bytes_to_eat;
    record_remaining_bytes_ -= bytes_to_eat;

    read_active_flag_ = true;

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_bin_backward::
read_r_data
(int&     n_correlation_values_found,
 double*& the_correlation_values)
{
  {
    int   bytes_to_eat;
    void* destination;
    void* source;

    //
    // Check preconditions. This function works only when the time
    // tag has been read and we are reading an l-type record.
    //

    if (!read_time_)               return 3;
    if ( read_completed_)          return 3;

    //
    // Read Below, the number of correlation values.
    // We're sure that we're not going beyond the data buffer
    // because when read_type() whas executed, it was
    // guaranteed that a full record was present there.
    //

    // Get the number of stored correlations.

    bytes_to_eat  = sizeof(int);
    source        = (void*)&(data_buffer_[record_cur_offset_]);
    destination   = (void*)&n_correlation_values_found;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.

    record_cur_offset_      += bytes_to_eat;
    record_remaining_bytes_ -= bytes_to_eat;

    // Get the correlation values themselves.

    bytes_to_eat  = n_correlation_values_found * sizeof(double);
    source        = (void*)&(data_buffer_[record_cur_offset_]);
    destination   = (void*)the_correlation_values;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers and sizes.

    record_cur_offset_      += bytes_to_eat;
    record_remaining_bytes_ -= bytes_to_eat;

    //
    // Check that we've read the exact number of bytes in the
    // r-record.
    //

    if (record_remaining_bytes_ != 0) return 5;

    // Update flags.

    read_type_         = false;
    read_active_flag_  = false;
    read_time_         = false;
    read_completed_    = true;

    //
    // Backtrack to find the pointers of the previous record.
    // This may start a read operation if the current buffer has
    // been exhausted or an end-of-file condition if no more data
    // is available.
    // All these problems / conditions are reflected in the  status
    // code returned by backtrack_to_previous_record. We'll keep
    // the value of such return status because it is the task of
    // read_type to report any problems when we read a new record.
    //

    backtrack_status_ = backtrack_to_previous_record();

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_bin_backward::
read_prev_chunk
(void)
{
  {
    int    backtrack_index;
    int    bytes_in_record;
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
    // has already been processed, starting with an 'r' tag (or just at
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
    // record in the buffer just read; when this situation arises, we KNOW
    // that all the information in the buffer contains an integral
    // number of records. Therefore, we may skip the strategy required
    // in the opposite case. This means that offset points to the correct
    // place (the beginning of a record) and that no corrections
    // need to be applied to this variable.
    //

    record_cur_backtrack_index_ = data_buffer_size_ - sizeof(int);

    if (bytes_read != file_bytes_unread_)
    {
      //
      // The file has not been exhausted (see comments above). We need
      // to readjust the offset to guarantee an integral number of
      // records in the current data buffer.
      //
      // Use the backtracking tags to ascertain how many full records are stored
      // in the buffer just read.
      //

      backtrack_index = record_cur_backtrack_index_;

      while (true)
      {
        //
        // Retrieve the number of bytes for the current record. Note that this
        // amount does not include the byte used to store the 'b' tag.
        //

        bytes_in_record = (*((int*)(&data_buffer_[backtrack_index])));

        // Add the byte used the 'b' tag.

        bytes_in_record++;

        //
        // Compute the address of the previous backtrack tag to check whether it
        // is still included in the current buffer just read. Note that
        // this is done:
        // - Substracting the bytes used by the current record (including the
        //   b-tag) from the current backtrack index and
        // - substracting the size of an integer (the type used to store the
        //   size of the record in bytes).
        // The resulting value is an index inside the buffer where the previous
        // backtracking tag STARTS. If this index goes beyond the origin of
        // the current buffer, we are over.
        //

        previous_backtrack_index = backtrack_index - bytes_in_record - sizeof(int);

        if (previous_backtrack_index < offset) break;

        // Make previous_backtrack_index the current one and try again.

        backtrack_index = previous_backtrack_index;
      }

      //
      // At this point, backtrack_index points to the LAST backtracking tag
      // fitting completely in the buffer. Only the information stored in
      // the data buffer that is located AFTER this tag contain full r-records.
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
    // Now we know that we have an integral set of records starting at
    //   data_buffer_[offset]
    // and ending at
    //   data_buffer_[data_buffer_size_-1]
    // This is our work window.
    //
    // Note that, now, offset points to the first useful byte in the record,
    // which will contain a 'r' tag and the last 5 bytes of the buffer
    // will contain a 'b' tag (1 byte) plus the size of the last record (4 bytes).
    //
    // Set the members controlling:
    //   - The position in the data buffer where actual data starts.
    //   - The number of bytes (used for actual data, not for backtracking)
    //     in the last record.
    //   - The position in the data buffer where the last record starts.
    //

    chunk_start_index_      = offset;
    record_remaining_bytes_ = *((int*)(&(data_buffer_[record_cur_backtrack_index_])));
    record_cur_offset_      = record_cur_backtrack_index_ - record_remaining_bytes_ - 1;

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_bin_backward::
read_time
(double& time)
{
  {
    int   bytes_to_eat;
    void* destination;
    void* source;

    //
    // Check preconditions. This function works only when the active
    // flag element and no more has been read.
    //

    if (!read_active_flag_) return 3;
    if (read_time_)         return 3;

    //
    // Get the time (one double). We are sure that there's data in the
    // buffer since the type has already been read using read_type() and
    // this method guarantees that.
    //

    bytes_to_eat = sizeof(double);
    source       = (void*)&(data_buffer_[record_cur_offset_]);
    destination  = (void*)&time;

    memcpy(destination, source, (size_t)bytes_to_eat);

    // Update pointers, sizes and flags.

    record_cur_offset_      += bytes_to_eat;
    record_remaining_bytes_ -= bytes_to_eat;

    read_time_ = true;

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_bin_backward::
read_type
(char& record_type)
{
  {
    int bytes_to_eat;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Check preconditions. This function works only if no other
    // read operation is on its way.
    //

    if (!read_completed_) return 3;

    //
    // Let's check if the last backtrack operation returned an error.
    // This includes the "end-of-file" condition, which is not an
    // error, in fact.
    //

    //status = backtrack_to_previous_record();
    //if (status != 0) return status;

    if (backtrack_status_ != 0) return backtrack_status_;

    //
    // We are sure now that we may retrieve the type data. Get it and
    // check that we are facing an 'r' tag and no other.
    //

    record_type = data_buffer_[record_cur_offset_];

    if (record_type != 'r') return 4;

    // Update the current pointer and set flags accordingly.

    bytes_to_eat             = sizeof(char);
    record_cur_offset_      += bytes_to_eat;
    record_remaining_bytes_ -= bytes_to_eat;

    read_completed_        = false;
    read_type_             = true;

    // That's all.

    return 0;
  }
}
