/** \file r_matrix_based_file_reader_txt.cpp 
  \brief Implementation file for r_matrix_based_file_reader_txt.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_based_file_reader_txt.hpp"

r_matrix_based_file_reader_txt::
r_matrix_based_file_reader_txt
(void)
{
  {
    cur_record_cur_               = -1;
    cur_record_end_               = -1;
    cur_record_start_             = -1;

    text_buffer_size_             = 100; // Initial guess.
    text_buffer_                  = new char[text_buffer_size_];
  }
}

r_matrix_based_file_reader_txt::
~r_matrix_based_file_reader_txt
(void)
{
  {
    delete text_buffer_;
    text_buffer_ = NULL;
  }
}

void
r_matrix_based_file_reader_txt::
guarantee_text_buffer_size
(int size)
{
  {
    if (text_buffer_size_ >= size) return;

    text_buffer_size_ = size;

    delete text_buffer_;
    text_buffer_ = new char[text_buffer_size_];
  }
}

bool
r_matrix_based_file_reader_txt::
is_whitespace
(char chr)
{
  {
    if (chr == ' ' ) return true;
    if (chr == '\t') return true;
    if (chr == '\n') return true;
    if (chr == '\r') return true;

    return false;
  }
}

int
r_matrix_based_file_reader_txt::
parse_attributes
(char* buffer, int parsefrom, int parseto,
 map<string,string>& attributes)
{
  {
    int chars_to_copy;
    int copyfrom;
    int copyto;
    int i;

    // Check limits and non-null input buffer.

    if (parsefrom > parseto) return 1;
    if (buffer == NULL)      return 1;

    //
    // Reset the contents of the attributes map, just in case it's been
    // used before by the calling module.
    //

    attributes.clear();

    // Start parsing.

    i = parsefrom;

    // Parse all attribute pair.

    while (i <= parseto)
    {

      //
      // Skip all leading whitespace before the attribute name.
      // Note that if the buffer is exhausted at this point, it
      // means that no more attribute pairs exist, so reaching
      // the end of such buffers implies no error, but just the
      // end of our work...
      //

      while ((i <= parseto) && (is_whitespace(buffer[i]))) i++;
      if (i > parseto) return 0;

      //
      // Here, i points, presumably, to the first non-whitespace character,
      // the beginning of the attribute name and our starting copy
      // position.
      //

      copyfrom = i;

      //
      // Check that we have a non '=' and a non '"' char (our XML delimiters
      // for attributes).
      //

      if ((buffer[copyfrom] == '=') ||
          (buffer[copyfrom] == '"')) return 1;

      //
      // Nice! We have at copyfrom what seems to be the beginning of the
      // attribute name.
      //
      // Look for its end. We'll start right after the current character,
      // which has already been tested.
      //

      i++;

      while ((i <= parseto) && (!is_whitespace(buffer[i])) && (buffer[i] != '='))
      {
        if (buffer[i] == '"') return 1;
        i++;
      }
      if (i > parseto) return 1;

      // Here, i points to the first whitespace or '=' character. Just
      // before this position is where our attribute name finishes.
      //

      copyto = i-1;

      // Compute the length of the attribute value.

      chars_to_copy = copyto - copyfrom + 1;

      // Copy the attribute name to a temporal string.

      string attr_name(&(buffer[copyfrom]), (size_t)(chars_to_copy));

      //
      // Skip all whitespace until we find the character '='. Note that
      // we could be already positioned over the '=' char, if the previous
      // look stopped by this reason.
      //

      while (i <= parseto)
      {
        if (buffer[i] == '=') break;
        if (!is_whitespace(buffer[i])) return 1;

        i++;
      }
      if (i > parseto) return 1;

      //
      // At this point, i points to the '=' character. Make it
      // point right after that.
      //

      i++;

      //
      // Skip all whitespace until we find the character '"', which
      // delimites the beginning of the attribute value.
      //

      while (i <= parseto)
      {
        if (buffer[i] == '"') break;
        if (!is_whitespace(buffer[i])) return 1;

        i++;
      }
      if (i > parseto) return 1;

      //
      // Here, i points to the '"' character. Skip it, so
      // we get positioned just after it.
      //

      i++;

      //
      // This is the position where the attribute value should begin,
      // and therefore our starting position for the later copy operation.
      //

      copyfrom = i;

      //
      // Keep scanning until we find another '"' char. This will
      // be the value of the attribute. Note that we start
      // scanning at position copyfrom, since a couple of consecutive
      // quotes ("") could show up, which would be an error. If
      // we skip this fisrt position, we could inadvertedly ignore
      // this erroneous situation.
      //

      while ((i <= parseto) && (buffer[i] != '"')) i++; 
      if (i > parseto) return 1;

      //
      // i points to the second '"' character.
      // We must copy just before that position.
      //

      copyto = i-1;

      // Compute the length of the string to copy.

      chars_to_copy = copyto - copyfrom + 1;

      //
      // Test that this length is not zero (this would happen if
      // a couple of consecutive quotes ("") is what we've found.
      //

      if (chars_to_copy < 1) return 1;

      // Convert the attribute value to string.

      string attr_value(&(buffer[copyfrom]), size_t(chars_to_copy));

      // Insert the attribute name + value pair in our output map.

      attributes.insert(pair<string,string>(attr_name, attr_value));

      //
      // Make i point right after the '"' char, ready to start
      // the parsing of a new attribute name / value pair.
      //

      i++;

    }

    return 0;
  }
}

int
r_matrix_based_file_reader_txt::
parse_closing_r_tag
(void)
{
  {

    char lctag[] = "</r>";
    char uctag[] = "</R>";

    char chr;
    int  i;
    int  j;
    int  text_size;

    //
    // Try to skip all whitespace characters starting at the current
    // record position.
    //

    chr = data_buffer_[cur_record_cur_];

    while (is_whitespace(chr))
    {
      cur_record_cur_++;
      chr = data_buffer_[cur_record_cur_];
    }

    //
    // At this point, the only non-whitespace remaining characters must
    // be these of the closing tag.
    //
    // Note, however, that there could be some whitespace merged with
    // the tag itself, as for instance "< /r  >" (quotes excluded).
    //
    // Therefore, the first thing to do is to copy the remaining characters
    // in the buffer but excluding whitespace. To do it, we'll use our
    // internal text buffer, text_buffer_. We must guarantee that this buffer
    // is big enough to hold the remaining characters in the current
    // record.
    //

    text_size = cur_record_end_ - cur_record_cur_ + 1;

    guarantee_text_buffer_size(text_size + 1); // Includes final NULL character.

    j = 0;
    for (i = cur_record_cur_; i <= cur_record_end_; i++)
    {
      chr  = data_buffer_[i];

      if (!is_whitespace(chr))
      {
        text_buffer_[j] = chr;
        j++;
      }
    }

    text_buffer_[j] = 0; // Ending NULL character.

    //
    // Update cur_record_cur_ to denote that we've processed the current
    // record completely.
    //

    cur_record_cur_ = cur_record_end_ + 1; // Beyond the record limits.

    //
    // Now that the closing tag has been copied to text_buffer_ excluding
    // all whitespace, it is possible to compare it with the reference
    // closing tags (lctag and uctag parameters).
    //

    if (strcmp(text_buffer_, lctag) != 0)
    {
      // Lowercase comparison failed. Check uppercase.

      if (strcmp(text_buffer_, uctag) != 0)
      {
        // Uppercase comparison failed too. We have a malformed record!

        return 1;
      }
    }

    // We've found the sought closing tag.

    return 0;

  }
}

int
r_matrix_based_file_reader_txt::
parse_double_value
(double& dvalue)
{
  {
    char   chr;
    void*  destination;
    void*  source;
    int    text_end;
    int    text_size;
    int    text_start;

    //
    // Skip any whitespace that might be present before the text representing
    // the double value.
    //

    chr = data_buffer_[cur_record_cur_];

    while (is_whitespace(chr))
    {
      cur_record_cur_++;
      chr = data_buffer_[cur_record_cur_];
    }

    text_start = cur_record_cur_;

    //
    // Skip all non-whitespace characters. These are the characters that
    // presumably may be interpreted as a double.
    //
    // Note that the loop is also aborted if an opening < is found. This
    // specific test is made to deal with the situation in which the last
    // double in a record is adjacent to the closing tag, thus allowing
    // no spaces between these two elements.
    //

    while ((!is_whitespace(chr)) && (chr != '<'))
    {
      cur_record_cur_++;
      chr = data_buffer_[cur_record_cur_];
    }

    text_end = cur_record_cur_ - 1;
    text_size = text_end - text_start + 1;

    //
    // Guarantee that our internal text buffer is big enough to hold the
    // set of characters just read.
    //

    guarantee_text_buffer_size(text_size + 1); // Includes final NULL character.

    // Copy the text in the data buffer (source) to our local copy (destination).

    source      = (void*)&(data_buffer_[text_start]);
    destination = (void*)text_buffer_;

    memcpy(destination, source, (size_t)text_size);
    text_buffer_[text_size] = 0; // Ending NULL character.

    // Try to convert the text just obtained to a binary double value.

    if (sscanf(text_buffer_, "%lf", &dvalue) == EOF)
    {
      // The text read does not represent a legal double value!!!
      return 1;
    }

    // That's all.

    return 0;
  }
}

int
r_matrix_based_file_reader_txt::
parse_int_value
(int& ivalue)
{
  {
    char  chr;
    void* destination;
    void* source;
    int   text_end;
    int   text_size;
    int   text_start;

    //
    // Skip any whitespace that might be present before the text representing
    // the integer value.
    //

    chr = data_buffer_[cur_record_cur_];

    while (is_whitespace(chr))
    {
      cur_record_cur_++;
      chr = data_buffer_[cur_record_cur_];
    }

    text_start = cur_record_cur_;

    //
    // Skip all digit characters. These are the the ones that
    // presumably may be interpreted as an integer.
    //

    while ((chr >= '0')  && (chr <= '9')) 
    {
      cur_record_cur_++;
      chr = data_buffer_[cur_record_cur_];
    }

    text_end = cur_record_cur_ - 1;
    text_size = text_end - text_start + 1;

    //
    // Guarantee that our internal text buffer is big enough to hold the
    // set of characters just read.
    //

    guarantee_text_buffer_size(text_size + 1); // Includes final NULL character.

    // Copy the text in the data buffer (source( to our local copy (destination).

    source      = (void*)&(data_buffer_[text_start]);
    destination = (void*)text_buffer_;

    memcpy(destination, source, (size_t)text_size);
    text_buffer_[text_size] = 0; // Ending NULL character.

    // Try to convert the text just obtained to a binary integer value.

    if (sscanf(text_buffer_, "%d", &ivalue) == EOF)
    {
      //  The text read does not represent a legal integer value!!!
      return 1;
    }

    // That's all.

    return 0;
  }
}

int
r_matrix_based_file_reader_txt::
read_active_flag
(bool& active)
{
  {
    //
    // Check preconditions. This function works only when (1) the type
    // tag has been read and (2) no other component in the record has
    // been read neither.
    //

    if (!read_type_)        return 3;
    if ( read_active_flag_) return 3;

    //
    // The active flag has been read, in fact, by read_type() because it
    // is included in the attribute list of the opening tag. Therefore,
    // the only thing we have to do is to retrieve it from the
    // last_record_is_active_ attribute, set by that method.
    //

    active = last_record_is_active_;

    // Set flags.

    read_active_flag_ = true;

    // That's all.

    return 0;
  }
}

int
r_matrix_based_file_reader_txt::
read_r_data
(int &    n_correlation_values_found,
 double*& the_correlation_values)
{
  {
    double dval;
    int    status;

    //
    // Check preconditions. This function works only when the time
    // tag has been read.
    //

    if (!read_time_)               return 3;
    if ( read_completed_)          return 3;

    // Parse as much correlation values as possible.

    n_correlation_values_found = 0;
    status = parse_double_value(dval);
    if ((dval > 1.0) | (dval < -1.0)) return 4;

    while (status == 0)
    {
      the_correlation_values[n_correlation_values_found] = dval;

      if ((dval > 1.0) | (dval < -1.0)) return 4;

      n_correlation_values_found++;
      status = parse_double_value(dval);
    }

    //
    // Check that the characters after the list of doubles just found
    // are only whitespace followed by the closing r-tag.
    //

    status = parse_closing_r_tag();

    if (status != 0)
    {
      // The remaining characters are invalid. We've got a malformed l-record.
      return 5;
    }

    //
    // Set flags to start again.
    //

    read_type_             = false;
    read_active_flag_      = false;
    read_time_             = false;
    read_r_data_           = false;

    read_completed_        = true;

    // That's all.

    return 0;
  }
}

int
r_matrix_based_file_reader_txt::
read_time
(double& time)
{
  {
    int status;

    //
    // Check preconditions. This method may be called only after read_active_flag()
    // has been invoked.
    //

    if (!read_active_flag_)  return 3;
    if (read_r_data_)        return 3;

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

    // That's all.

    return 0;
  }
}
