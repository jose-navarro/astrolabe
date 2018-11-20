/** \file r_matrix_file_reader_txt_forward.cpp 
  \brief Implementation file for r_matrix_file_reader_txt_forward.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_file_reader_txt_forward.hpp"

int
r_matrix_file_reader_txt_forward::
find_next_record_limits
(void)
{
  {
    int  i;
    bool opening_found;
    int  status;
    int  total_found;
    int  total_moved;

    // Reset current record pointers.

    cur_record_cur_   = -1;
    cur_record_end_   = -1;
    cur_record_start_ = -1;

    //
    // Try to find two closing '>'. This will mean that the opening
    // and closing record tags have been identified. When the second
    // closing '>' is reached, we've also reached the end of the record.
    //
    // Note that during this process the data buffer may be exhausted.
    // If such situation arises, more data (if any available) must be
    // loaded.
    //
    // Looking for the second closing '>' now implies that once found
    // (either because we had enough data in the buffer or because
    // some more had to be loaded) a whole record will be available
    // in the data buffer. This simplifies the algorithm.
    //
    // At the same time, we'll check if any opening '<' are found. This will
    // help us to determine if we are dealing with a corrupted or malformed
    // file in those cases that produce an end-of-file when trying to
    // load more data.
    //

    opening_found = false;
    total_found   = 0;
    i             = data_buffer_current_;

    while (i < data_buffer_bytes_)
    {
      if (data_buffer_[i] == '<') opening_found = true;

      if (data_buffer_[i] == '>')
      {
        total_found ++;
        if (total_found == 2) break;
      }

      i++;
    }

    //
    // If we've been able to find two closing '>', then there's enough
    // data in the buffer to hold a complete record. In the opposite case,
    // we must get more data from the file.
    //

    if (total_found == 2)
    {
      // We have a complete record and have identified its ending position.

      cur_record_end_ = i;
    }
    else
    {
      // End of buffer reached. We must try to load more data.

      //
      // Move the data still available at the end of the buffer to its
      // beginning.
      //
      // Note that move_remaining_up() resets de data_buffer_cur_ and
      // data_buffer_bytes_ members!!!
      //

      total_moved = move_remaining_up();

      //
      // Read as many data as possible from the file. Be careful! The first
      // total_moved bytes in the data buffer must not be overwritten!
      //

      status = read_next_chunk(total_moved);

      // Did we have problems while reading?

      if (status == 1)
      {
        //
        // read_next_chunk() reported an end of file condition. If we had
        // not found an opening '<', this is a LEGAL eof. Otherwise, the
        // file has ended abruptly, with a half written record...
        //

        if (opening_found) return 2; // Unexpected end of file.

        return 1; // Legal end of file.
      }
      else if (status == 2)
      {
        //
        // read_next_chunk() reports I/O problems. We'll report exactly
        // the same.
        //

        return 3;
      }

      //
      // read_next_chunck() returned more data. Now we are sure that there
      // is enough information to hold a full record.
      //
      // However, since the original information in the buffer has been
      // moved to its beginning, we must, again, look for the second
      // closing '>'.
      //
      // We are assuming now that the whole record fits into our buffer, since
      // at this point we've read a full chunk of data from disk.
      //

      i = data_buffer_current_; // Beginning of the buffer, since
                                // move_remaining_up() resets
                                // data_buffer_current_ to 0.
      total_found = 0;

      while (i < data_buffer_bytes_)
      {
        if (data_buffer_[i] == '>')
        {
          total_found ++;
          if (total_found == 2) break;
        }

        i++;
      }

      // We've got the position of the last (second, in fact) closing '>'.

      cur_record_end_ = i;

    }

    // Now, find the first opening '<'.

    i = data_buffer_current_;

    while (i < data_buffer_bytes_)
    {
      if (data_buffer_[i] == '<') break;
      i++;
    }

    cur_record_start_ = i;

    // Set the current pointer to the start of the record.

    cur_record_cur_ = cur_record_start_;


    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_txt_forward::
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
r_matrix_file_reader_txt_forward::
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

    // Set the name of the file just read.

    file_name_ = strdup(file_name);

    // Successful completion.

    return 0;
  }
}

r_matrix_file_reader_txt_forward::
r_matrix_file_reader_txt_forward
(void)
{
  {
    data_buffer_bytes_   = 0;
    data_buffer_current_ = 0;
  }
}

r_matrix_file_reader_txt_forward::
~r_matrix_file_reader_txt_forward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
r_matrix_file_reader_txt_forward::
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
r_matrix_file_reader_txt_forward::
read_type
(char& record_type)
{
  {
    string      att_name;
    int         att_search_end;
    int         att_search_start;
    string      att_value;
    int         status;
    int         total_att;
    char        typ;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    //
    // Check preconditions. This function works only if no other
    // read operation is on its way.
    //

    if (!read_completed_) return 3;

    //
    // Get the limits of the next full record. This operation may
    // lead to different situations:
    //
    // 1 - A full record is found in the current data buffer.
    //
    // 2 - The end of the current data buffer is reached while looking
    //     for a complete record. This means that more data must be
    //     loaded from disk. At this point, several alternatives are
    //     again possible:
    //
    //     2.1 - The needed data is read and the full record limits are
    //           found.
    //     2.2 - A legal end of file condition arises. This means that
    //           there is nothing else to be read from the input file
    //           and therefore the read process is over.
    //     2.3 - An illegal end of file condition is detected. This means
    //           that the end of file arises when part of a record had
    //           been read, thus pointing to a corrupted / malformed
    //           input file.
    //     2.4 - An I/O error is detected while reading. Rare, but not
    //           impossible situation.
    //
    //  Situations 1 and 2.1 imply that a full record has been detected and
    //  that it contents must be parsed.
    //
    //  Situation 2.2 simply means that there's no more data to read, so
    //  we are over. We must report an end of file condition and return.
    //
    //  Situations 2.3 and 2.4 must be reported as errors.
    //

    status = find_next_record_limits();

    if (status == 1)
    {
      // Legal end of file.

      is_eof_ = true;
      return 1;
    }
    else if (status == 2)
    {
      // Illegal end of file (file corrupted / malformed).

      is_eof_ = true;
      return 5;
    }
    else if (status == 3)
    {
      // I/O error.
      return 2;
    }

    //
    // Update the global data buffer pointer. We'll make it point just
    // after the current record's end (for later use when processing
    // new records).
    //

    data_buffer_current_ = cur_record_end_ + 1;

    //
    // Now, cur_record_start_ and cur_record_end_ hold the limits of the
    // full record. We must start parsing the buffer to look for the
    // opening tag. Note that cur_record_cur_ points to the first non
    // scanned position in the data buffer.
    //
    // There is no whitespace nor other spurious characters AROUND the
    // record, since find_next_record_limits() returns the precise limits
    // of the record itself, eliminating any invalid garbage around.
    //
    // This means that data_buffer_[cur_record_cur_] points to an opening
    // '<' character. The record type (inside the opening record tag) will
    // be located after this opening '<'.
    //

    cur_record_cur_ ++;

    //
    // Get the record type. Make sure that we skip any whitespace that might
    // have written inside the record tag.
    //

    typ = data_buffer_[cur_record_cur_];

    while (is_whitespace(typ))
    {
      cur_record_cur_++;
      typ = data_buffer_[cur_record_cur_];
    }

    // The record type MUST be at the current position now.

    typ = data_buffer_[cur_record_cur_];

    //
    // Move the current pointer forward to go beyond the type. This
    // new position, additionally, will be the starting point to
    // search for attributes inside the opening tag.
    //

    cur_record_cur_++;
    att_search_start = cur_record_cur_;

    // Check that a correct type tag has been read.

    if (typ != 'r') return 4;

    //
    // Assign the record type, using only lowercase characters.
    //

    record_type = 'r';

    //
    // Update the current pointer until the closing '>' character in the
    // opening record tag is passed.
    //

    while (data_buffer_[cur_record_cur_] != '>') cur_record_cur_++;

    //
    // Now, cur_record_cur_ points the the position where the closing '>'
    // char has been found. The PREVIOUS position will be the one used
    // to search for attributes in the opening tag.
    //

    att_search_end = cur_record_cur_ - 1;

    // Move the cur_record_cur_ pointer just beyond the closing '>'.

    cur_record_cur_ ++;

    //
    // Parse the attributes that should be present in the opening tag.
    // These differ according to the type of the record being read:
    //
    // For both kinds of records: id (mandatory) and s (optional).
    // For l-records only: n (mandatory).
    //
    // If attribute s does not exist, the record is assumed active
    // by default.
    //
    // Parsing may fail because of syntactical reasons...
    //

    status = parse_attributes(data_buffer_, att_search_start, att_search_end, cur_record_attributes_);
    if (status != 0) return 4;

    //
    // The parsing process succeeded, so no syntax errors exist. 
    //

    total_att = (int)cur_record_attributes_.size();

    //
    // Look for the s attribute. It is optional, so be prepared for a failure.
    //

    last_record_is_active_ = true;

    att_name = "s";
    att_value = cur_record_attributes_[att_name];

    if (att_value != "")
    {
      //
      // s attribute found. Decrease the total number of attributes to check.
      //

      total_att --;

      //
      // Instead of the default "true" value assigned above, we
      // must set last_record_is_active_ according to the attribute
      // just obtained.
      //

      if (att_value == "r")
      {
        last_record_is_active_ = false;
      } else if (att_value == "a")
      {
        last_record_is_active_ = true;
      }
      else
      {
        // Oooops! Invalid value for the s attribute.
        return 4;
      }
    }

    //
    // If there still exists attributes in the list (see total_att), it means
    // that there are too much of these, which is an error.
    //

    if (total_att != 0) return 4;

    // Set remaining flags.

    read_completed_        = false;
    read_type_             = true;

    // That's all.

    return 0;
  }
}

