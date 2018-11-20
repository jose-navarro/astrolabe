/** \file obs_e_based_socket_reader.cpp 
  \brief Implementation file for obs_e_based_socket_reader.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "obs_e_based_socket_reader.hpp"

int
obs_e_based_socket_reader::
close
(void)
{
  {
    // Just return if the socket is already closed.

    if (!socket_is_open_) return 0;

    // Delete the sockets. This will actually close the connection.

    try
    {
      if (socket_        != NULL) delete socket_;
      if (server_socket_ != NULL) delete server_socket_;

      socket_        = NULL;
      server_socket_ = NULL;
    }
    catch(SocketException &)
    {
      // Error sending the end-of-file command.

      return 1;
    }

    // Destroy the decoder objects and buffers.

    if (decoder_membuf_    != NULL) delete decoder_membuf_;
    if (decoder_formatter_ != NULL) delete decoder_formatter_;
    if (decoder_stream_    != NULL) delete decoder_stream_;
    if (data_buffer_       != NULL) delete [] data_buffer_;

    data_buffer_     = NULL;
    decoder_membuf_    = NULL;
    decoder_formatter_ = NULL;
    decoder_stream_    = NULL;

    // Update flags.

    socket_is_open_ = false;

    // That's all.

    return 0;
  }
}

bool
obs_e_based_socket_reader::
epoch_changed
(void)
{
  {
    return last_epoch_changed_;
  }
}

void
obs_e_based_socket_reader::
epoch_change_acknowledged
(void)
{
  {
    last_epoch_changed_ = false;
  }
}

obs_e_based_socket_reader::
obs_e_based_socket_reader
(void)
{
  {
    data_buffer_                = NULL;
    data_buffer_size_           = _EVENT_SOCKET_READER_DEFAULT_BUFFER_SIZE;

    decoder_membuf_             = NULL;
    decoder_formatter_          = NULL;
    decoder_stream_             = NULL;

    last_epoch_changed_         = false;
    last_epoch_time_available_  = false;
    last_epoch_time_            = 0;

    last_record_is_active_      = false;

    first_l_read_               = false;

    is_eof_                     = false;

    o_records_allowed_          = true;

    read_active_flag_           = false;
    read_completed_             = true;
    read_identifier_            = false;
    read_iid_                   = false;
    read_l_data_                = false;
    read_o_data_                = false;
    read_time_                  = false;
    read_type_                  = false;

    reading_first_epoch_        = true;

    reading_l_                  = false;
    reading_o_                  = false;

    server_socket_              = NULL;
    socket_                     = NULL;
    socket_is_open_             = false;
  }
}

obs_e_based_socket_reader::
~obs_e_based_socket_reader
(void)
{
  {
    if (socket_is_open_) close();
  }
}

void
obs_e_based_socket_reader::
get_full_record
(void)
{
  {
    int  bytesReceived; // Total bytes read

    //
    // Get a full data record. These are sent at once.
    //
    // Note the zero (0) in the next while clause. It means "end of transmission".
    //

    while ((bytesReceived = socket_->recv(data_buffer_, data_buffer_size_)) > 0)
    {
      //
      // Once inside this loop, we've received a full record.
      //
      // Let's send an acknowledgement message. This message, according
      // to the protocol, is just a single character (1 byte), which
      // does not need to be encoded. It does not matter what the
      // contents of this character is (the protocol says so), so
      // we'll send the first character received in our buffer...

      socket_->send(data_buffer_, 1);

      //
      // And now, we leave the loop, since whe are interested in reading
      // a single record only.
      //

      break;
    }

    // That's all!

    return;
  }
}

bool
obs_e_based_socket_reader::
is_eof
(void)
const
{
  {
    return is_eof_;
  }
}

int
obs_e_based_socket_reader::
open
(const char* host,
 int         port,
 bool        server_mode)
{
  {
    // Won't accept open requests if the file is already open.

    if (socket_is_open_) return 1;

    //
    // We won't work neither if the selected port is out of
    // range.
    //

    if ((port < 1) || (port > 65535)) return 2;

    //
    // If we behave as a client socket (server_mode == false) then
    // we must check that we've got a servers' name or IP address.
    //

    if ((!server_mode) && (host == NULL)) return 2;

    // Try to create the buffer used to receive data.

    try
    {
      data_buffer_ = new char[data_buffer_size_];

      decoder_membuf_     = new mem_buffer(data_buffer_, data_buffer_size_);
      decoder_formatter_  = new XDR_format<mem_buffer>(*decoder_membuf_);
      decoder_stream_     = new obj_input<XDR_format<mem_buffer> >(*decoder_formatter_);
    }
    catch (...)
    {
      return 3;
    }

    //
    // Depending on how we must behave (client or server) create a
    // client or server socket.
    //

    if (server_mode)
    {
      // SERVER MODE.

      // Try to create the server socket and make it accept a connection.

      try
      {
        server_socket_ = new TCPServerSocket((unsigned short)port);
        socket_ = server_socket_->accept();
      }
      catch (SocketException &)
      {
        return 2;
      }
    }
    else
    {
      // CLIENT MODE.

      //
      // Try to open the connection. Note that this may
      // fail either because the host is not valid or there's no one
      // listening at the other end!
      //

      try
      {
        socket_ = new TCPSocket(host, (unsigned short)port);
      }
      catch (SocketException &)
      {
        // Error opening the connection.

        return 2;
      }
    }

    // Update flags.

    socket_is_open_ = true;

    // Successful completion.

    return 0;
  }
}

int
obs_e_based_socket_reader::
read_active_flag
(bool& active)
{
  {
    char cactive[2];
    //
    // Check preconditions. This function works only when (1) the type
    // tag has been read and (2) no other component in the record has
    // been read neither.
    //

    if (!read_type_)        return 3;
    if ( read_active_flag_) return 3;

    // Get the active flag (a char with two possible values, "0" and "1").

    try
    {
      decoder_stream_->cstring(cactive, 1);
    }
    catch (...)
    {
      // Problems while decoding the flag. We'll report read errors.

      return 2;
    }

    // Set parameter active using the cactive char just read.

    active = (cactive[0] == '1');

    // Update last record active flag.

    last_record_is_active_ = active;

    // Set flags.

    read_active_flag_ = true;

    // That's all.

    return 0;
  }
}

int
obs_e_based_socket_reader::
read_identifier
(char*& identifier,
 int    id_len)
{
  {
    int   sizeof_identifier;

    // Check that the socket is open!

    if (!socket_is_open_) return 2;

    //
    // Check preconditions. This function works only when (1) the active
    // flag has been read, (2) no other component in the record has
    // been read neither, (3) the buffer for the identifier is non-null.
    //

    if (!read_active_flag_) return 3;
    if ( read_identifier_)  return 3;
    if (identifier == NULL) return 3;

    //
    // Retrieve the size of the identifier from the input stream.
    // It is an int value.
    //

    try
    {
      decoder_stream_->simple(sizeof_identifier);
    }
    catch (...)
    {
      //
      // Problems decoding the size of the identifier. We'll report read
      // errors.
      //

      return 2;
    }
    //
    // If the size of the identifier just obtained exceeds that of the
    // identifier parameter, we must report an error.
    //

    if (id_len <= sizeof_identifier) return 4;

    // Get the identifier (sizeof_identifier chars).

    try
    {
      decoder_stream_->cstring(identifier, sizeof_identifier);
    }
    catch (...)
    {
      // Errors decoding the identifier. We'll report read errors.

      return 2;
    }

    // Update flags.

    read_identifier_ = true;

    // That's all.

    return 0;
  }
}

int
obs_e_based_socket_reader::
read_l_data
(int      n_tag_values,
 double*& the_tags,
 int      n_expectation_values,
 double*& the_expectations,
 int&     n_covariance_values_found,
 double*& the_covariance_values)
{
  {
    int   n;
    int   n_of_elements;

    // Check that the socket connection is open!

    if (!socket_is_open_)          return 2;

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

    try
    {

      //
      // Get the number of tags. If it doesn't match with
      // n_tag_values, we've found an error.
      //

      decoder_stream_->simple(n_of_elements);
      if (n_of_elements != n_tag_values) return 7;

      // Get the tags themselves, if any (there may be zero).

      if (n_tag_values > 0) decoder_stream_->vector(the_tags, n_tag_values);

      //
      // Get the number of expectations. If it doesn't match with
      // n_expectation_values, we've found an error.
      //

      decoder_stream_->simple(n_of_elements);
      if (n_of_elements != n_expectation_values) return 7;

      // Get the expectations themselves.

      decoder_stream_->vector(the_expectations, n_expectation_values);

      //
      // Get the number of covariance matrix values. If it doesn't
      // adhere to the rules, we've found an error.
      //

      n = n_expectation_values;

      decoder_stream_->simple(n_of_elements);

      if ((n_of_elements != 0) &
          (n_of_elements != n) &
          (n_of_elements != (n * (n + 1))/2)) return 7;

      n_covariance_values_found = n_of_elements;

      // Get the covariance values themselves, if any (there may be zero).

      if (n_covariance_values_found > 0)
        decoder_stream_->vector(the_covariance_values, n_covariance_values_found);

    }
    catch (...)
    {
      //
      // Error decoding either the number of elements in the
      // array or the array itself. Report an read error condition.
      //

      return 2;
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
obs_e_based_socket_reader::
read_instance_id
(int& instance_identifier)
{
  {
    // Check that the socket connection is open!

    if (!socket_is_open_) return 2;

    //
    // Check preconditions. This function works only when (1) the identifier
    // has been read, (2) no other component in the record has
    // been read neither and (3) we are reading an l-type record.
    //

    if (!read_identifier_) return 3;
    if ( read_iid_)        return 3;
    if (!reading_l_)       return 3;

    try
    {
      // Get the id (an integer).

      decoder_stream_->simple(instance_identifier);
    }
    catch (...)
    {
      // Problems decoding the identifier. Report a read error condition.
      return 2;
    }

    // Update flags.

    read_iid_ = true;

    // That's all.

    return 0;
  }
}

int
obs_e_based_socket_reader::
read_o_data
(int   n_parameter_iids,
 int*& the_parameter_iids,
 int   n_observation_iids,
 int*& the_observation_iids,
 int   n_instrument_iids,
 int*& the_instrument_iids)
{
  {
    int n_pars_found;
    int n_obs_found;
    int n_inst_found;

    // Check that the socket connection is open!

    if (!socket_is_open_) return 2;

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
    // Try to get the data. This is done thrice; each time we
    // get the number of elements (parameters, observation or
    // instrument instance identifiers) and then the list
    // of identifiers themselves.

    try
    {

      //
      // Parameter instance identifiers.
      //

      // Get the amount of these.

      decoder_stream_->simple(n_pars_found);
      if (n_pars_found != n_parameter_iids) return 7;

      // Get the list of identifiers.

      decoder_stream_->vector(the_parameter_iids, n_pars_found);

      //
      // Observation instance identifiers.
      //

      // Get the amount of these.

      decoder_stream_->simple(n_obs_found);
      if (n_obs_found != n_observation_iids) return 7;

      // Get the list of identifiers.

      decoder_stream_->vector(the_observation_iids, n_obs_found);

      //
      // Instrument instance identifiers, if any.
      //

      // Get the amount of these.

      decoder_stream_->simple(n_inst_found);
      if (n_inst_found != n_instrument_iids) return 7;

      // Get the list of identifiers, if any!

      if (n_inst_found > 0) decoder_stream_->vector(the_instrument_iids, n_inst_found);
    }
    catch (...)
    {
      //
      // Problems decoding incoming data. Report a read error condition.
      //

      return 2;
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
obs_e_based_socket_reader::
read_time
(double& time)
{
  {
    // Check that the socket connection is open!

    if (!socket_is_open_) return 2;

    //
    // Check preconditions. This function works only in the following
    // situations:
    //
    //   - if reading an l-type record, the instance identifier and no other further component
    //     of the record must have been read.
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

    try
    {
      // Get the time (a double).

      decoder_stream_->simple(time);
    }
    catch (...)
    {
      // Error decoding the time. Report this as a read error condition.

      return 2;
    }


    // Update flags.

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
obs_e_based_socket_reader::
read_type
(char& record_type)
{
  {
    char the_type[2];

    // Check that the socket connection is open!

    if (!socket_is_open_) return 2;

    //
    // Check preconditions. This function works only if no other
    // read operation is on its way.
    //

    if (!read_completed_) return 3;

    //
    // Records are sent completeley at once. Since these records START
    // with the record type (l, o, or end-of-file command) this method
    // must take care of reading the full record here and buffer it
    // for later use of the different read_xxx methods in this class.
    //
    // Note that this is the place to detect the end-of-file message,
    // since it arrives stored in the same place that the record type.
    //

    try
    {
      //
      // get_full_record() stores the data read in the underlying
      // buffer used by our decoders (data_buffer_).
      //
      // get_full_record(), by the way, reads data from the socket
      // connection!
      //

      get_full_record();
    }
    catch (...)
    {
      // Problems reading the socket. We'll report a read error condition.
      return 2;
    }

    // Let's decode the type. Beware: it may be the end-of-file command!

    try
    {
      decoder_membuf_->rewind(); // So we start again from the beginning of the buffer.
      decoder_stream_->cstring(the_type, 1);
    }
    catch (...)
    {
      // Error decoding the type. We'll report a read error condition.
      return 2;
    }

    // Have we received the "end-of-file" command?

    if (the_type[0] == 'e')
    {
      // End of transmission!!!

      is_eof_ = true;
      return 1;
    }

    // Copy the type just decoded.

    record_type = the_type[0];

    // Check that a correct type tag has been read.

    if ((record_type != 'o') & (record_type != 'l')) return 4;

    // Check that we're not getting an o-record if it is not allowed.

    if ((record_type == 'o') & (!o_records_allowed_)) return 4;

    // Update flags.


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
