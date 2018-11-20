/** \file obs_residuals_file_reader_txt_forward.cpp 
  \brief Implementation file for obs_residuals_file_reader_txt_forward.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "obs_residuals_file_reader_txt_forward.hpp"

obs_residuals_file_reader_txt_forward::
obs_residuals_file_reader_txt_forward
(void)
{
  {
    // Intentionally left blank
  }
}

obs_residuals_file_reader_txt_forward::
~obs_residuals_file_reader_txt_forward
(void)
{
  {
    // INTENTIONALLY LEFT BLANK.
  }
}

int
obs_residuals_file_reader_txt_forward::
read_o_data
(int   n_parameter_iids,
 int*& the_parameter_iids,
 int   n_observation_iids,
 int*& the_observation_iids,
 int   n_instrument_iids,
 int*& the_instrument_iids)
{
  {

    //
    // o-records do not exist in observation residuals files. We return invalid
    // call sequence error code.
    //
    // Note that this method is provided just to comply with the
    // ASTROLABE interface.
    //

    return 3;
  }
}

int
obs_residuals_file_reader_txt_forward::
read_time
(double& time)
{
  {
    int status;

    //
    // Check preconditions. This function works only in this situation:
    //
    //   - The instance identifier has just been read.
    //

    if (!read_iid_)    return 3;
    if ( read_l_data_) return 3;


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

      // If the epoch changes, we will no more be reading the first one.

      if (last_epoch_changed_) reading_first_epoch_ = false;
    }

    // That's all.

    return 0;
  }
}

int
obs_residuals_file_reader_txt_forward::
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

    if (typ != 'l') return 4;

    //
    // Assign the record type, using only lowercase characters.
    // Set the appropriate related flags.
    //

    record_type = 'l';
    reading_o_  = false;
    reading_l_  = true;

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
    // Look for the mandatory "n" attribute.
    //

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
