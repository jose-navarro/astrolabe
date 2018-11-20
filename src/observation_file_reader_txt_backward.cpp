/** \file observation_file_reader_txt_backward.cpp 
  \brief Implementation file for observation_file_reader_txt_backward.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "observation_file_reader_txt_backward.hpp"

observation_file_reader_txt_backward::
observation_file_reader_txt_backward
(void)
{
  {
    chunk_start_index_         = -1;
    data_buffer_current_       = -1;
    epoch_start_               = -1;
    epoch_remaining_records_   =  0;
    epoch_end_                 = -1;
    file_bytes_unread_         =  0;
  }
}

observation_file_reader_txt_backward::
~observation_file_reader_txt_backward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
observation_file_reader_txt_backward::
find_first_epoch
(int start, int& position)
{
  {
    char   chr;
    int    found_l_record;
    int    from;
    int    i;
    int    next_opening;
    int    record_end;
    int    record_start;
    int    status;
    double timetag_current;
    bool   timetag_differs;
    double timetag_first;
    char   type;

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
    // again for the record. It's type may be either l- or o-, it does not
    // matter.
    //

    from = i;

    status = find_record_fwd(from, record_start, record_end, type);

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

      status = find_record_fwd(from, record_start, record_end, type);

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
    // Find the timetag for this "first" record. It will be used to compare
    // to following records' timetags.
    //

    status = find_timetag(type, record_start, record_end, timetag_first);
    if (status != 0) return 2; // Malformed record found.

    from = record_end + 1;
    if (from >= data_buffer_size_) return 1;

    //
    // We have got the time tag value of the first complete record stored
    // in the internal data buffer.
    //
    // We cannot guarantee that this record belongs to a complete epoch, so
    // now now we'll look for l-records (these are always the first ones
    // in every epoch) until we find one with a time tag value that differs
    // from the first time tag we've just obtained.
    //
    // The position of such record will mark the beginning of a complete epoch.
    //

    timetag_differs = false;

    while (!timetag_differs)
    {

      // Look for l-records.

      found_l_record = false;

      while (!found_l_record)
      {

        if (from >= data_buffer_size_) return 1; // Buffer exhausted.

        status = find_record_fwd(from, record_start, record_end, type);

        if (status == 1)
        {
          return 1; // Buffer exhausted while looking for the first record.
        }
        else if (status == 2)
        {
          return 2; // Malformed record found.
        }

        found_l_record = (type == 'l');

        from = record_end + 1;
      }

      // l-record found. Check its time tag.

      status = find_timetag(type, record_start, record_end, timetag_current);
      if (status != 0) return 2; // Malformed record found.

      timetag_differs = (timetag_first != timetag_current);
    }

    //
    // Finally, we found an l-record with the right timestamp. The starting
    // position of this record becomes the starting position of a full epoch
    // (by now).
    //

    position = record_start;

    //
    // Update position to include any whitespace characters preceding the
    // first record in the epoch but located AFTER the last record not
    // in the first epoch just found.
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
observation_file_reader_txt_backward::
find_next_record_limits
(void)
{
  {
    int  status;
    char type;

    //
    // Check if there are still records left in the current epoch. If not,
    // get a previous epoch (which may, in turn trigger I/O operations).
    //
    // Note the while loop instead of an if clause. This is so because even
    // when new data is read from the input file, it could countain no records
    // if the chunk thus obtained contains only whitespaces.
    //

    while (epoch_remaining_records_ == 0)
    {
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
      else
      {

        //
        // Note that read_prev_chunk() in the if corresponding to this
        // else already calls find_prev_epoch. Therefore, we only need
        // to call this method when no data has been read from the input file!!!
        //

        status = find_prev_epoch(epoch_start_ - 1,
                                 epoch_start_, epoch_end_,
                                 cur_record_start_, cur_record_end_,
                                 epoch_remaining_records_);

        //
        // The only reason why find_prev_epoch() could return a non-zero
        // error code are malformed records. If so, return an error code.
        //

        if (status != 0) return 2;
      }

      //
      // Update the current record current pointer to point to the beginning
      // of the first record in the epoch.
      //

      cur_record_cur_ = cur_record_start_;
    }

    //
    // At this point we are sure that there are still records left in
    // the data buffer (either because these there were there already or
    // because a new data chunk has been read.
    //
    // Let's fetch its limits, which will be used by the different (public)
    // read_* functions to retrieve the different elements in the record.
    //

    status = find_record_fwd(cur_record_cur_,
                             cur_record_start_, cur_record_end_,
                             type);

    //
    // The previous call SHOULD NEVER FAIL, since records where parsed correctly
    // when the current epoch was found using find_prev_epoch(). However, and
    // for the sake of completeness, we'll check the return code.
    //

    if (status != 0) return 2; // Malformed record.

    //
    // The current record current pointer must point to the beginning of
    // the record.
    //

    cur_record_cur_ = cur_record_start_;

    //
    // There's one record less in the current epoch (the one whose limits
    // we've just computed above).
    //

    epoch_remaining_records_--;


    // That's all.

    return 0;
  }
}

int
observation_file_reader_txt_backward::
find_prev_epoch
(int  start,
 int& epoch_first , int& epoch_last,
 int& record_first, int& record_last,
 int& total_records)
{
  {
    char   chr;
    int    from;
    int    record_end;
    int    record_start;
    int    status;
    double timestamp_current;
    double timestamp_last;
    char   type;

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
      epoch_first          = -1;
      epoch_last           = -1;
      total_records        = 0;
      data_buffer_current_ = chunk_start_index_ - 1; // Buffer exhausted.
      record_first         = -1;
      record_last          = -1;

      return 0;
    }

    //
    // At this point, it is guaranteed that we'll find a complete epoch, since
    // the process of loading the internal data buffer guarantees an integral
    // number of these. Having eaten up all whitespace (see above) and facing
    // a non-whitespace character in front of us, tell us that some data
    // is ready to be processed... that is, at least, one more epoch.
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
    // Get the last full record in the buffer. We'll use it to retrieve
    // its timestamp and will use to check the end of the epoch when we
    // read more records.
    //

    status = find_record_bwd(from , record_start, record_end, type);
    if (status != 0) return 1; // Malformed record found.

    status = find_timetag(type, record_start, record_end, timestamp_last);
    if (status != 0) return 1;  // Malformed record found.

    //
    // We've got a record. By now, its limits are also the limits of the
    // full epoch.
    //

    total_records  = 1;
    epoch_first    = record_start;
    epoch_last     = record_end;

    //
    // Just in case there is only one record in this epoch: set the
    // data buffer_current_ and record_* pointers right now.
    //

    data_buffer_current_ = record_start - 1;
    record_first         = record_start;
    record_last          = record_end;

    //
    // Try to find as many records as possible sharing the time tag just
    // obtained (timetag_last).
    //

    while (true)
    {
      from = record_start - 1;

      // Again, skip whitespace.

      while (from >= chunk_start_index_)
      {
        chr = data_buffer_[from];

        if (!is_whitespace(chr)) break;

        from--;
      }

      //
      // Update the pointer to the first position in the epoch as well as
      // the data_buffer_current_ pointer.
      //

      epoch_first          = from + 1;
      data_buffer_current_ = from;

      // Have we exhausted the internal buffer?

      if (from < chunk_start_index_)
      {
        //
        // No more data in the chunk (NOR in the EPOCH, since it is guaranteed
        // that epochs are stored completely in the internal data buffer).
        //
        // This is, therefore, not an error condition but an end condition.
        //

        return 0;
      }

      //
      // Here, a closing '>' should be found. Otherwise, the file is not
      // correctly written (there are garbage characters between valid records).
      //

      if (data_buffer_[from] != '>') return 1;

      //
      // No more whitespace. Therefore, more records must be included in
      // the internal data buffer. Get the previous one.
      //

      status = find_record_bwd(from , record_start, record_end, type);
      if (status != 0) return 1; // Malformed record found.

      status = find_timetag(type, record_start, record_end, timestamp_current);
      if (status != 0) return 1; // Malformed record found.

      if (timestamp_current == timestamp_last)
      {
        //
        // One more record with the same timetag found. Update the first
        // position of the epoch as well as the number of records found.
        //

        epoch_first = record_start;
        total_records++;

        // Update the record_* pointers as well.

        record_first = record_start;
        record_last  = record_end;
      }
      else
      {
        // We've found a record with a different timestamp. We're over.
        break;
      }
    }

    // Remove, one more time, whitespace

    from = record_first - 1;

    while (from >= chunk_start_index_)
    {
      chr = data_buffer_[from];

      if (!is_whitespace(chr)) break;

      from--;
    }

    // Update the first position in the epoch.

    epoch_first = from + 1;

    //
    // Update the data_buffer_cur_ pointer. Make it point to one position
    // less than the start of the first record in this epoch.
    //

    data_buffer_current_ = from;

    // That's all.

    return 0;

  }
}

int
observation_file_reader_txt_backward::
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
observation_file_reader_txt_backward::
find_record_bwd
(int from,  int& start, int& end, char& type)
{
  {
    char chr;
    int  closing_tag_start;
    int  i;
    int  index;
    int  j;
    int  k;
    int  text_size;
    char type_closing;
    char type_opening;

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
    // closing record tag, either l- or o-.
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
    // is "</X>", without quotes, where X stands for l or o.
    //

    if (j != 4) return 2; // Malformed record found.

    //
    // The length seems to be correct. However, we must still check that
    // the pattern is "</X>".
    //
    // This means that the second position in the text buffer must be a slash
    // and that the third one either an l or an o (lower or uppercase).
    //

    if (text_buffer_[1] != '/') return 2; // Malformed record found.

    chr = text_buffer_[2];

    if (chr == 'l')
    {
      type_closing = 'l';
    }
    else if (chr == 'o')
    {
      type_closing = 'o';
    }
    else
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
    // positions going from start to k. Now, we'll try to ascertain if thi is an l- or
    // o- tag.
    //
    // To do it, we'll skip all whitespace first, and then check if we've found
    // an l or o character.
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

    if (chr == 'l')
    {
      type_opening = 'l';
    }
    else if (chr == 'o')
    {
      type_opening = 'o';
    }
    else
    {
      // This is an error! This is not a valid opening tag.
      return 2; // Malformed record found.
    }

    if (type_opening != type_closing)
    {
      // Tag mismatch.
      return 2; // Malformed record found.
    }

    type = type_opening; // type_closing could have benn used instead.

    // That's all!

    return 0;
  }
}

int
observation_file_reader_txt_backward::
find_record_fwd
(int from,  int& start, int& end, char& type)
{
  {
    char chr;
    int  i;
    int  index;
    int  j;
    int  k;
    int  opening_tag_end;
    int  text_size;
    char type_closing;
    char type_opening;

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
    // opening record tag, either l- or o-.
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
    // going from start to index. Now, we'll try to ascertain if thi is an l- or
    // o- tag.
    //
    // To do it, we'll skip all whitespace first, and then check if we've found
    // an l or o character.
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

    if (chr == 'l')
    {
      type_opening = 'l';
    }
    else if (chr == 'o')
    {
      type_opening = 'o';
    }
    else
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
    // is "</X>", without quotes, where X stands for l or o (lower or uppercase)
    //

    if (j != 4) return 2; // Malformed record found.

    //
    // The length seems to be correct. However, we must still check that
    // the pattern is "</X>".
    //
    // This means that the second position in the text buffer must be a slash
    // and that the third one either an l or an o (lower or uppercase).
    //

    if (text_buffer_[1] != '/') return 2; // 1 is the second position.

    chr = text_buffer_[2];

    if (chr == 'l')
    {
      type_closing = 'l';
    }
    else if (chr == 'o')
    {
      type_closing = 'o';
    }
    else
    {
      // It is not a closing tag!!! This is an error!
      return 2; // Malformed record found.
    }

    if (type_opening != type_closing)
    {
      // Tag mismatch.
      return 2; // Malformed record found.
    }

    type = type_opening; // type_closing could have benn used instead.


    // That's all!

    return 0;

  }
}

int
observation_file_reader_txt_backward::
find_timetag
(char    record_type,
 int     record_start, int record_end,
 double& timetag)
{
  {
    char   chr;
    void*  destination;
    int    index;
    void*  source;
    int    timetag_first;
    int    timetag_last;
    int    timetag_size;
    double timetag_value;

    index = record_start;

    //
    // Skip the opening record tag. Iterate until the closing ">" in the
    // opening tag is found.
    //

    chr = data_buffer_[index];

    while (chr != '>')
    {
      index++;
      if (index > record_end) return 1;

      chr = data_buffer_[index];
    }

    index++; // Skip the '>' we've just found.
    if (index > record_end) return 1;

    chr = data_buffer_[index];

    //
    // Skip all whitespace between the opening record tag
    // closing ">" and the time tag itself.
    //

    while (is_whitespace(chr))
    {
      index++;
      if (index > record_end) return 1;

      chr = data_buffer_[index];
    }

    if (index > record_end) return 1;

    chr = data_buffer_[index];

    // At this moment, index points to the first character in the time tag.

    timetag_first = index;

    // Look now for the last position.

    index++;
    if (index > record_end) return 1;

    chr = data_buffer_[index];

    while (!is_whitespace(chr))
    {
      index++;
      if (index > record_end) return 1;

      chr = data_buffer_[index];
    }

    timetag_last = index;
    timetag_size = timetag_last - timetag_first + 1;

    //
    // The string representing the time tag is in positions time_tag_first to
    // time_tag_last in the internal data buffer. Now it is time to convert
    // this string to binary form. We'll use the text buffer provided by
    // our parent class to perform this conversion.
    //

    guarantee_text_buffer_size(timetag_size + 1); // Includes final NULL.

    // Copy the text in the data buffer (source) to our local copy (destination).

    source      = (void*)&(data_buffer_[timetag_first]);
    destination = (void*)text_buffer_;

    memcpy(destination, source, (size_t)timetag_size);
    text_buffer_[timetag_size] = 0; // Ending NULL character.

    // Try to convert the text just obtained to a binary double value.

    if (sscanf(text_buffer_, "%lf", &timetag_value) == EOF)
    {
      // The text read does not represent a legal double value!!!
      return 1;
    }

    // We've got it!

    timetag = timetag_value;

    // That's all!

    return 0;

  }
}

int
observation_file_reader_txt_backward::
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
    // Note that read_prev_chunk() tries to find the first complete epoch
    // in the information read. This may lead to errors 3 and 4 in the
    // list below.
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

int
observation_file_reader_txt_backward::
read_o_data
(int   n_parameter_iids,
 int*& the_parameter_iids,
 int   n_observation_iids,
 int*& the_observation_iids,
 int   n_instrument_iids,
 int*& the_instrument_iids)
{
  {
    int i;
    int ival;
    int status;

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
    // Proceed to parse n_parameter_iids + n_observation_iids +
    // n_instrument_iids integer values. Note that there may
    // be no list of instrument instance indentifiers.
    //

    // Parameter instance identifiers.

    for (i = 0; i < n_parameter_iids; i++)
    {
      status = parse_int_value(ival);
      if (status != 0)
      {
        // The text parsed does not contain a valid integer value.
        return 4;
      }

      the_parameter_iids[i] = ival;
    }

    // Observation instance identifiers

    for (i = 0; i < n_observation_iids; i++)
    {
      status = parse_int_value(ival);
      if (status != 0)
      {
        // The text parsed does not contain a valid integer value.
        return 4;
      }

      the_observation_iids[i] = ival;
    }

    // Instrument instance identifiers, if any.

    if (n_instrument_iids)
    {
      for (i = 0; i < n_instrument_iids; i++)
      {
        status = parse_int_value(ival);
        if (status != 0)
        {
          // The text parsed does not contain a valid integer value.
          return 4;
        }

        the_instrument_iids[i] = ival;
      }
    }

    //
    // Check that the characters after the list of integers just found
    // are only whitespace followed by the closing o-tag.
    //

    status = parse_closing_o_tag();

    if (status != 0)
    {
      // The remaining characters are invalid. We've got a malformed o-record.
      return 5;
    }

    //
    // Set flags.
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
observation_file_reader_txt_backward::
read_prev_chunk
(void)
{
  {
    long   bytes_read;
    long   bytes_to_read;
    char   chr;
    bool   done;
    int    i;
    int    offset;
    int    new_offset;
    int    record_end;
    int    record_start;
    int    status;
    char   type;

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
      // When bytes_to_read is exactly the amount of information that
      // had to be read or, in other words, when we have EXHAUSTED the
      // file, we must proceed in a different way than in the opposite
      // case. The identification of the first epoch in the chunk just
      // read is different.
      //

      if (bytes_read == file_bytes_unread_)
      {
        //
        // We've exhaused the input file. Therefore, we must ASSUME that
        // the first complete record in the internal data buffer corresponds
        // to the beginning of a full epoch. The only thing we may check
        // is that this record is of type l.
        //
        // We therefore assume that an integral number of epochs is stored
        // in our buffer.
        //

        // Skip whitespace from the beginning of the buffer.

        for (i = offset; i < data_buffer_size_; i++)
        {
          chr = data_buffer_[i];
          if (!is_whitespace(chr)) break;
        }

        if (i >= data_buffer_size_)
        {
          //
          // The buffer just read contains only whitespace. Since we have
          // exhausted the contents of the file, this means that we are
          // facing an end-of-file condition.
          //

          file_bytes_unread_ = 0;
          is_eof_ = true;
          return 1;
        }

        // Look for the first record in the buffer.

        status = find_record_fwd(i, record_start, record_end, type);

        //
        // If status is nonzero, this means that we have not been able to
        // find a single record in a non-empty buffer. This means that
        // the file is malformed.
        //

        if (status != 0) return 3;

        //
        // Check the type. Epochs must begin with an l-type record. If not,
        // the file is malformed too.
        //

        if (type != 'l') return 3;

        //
        // Everything is OK. Although we readjust the offset (to start at
        // the first non-whitespace character) we do not touch the amount of
        // bytes read, since we are at the beginning of the file and don't want
        // to re-read again and again the whitespace at the beginning of the
        // file.
        //

        new_offset = record_start;
        done = true;
      }
      else
      {
        //
        // The file has not been exhausted. We may not assume that the
        // first record in the chunk is the first one in a complete epoch.
        //
        // We'll use the first complete record in the internal data buffer
        // to retrieve its time tag. Once done so, we will look for the first
        // complete epoch that has a different time tag. This is the task
        // of find_first_epoch.
        //
        // Then, we will readjust the offset to guarantee that an INTEGRAL
        // number of epochs is stored in the buffer.
        //

        status = find_first_epoch(offset, new_offset);

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
          // Buffer exhausted while looking for the epoch. This means that
          // our internal buffer is too small to hold enough records at
          // a time!
          //

          return 4;
        }
        else if (status == 2)
        {
          // Malformed record found while trying to find the first epoch.
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
      //
      // Set the member controlling the position in the data buffer where actual
      // data starts.
      //
      // Additionally, set the pointer to the start of the current epoch one
      // position beyond the end of the data buffer. The next time a previous
      // epoch is requested, the search will start at epoch_end_ - 1, that is,
      // exactly at the end of the buffer.
      //
      // Finally, the global buffer pointer is set also to one position beyond
      // the last item in the data buffer.
      //

      chunk_start_index_   = offset;
      epoch_start_         = data_buffer_size_;
      data_buffer_current_ = data_buffer_size_;

      // Check if we are over.

      if (done) break;
    }

    //
    // Now, we are sure that there is data in the internal buffer. Try
    // to fetch the "previous" epoch, that is, the first one starting
    // at the end of the buffer.
    //

    status = find_prev_epoch(epoch_start_ - 1,
                             epoch_start_, epoch_end_,
                             cur_record_start_, cur_record_end_,
                             epoch_remaining_records_);

    if (status != 0) return 3; // Malformed record.

    cur_record_cur_ = cur_record_start_;

    // That's all.

    return 0;
  }
}

int
observation_file_reader_txt_backward::
read_time
(double& time)
{
  {
    int status;

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
    // Parse the data buffer. The next non-whitespace characters should
    // contain a valid double value (the time).
    //
    // Note that parse_int_value() updates the pointer cur_record_cur_.
    //

    status = parse_double_value(time);
    if (status != 0)
    {
      // The characters found do not represent a legal double value.
      return 4;
    }

    // Set flags.

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
observation_file_reader_txt_backward::
read_type
(char& record_type)
{
  {
    string      att_name;
    int         att_search_end;
    int         att_search_start;
    string      att_value;
    const char* cchr;
    int         i;
    int         instance_id;
    int         len;
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
    //      data records to identify the beginning of an epoch (when this
    //      action is taken).
    //
    //  Situations 1 and 2.1 imply that a full record has been detected and
    //  that it contents must be parsed.
    //
    //  Situation 2.2 simply means that there's no more data to read, so
    //  we are over. We must report an end of file condition and return.
    //
    //  Situations 2.3 and 3 must be reported as errors.
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
      // Buffer too small. Unable to find the first epoch in a data chunk
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

    if ((typ != 'o') & (typ != 'l')) return 4;

    //
    // Assign the record type, using only lowercase characters.
    // Set the appropriate related flags.
    //

    if (typ == 'o')
    {
      record_type = 'o';
      reading_o_  = true;
      reading_l_  = false;
    }
    else
    {
      record_type = 'l';
      reading_o_  = false;
      reading_l_  = true;
    }

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
    // For l-records: l_id (mandatory) and s (optional).
    // For o-records: o_id (mandatory) and s (optional).
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
    // It may be present in both kinds of records (l- and o-).
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
    // Look for the id attribute, present (and mandatory) in both
    // (l- and o-) records.
    //

    att_name = "id";

    // Try to find the attribute in our list.

    att_value = cur_record_attributes_[att_name];

    // Did we find it?

    if (att_value == "") return 4;

    // We've found the attribute. One less to search for.

    total_att --;

    //
    // Retrieve the value associated to the attribute
    // and set last_record_identifier_ correspondingly.
    //

    set_last_record_identifier(att_value.c_str());

    //
    // l-records have an extra (and mandatory) "n" attribute
    // (the instance identifier). Failing to find it must be
    // reported as an error.
    //

    if (record_type == 'l')
    {
      att_name = "n";

      // Try to find the attribute in our list.

      att_value = cur_record_attributes_[att_name];

      // Did we find it?

      if (att_value == "") return 4;

      // We've found the attribute. One less to search for.

      total_att --;

      //
      // The "n" attribute must be an INTEGER value. Let's try to
      // convert it from string form.
      //

      len  = (int)att_value.size();
      cchr = att_value.c_str();
  
      if ((cchr[0] != '-') & (cchr[0]!= '+') & ((cchr[0] < '0') || (cchr[0] > '9'))) return 4;
      for (i = 1; i < len; i++) if (cchr[i] < '0' || cchr[i] > '9') return 4;
      if(sscanf(cchr, "%d", &instance_id)  == EOF ) return 4;

      // Update last record read information.

      last_record_instance_id_ = instance_id;
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
