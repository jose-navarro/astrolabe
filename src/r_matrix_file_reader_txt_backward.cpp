/** \file r_matrix_file_reader_txt_backward.cpp 
  \brief Implementation file for r_matrix_file_reader_txt_backward.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_file_reader_txt_backward.hpp"

int
r_matrix_file_reader_txt_backward::
find_first_complete_record
(int start, int& position)
{
  {
    char   chr;
    int    from;
    int    i;
    int    next_opening;
    int    record_end;
    int    record_start;
    int    status;

    //
    // Check if there is ONLY whitespace at the beginning of the buffer.
    // If so, we'll return an error code stating that the chunk contains
    // whitespace only.
    //
    // This situation may arise when there are lots of whitespace in
    // the input file and a the internal data buffer is completelly filled
    // with it OR when the last chunk (at the beginning of the file) is
    // read and there is whitespace between the first actual record and
    // the beginning of the file.
    //

    i = start;

    while (i < data_buffer_size_)
    {
      chr = data_buffer_[i];

      if (!is_whitespace(chr)) break;

      i++;
    }

    if (i >= data_buffer_size_) return 3; // Buffer contains whitespace only.

    //
    // Try to find a COMPLETE record starting at the current position. This MAY
    // fail. If so, we'll have to look for a record boundary and then look
    // again for the record.
    //

    from = i;

    status = find_record_fwd(from, record_start, record_end);

    if (status != 0)
    {

      //
      // Find the boundary between two records, that is, a point in the buffer
      // where a closing > is followed by whitespace and then by an opening <.
      //

      from = i;

      status = find_record_boundary(from, next_opening);

      if (status != 0) return 1; // Buffer exhausted while looking for the first record.

      from = next_opening;

      status = find_record_fwd(from, record_start, record_end);

      if (status == 1)
      {
        return 1; // Buffer exhausted while looking for the first record.
      }
      else if (status == 2)
      {
        return 2; // Malformed record found.
      }
    }

    //
    // We've found a complete record. Its starting position is
    // stored in record_start. Copy it to our output parameter.
    //

    position = record_start;

    //
    // Update position to include any whitespace characters preceding the
    // first record but located AFTER the last (imcomplete) record.
    //

    i = position - 1;

    while (i >= start)
    {
      chr = data_buffer_[i];

      if (!is_whitespace(chr)) break;

      i--;
    }

    position = i + 1;

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_txt_backward::
find_prev_record
(int  start,
 int& record_first, int& record_last)
{
  {
    char   chr;
    int    from;
    int    record_end;
    int    record_start;
    int    status;

    //
    // Purge whitespace. In cases where the top of the internal data buffer
    // contains only whitespace, there will be no records to find, so, in
    // this way, we avoid false errors.
    //

    from = start;

    while (from >= chunk_start_index_)
    {
      chr = data_buffer_[from];

      if (!is_whitespace(chr)) break;

      from--;
    }

    //
    // If we have reached the beginning of the data buffer while skipping
    // whitespace, it means that there are no records left there. This is
    // not an error; we have simply exhausted the current buffer.
    //

    if (from < chunk_start_index_)
    {
      data_buffer_current_ = chunk_start_index_ - 1; // Buffer exhausted.
      return 0;
    }

    //
    // At this point, it is guaranteed that we'll find a complete record, since
    // the process of loading the internal data buffer guarantees an integral
    // number of these. Having eaten up all whitespace (see above) and facing
    // a non-whitespace character in front of us, tell us that some data
    // is ready to be processed... that is, at least, one more record.
    //
    // This means that any errors found while searching for records may be
    // produced ONLY by malformed records.
    //

    //
    // Here, a closing '>' should be found. Otherwise, the file is not
    // correctly formed (there are garbage characters between valid records).
    //

    if (data_buffer_[from] != '>') return 1;

    //
    // Get the last full record ending in the current position (from).
    //

    status = find_record_bwd(from , record_start, record_end);
    if (status != 0) return 1; // Malformed record found.

    //
    // Set the data buffer_current_ and record_* pointers right now.
    //

    data_buffer_current_ = record_start - 1;
    record_first         = record_start;
    record_last          = record_end;

    // Remove, one more time, whitespace

    from = record_first - 1;

    while (from >= chunk_start_index_)
    {
      chr = data_buffer_[from];

      if (!is_whitespace(chr)) break;

      from--;
    }

    //
    // Update the data_buffer_cur_ pointer. It will point to the last
    // non-whitespace character in a former record (not yet processed).
    //

    data_buffer_current_ = from;

    // That's all.

    return 0;

  }
}

int
r_matrix_file_reader_txt_backward::
find_prev_record_limits
(void)
{
  {
    int  status;

    //
    // Check if there are still records left in the buffer. If not, read
    // more data from the actual file.
    //


    if (data_buffer_current_ < chunk_start_index_)
    {
      // Buffer exhausted. Try to get more data.

      status = read_prev_chunk();

      if (status == 1)
      {
        // This is a legal end-of-file condition.

        return 1;
      }
      else if (status == 2)
      {
        //
        // read_prev_chunk() reports I/O problems. We'll report exactly
        // the same.
        //

        return 3;
      }
      else if (status == 3)
      {
        // Malformed record found.

        return 2;
      }
      else if (status == 4)
      {
        // Buffer too small.

        return 4;
      }
    }


    //
    // Find the limits of the previous record in our buffer.
    //

    status = find_prev_record(data_buffer_current_,
                              cur_record_start_, cur_record_end_);

    //
    // The only reason why find_prev_record() could return a non-zero
    // error code are malformed records. If so, return an error code.
    //

    if (status != 0) return 2;


    //
    // Update the current record current pointer to point to the beginning
    // of the previous record just found.
    //

    cur_record_cur_ = cur_record_start_;

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_txt_backward::
find_record_boundary
(int from, int& next_opening)
{
  {
    char chr;
    bool done;
    int  i;
    int  j;

    done = false;
    i    = from;

    while (!done)
    {
      // Look for a closing '>'

      while (i < data_buffer_size_)
      {
        chr = data_buffer_[i];
        if (chr == '>') break;
        i++;
      }

      if (i >= data_buffer_size_) return 1;

      // Skip whitespace

      j = i + 1;

      while (j < data_buffer_size_)
      {
        chr = data_buffer_[j];
        if (!is_whitespace(chr)) break;
        j++;
      }

      if (j >= data_buffer_size_) return 1;

      //
      // If the char at j is an opening < we're done. On the contrary, we'll
      // keep searching one position beyond the current character.
      //

      if (chr == '<')
      {
        done = true;
        next_opening = j;
      }
      else
      {
        i = j + 1;
      }
    }

    // We're done.

    return 0;
  }
}

int
r_matrix_file_reader_txt_backward::
find_record_bwd
(int from,  int& start, int& end)
{
  {
    char chr;
    int  closing_tag_start;
    int  i;
    int  index;
    int  j;
    int  k;
    int  text_size;

    if (from < chunk_start_index_) return 1; // Beginning of buffer reached.

    index  = from;

    //
    // Look for the first '>' backwards. It should be part of the closing tag.
    //

    chr = data_buffer_[index];

    while (chr != '>')
    {
      index--;

       if (index < chunk_start_index_) return 1; // Beginning of buffer reached.

      chr = data_buffer_[index];
    }

    //
    // We've got a closing '>'. Save it as the last position of the
    // record.
    //

    end = index;

    //
    // Look for the first opening '<' backwards, since we are trying to find a
    // closing record tag.
    //

    index--;

    while (index >= chunk_start_index_)
    {
      chr = data_buffer_[index];

      if (chr == '<') break;

      index--;
    }

    if (index < chunk_start_index_) return 1;

    closing_tag_start = index;

    //
    // There is a tag, either opening or closing, in the data buffer positions
    // going from index to end. Let's copy these characters to the text buffer
    // provided by our parent class, removing all whitespace. This will let us
    // check more comfortably what type of tag we've just found.
    //
    // First, we need to guarantee that text_buffer (our parent's permanent
    // buffer) is big enough to hold the data.
    //


    text_size = end - index + 1;

    guarantee_text_buffer_size(text_size);

    j = 0;

    for (i = index; i <= end; i++)
    {
      chr = data_buffer_[i];

      if (!is_whitespace(chr))
      {
        text_buffer_[j] = chr;
        j++;
      }
    }

    //
    // If this is a closing tag, it must be 4 characters long (the pattern
    // is "</r>", without quotes.
    //

    if (j != 4) return 2; // Malformed record found.

    //
    // The length seems to be correct. However, we must still check that
    // the pattern is "</X>".
    //
    // This means that the second position in the text buffer must be a slash
    // and that the third one an "r".
    //

    if (text_buffer_[1] != '/') return 2; // Malformed record found.

    chr = text_buffer_[2];

    if (chr != 'r')
    {
      // It is not a closing tag!!! This is an error!
      return 2; // Malformed record found.
    }

    //
    // Look for the next opening '<' backwards. Presumably, it will belong to
    // the opening tag and will mark the beginning of the record.
    //

    index--;

    while (index >= chunk_start_index_)
    {
      chr = data_buffer_[index];

      if (chr == '<') break;

      index--;
    }

    if (index < chunk_start_index_) return 2; // Malformed record found.

    // index points now to the beginning of the record.

    start = index;

    //
    // Now, we'll try to find the first closing '>' forwards, which, presumably,
    // will be part of the opening tag.
    //

    k = start + 1;

    while (k < closing_tag_start)
    {
      chr = data_buffer_[k];

      if (chr == '>') break;

      k++;
    }

    if (k >= closing_tag_start) return 2; // Malformed record found.

    //
    // There is, again, a tag, either opening or closing, in the data buffer
    // positions going from start to k. Now, we'll try to check if it is
    // an r-tag.
    //
    // To do it, we'll skip all whitespace first, and then check if we've found
    // an r character.
    //
    // Note that we search between start + 1 and k - 1, to
    // avoid the opening '<' and closing '>' characters.
    //

    for (i = start+1; i < k; i++)
    {
      chr = data_buffer_[i];
      if (!is_whitespace(chr)) break;
    }

    if (i == k) return 2; // Malformed record found.

     // At this point, chr must be the tag type. Otherwise, we've detected an error.

    if (chr != 'r')
    {
      // This is an error! This is not a valid opening tag.
      return 2; // Malformed record found.
    }

    // That's all!

    return 0;
  }
}

int
r_matrix_file_reader_txt_backward::
find_record_fwd
(int from,  int& start, int& end)
{
  {
    char chr;
    int  i;
    int  index;
    int  j;
    int  k;
    int  opening_tag_end;
    int  text_size;

    if (from >= data_buffer_size_) return 1; // Reached the end of the buffer.

    index  = from;

    //
    // Look for the first '<' forwards. It should be part of the opening tag.
    //

    chr = data_buffer_[index];

    while (chr != '<')
    {
      index++;

       if (index >= data_buffer_size_) return 1; // End of buffer reached.

      chr = data_buffer_[index];
    }

    //
    // We've got an opening '<'. Save it as the first position of the
    // record.
    //

    start = index;

    //
    // Look for the first closing '>' forward, since we are trying to find a
    // opening record tag.
    //

    index++;

    while (index < data_buffer_size_)
    {
      chr = data_buffer_[index];

      if (chr == '>') break;

      index++;
    }

    if (index >= data_buffer_size_) return 1;

    opening_tag_end = index;

    //
    // There is a tag, either opening or closing, in the data buffer positions
    // going from start to index. Now, we'll try to ascertain if this is an
    // r- tag.
    //
    // To do it, we'll skip all whitespace first, and then check if we've found
    // an 'r' character.
    //
    // Note that we search between start + 1 and opening_tag_end - 1, to
    // avoid the opening '<' and closing '>' characters.
    //

    for (i = start+1; i < opening_tag_end; i++)
    {
      chr = data_buffer_[i];
      if (!is_whitespace(chr)) break;
    }

    if (i == opening_tag_end) return 2; // Malformed record found.

    // At this point, chr must be the tag type. Otherwise, we've detected an error.

    if (chr != 'r')
    {
      // This is an error! This is not a valid opening tag.
      return 2; // Malformed record found.
    }

    //
    // Look for the next closing '>' forwards. Presumably, it will belong to
    // the closing tag and will mark the end of the record.
    //

    index++;

    while (index < data_buffer_size_)
    {
      chr = data_buffer_[index];

      if (chr == '>') break;

      index++;
    }

    if (index >= data_buffer_size_) return 1;

    // index points now to the end of the record.

    end = index;

    //
    // Now, we'll try to find the first opening '<' backwards, which, presumably,
    // will be part of the closing tag.
    //

    k = end - 1;

    while (k > opening_tag_end)
    {
      chr = data_buffer_[k];

      if (chr == '<') break;

      k--;
    }

    if (k <= opening_tag_end) return 2; // Malformed record found.

    //
    // There is, again, a tag, either opening or closing, in the data buffer
    // positions going from k to end.
    //
    // Let's copy these characters to the text buffer removing whitespace
    // to check comfortably that this is the tag we're looking for.
    //

    text_size = end - k + 1;

    guarantee_text_buffer_size(text_size);

    j = 0;

    for (i = k; i <= end; i++)
    {
      chr = data_buffer_[i];

      if (!is_whitespace(chr))
      {
        text_buffer_[j] = chr;
        j++;
      }
    }

    //
    // If this is a closing tag, it must be 4 characters long (the pattern
    // is "</r>", without quotes.
    //

    if (j != 4) return 2; // Malformed record found.

    //
    // The length seems to be correct. However, we must still check that
    // the pattern is "</r>".
    //
    // This means that the second position in the text buffer must be a slash
    // and that the third one an 'r'.
    //

    if (text_buffer_[1] != '/') return 2; // 1 is the second position.

    chr = text_buffer_[2];

    if (chr != 'r')
    {
      // It is not a closing tag!!! This is an error!
      return 2; // Malformed record found.
    }

    // That's all!

    return 0;

  }
}

int
r_matrix_file_reader_txt_backward::
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

    if (read_status == 3) return 6; // Malformed records found.

    if (read_status == 4) return 7; // Buffer too small to locate the epoch.

    // Set the name of the file just read.

    file_name_ = strdup(file_name);

    // Successful completion.

    return 0;
  }
}

r_matrix_file_reader_txt_backward::
r_matrix_file_reader_txt_backward
(void)
{
  {
    chunk_start_index_         = -1;
    data_buffer_current_       = -1;
    file_bytes_unread_         =  0;
  }
}

r_matrix_file_reader_txt_backward::
~r_matrix_file_reader_txt_backward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
r_matrix_file_reader_txt_backward::
read_prev_chunk
(void)
{
  {
    long   bytes_read;
    long   bytes_to_read;
    bool   done;
    int    offset;
    int    new_offset;
    int    status;

    // Check that the file is open!

    if (!file_is_open_) return 2;

    done = true;

    while (true)
    {

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
      // We'll try to find the first complete record at the beginning
      // of the buffer to guarantee that an integral number of these
      // is present there.
      //

      status = find_first_complete_record(offset, new_offset);

      if (status == 3)
      {
        //
        // The buffer just read contains only whitespace. We'll have to
        // read more data. So we set the new_offset to offset (to indicate
        // that all the data read may be discarded) and set the done flag
        // to false.
        //

        new_offset = offset;
        done = false;
      }
      else if (status == 1)
      {
        //
        // Buffer exhausted while looking for the first record. This means that
        // our internal buffer is too small to hold a single record at a time!
        //

        return 4;
      }
      else if (status == 2)
      {
        // Malformed record found while trying to find the first record.
        return 3;
      }
      else
      {
        done = true;
      }

      //
      // new_offset points to the opening '<' character in the first record
      // of the first complete epoch in the data buffer.
      //
      // We'll compute how many bytes are we going to reject (from the
      // beginning of the buffer, since these contain no complete epochs).
      //

      bytes_read -= (new_offset - offset);
      offset = new_offset;

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
      //
      // Set the member controlling the position in the data buffer where actual
      // data starts.
      //
      // Finally, the global buffer pointer is set also to point to the last
      // position in the data buffer.
      //

      chunk_start_index_   = offset;
      data_buffer_current_ = data_buffer_size_ - 1;

      // Check if we are over.

      if (done) break;
    }

    // That's all.

    return 0;
  }
}

int
r_matrix_file_reader_txt_backward::
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
    // Get the limits of the previous full record. This operation may
    // lead to different situations:
    //
    // 1 - A full record is found in the current data buffer.
    //
    // 2 - The beginning of the current data buffer is reached while looking
    //     for a complete record. This means that more data must be
    //     loaded from disk. At this point, several alternatives are
    //     again possible:
    //
    //     2.1 - The needed data is read and the full record limits are
    //           found.
    //     2.2 - A legal end of file condition arises. This means that
    //           there is nothing else to be read from the input file
    //           and therefore the read process is over.
    //     2.3 - An I/O error is detected while reading. Rare, but not
    //           impossible situation.
    //  3 - The internal buffer proves to be too small to hold enough full
    //      data records to identify the first full record in the buffer.
    //
    //  Situations 1 and 2.1 imply that a full record has been detected and
    //  that it contents must be parsed.
    //
    //  Situation 2.2 simply means that there's no more data to read, so
    //  we are over. We must report an end of file condition and return.
    //
    //  Situations 2.3 and 3 must be reported as errors.
    //

    status = find_prev_record_limits();

    if (status == 1)
    {
      // Legal end of file.

      is_eof_ = true;
      return 1;
    }
    else if (status == 2)
    {
      // Malformed record found.

      return 4;
    }
    else if (status == 3)
    {
      // I/O error.
      return 2;
    }
    else if (status == 4)
    {
      //
      // Buffer too small. Unable to find the first full record in a data chunk
      // just read. If this ever happens, the two most probable combined reasons
      // are: the size of the internal buffer is really small and there are lots
      // of whitespace in the input file.
      //

      return 6;
    }

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
    // r-records may have only one attribute ("s").
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
