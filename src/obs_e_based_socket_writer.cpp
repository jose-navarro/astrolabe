/** \file obs_e_based_socket_writer.cpp 
  \brief Implementation file for obs_e_based_socket_writer.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "obs_e_based_socket_writer.hpp"

int
obs_e_based_socket_writer::
close
(void)
{
  {
    const char* eof_command = "e";

    // Just return if the socket is already closed.

    if (!socket_is_open_) return 0;

    //
    // Send the "end-of-file" (EOF) command to the receiver, so it's
    // aware that we won't talk anymore.
    //

    // First, encode the command using the XDR standard.

    try
    {
      coder_membuf_->rewind();
      coder_stream_->cstring(eof_command, 1);
    }
    catch (...)
    {
      // Error writing.

      return 2;
    }

    //
    // Now send the encoded EOF command. The pointer to the encoded info is
    // obtained calling the data() member of our membuf. The number
    // of bytes used by our encoded data is retrieved using method
    // size() in this very same object.
    //

    try
    {
      // Send the EOF command.
      socket_->send(coder_membuf_->data(), coder_membuf_->size());

      // Wait for the acknowledgement sent by the receiver.
      getACK();

    }
    catch(SocketException &)
    {
      // Error sending the end-of-file command.

      return 2;
    }

    // Delete the socket. This will actually close the connection.

    try
    {
      if (socket_        != NULL) delete socket_;
      if (server_socket_ != NULL) delete server_socket_;

      socket_        = NULL;
      server_socket_ = NULL;
    }
    catch(SocketException &)
    {
      // Error closing the socket connection.

      return 1;
    }

    // Destroy the encoder objects and buffers.

    if (coder_membuf_    != NULL) delete coder_membuf_;
    if (coder_formatter_ != NULL) delete coder_formatter_;
    if (coder_stream_    != NULL) delete coder_stream_;
    if (data_buffer_     != NULL) delete [] data_buffer_;

    data_buffer_     = NULL;
    coder_membuf_    = NULL;
    coder_formatter_ = NULL;
    coder_stream_    = NULL;

    // Update flags.

    socket_is_open_ = false;

    // That's all.

    return 0;
  }
}

obs_e_based_socket_writer::
obs_e_based_socket_writer
(void)
{
  {

    coder_membuf_        = NULL;
    coder_formatter_     = NULL;
    coder_stream_        = NULL;
    data_buffer_         = NULL;
    data_buffer_size_    = _EVENT_SOCKET_WRITER_DEFAULT_BUFFER_SIZE;
    first_l_written_     = false;
    first_time_writing_  = true;
    host_                = "";
    last_time_tag_       = 0.0;
    o_records_allowed_   = true;
    port_                = 0;
    server_socket_       = NULL;
    socket_              = NULL;
    socket_is_open_      = false;

  }
}

obs_e_based_socket_writer::
~obs_e_based_socket_writer
(void)
{
  {
    if (socket_is_open_) close();
  }
}

void
obs_e_based_socket_writer::
getACK
(void)
{
  {
    char echoBuffer[1];      // Buffer for 1 acknowledgement character.
    int  bytesReceived;      // Bytes read on each recv()
    int  totalBytesReceived; // Total bytes read

    bytesReceived = 0;
    totalBytesReceived = 0;  


    while (totalBytesReceived < 1)
    {
      // Receive up to the buffer size bytes from the sender

      if ((bytesReceived = (socket_->recv(echoBuffer, 1))) <= 0)
      {
        throw SocketException("Error reading acknowledgement character"); // Unable to read.
      }

      totalBytesReceived += bytesReceived;  // Keep tally of total bytes
    }

    return;
  }
}

bool
obs_e_based_socket_writer::
is_open
(void)
const
{
  {
    return socket_is_open_;
  }
}

int
obs_e_based_socket_writer::
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

    //
    // Since everything is OK, we'll copy the input parameters to
    // our own members. Note that host may be NULL, but only in
    // server mode, which is NOT a problem.
    //

    host_ = host;
    port_ = (unsigned short)port;

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
        server_socket_ = new TCPServerSocket(port_);
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
        socket_ = new TCPSocket(host_, port_);
      }
      catch (SocketException &)
      {
        // Error opening the connection.

        return 2;
      }
    }

    // Try to create the buffer used for block reads.

    try
    {
      //
      // Instantiate a data buffer with the proper size.
      //

      data_buffer_     = new char[data_buffer_size_];
      socket_is_open_  = true;

      // Set up the stack of XDR objects needed to write XDR coded data.

      coder_membuf_    = new mem_buffer(data_buffer_, data_buffer_size_);
      coder_formatter_ = new XDR_format<mem_buffer>(*coder_membuf_);
      coder_stream_    = new obj_output<XDR_format<mem_buffer> >(*coder_formatter_);

    }
    catch (...)
    {
      //
      // If there's not enough memory, close again the file, flag it
      // as not open and return the error code.
      //

      close();
      socket_is_open_ = false;
      return 3;
    }

    // Successful completion.

    return 0;
  }
}

int
obs_e_based_socket_writer::
write_l
(bool         active,
 char*        identifier,
 int          instance_identifier,
 double       time,
 int          n_tags,
 double*      buffer_tags,
 int          n_expectations,
 double*      buffer_expectations,
 int          n_covariance_values,
 double*      buffer_covariance)
{
  {
    int sizeof_identifier;

    // We need an open socket connection to work.

    if (!socket_is_open_)      return 1;

    // identifier must be at least 1 char long excluding the terminating NULL char.

    if (identifier == NULL)    return 4;

    sizeof_identifier = (int)strlen(identifier);
    if (sizeof_identifier < 1) return 4;

    // The dimensions of the several double arrays must be correct.

    if (n_tags < 0)            return 5;
    if (n_expectations <= 0)   return 5;
    if ((n_covariance_values != 0)              &
        (n_covariance_values != n_expectations) &
        (n_covariance_values != (n_expectations*(n_expectations+1))/2))
                               return 5;

    //
    // Check if this is the first time we write to the socket. If so, we
    // must give an initial value to the last time tag in use.
    //
    // Note that inactive records do not change flags at all. We
    // simply write to the socket, but it is as if we wouldn't
    // written nothing at all from the flags standpoint.
    //

    if (active && first_time_writing_)
    {
      first_time_writing_ = false;
      last_time_tag_      = time;
    }

    //
    // Check if an epoch change is just arriving... Inactive records do
    // not change flags.
    //

    if (active && (time != last_time_tag_))
    {
      // We've got a new last_time_tag_ to compare to.

      last_time_tag_ = time;

      //
      // Update the first_l_written_ tag. This is not strictly necessary,
      // since we are writing an l-record here and this means that if we've
      // been the first write_x method called after an epoch change, it is
      // OK (an l-record must be the first one in an epoch). However,
      // the next assignment is made for clarity reasons. Later on, at
      // the end of the method, this flag is set again to its proper value, true,
      // since, obviously, an l-record will have been written as the first
      // one in an epoch.
      //

      first_l_written_ = false;
    }

    //
    // Write the l-record to the socket.
    //
    // Note that ALL information is coded using the XDR standard.
    //
    // The actual information (always coded) sent is the following:
    //
    // - Type - a single "l" character (denoting this one is an l-record).
    // - Active flag. A single character, either a '1' (active) or a
    //   '0' (removed)
    // - Length of the identifier, as an integer
    // - The identifier itself.
    // - The instance identifier, as an integer.
    // - The time tag, as a double.
    // - The number of tags, as an integer.
    // - The array of tags, but only if n_tags > 0, as n_tags double values.
    // - The number of expectations (always > 0), as an integer.
    // - The array of expectations, as n_expectations double values.
    // - The number of covariance values (which may be zero), as an integer.
    // - The covariance array, but only if n_covariance_values > 0, as
    //   n_covariance_values double values.
    //

    try
    {

      // Reset the memory buffer in the XDR stream so we start from scratch.

      coder_membuf_->rewind();

      // The type.

      coder_stream_->cstring("l", 1);

      // The active flag.

      if (active) coder_stream_->cstring("1",1);
      else        coder_stream_->cstring("0",1);

      // Length of the identifier.

      coder_stream_->simple(sizeof_identifier);

      // The identifier itself.

      coder_stream_->cstring(identifier, sizeof_identifier);

      // The instance identifier.

      coder_stream_->simple(instance_identifier);

      // The time tag.

      coder_stream_->simple(time);

      // The number of tags.

      coder_stream_->simple(n_tags);

      // The tags themselves, providing there are any.

      if (n_tags > 0) coder_stream_->vector(buffer_tags, n_tags);

      // The number of expectation values, always > 0.

      coder_stream_->simple(n_expectations);

      // The expectations themselves.

      coder_stream_->vector(buffer_expectations, n_expectations);

      // The number of covariance matrix values.

      coder_stream_->simple(n_covariance_values);

      // The covariance matrix values themselves, providing there are any.

      if (n_covariance_values > 0) coder_stream_->vector(buffer_covariance, n_covariance_values);

    }
    catch (...)
    {
      // Problems encoding data. We'll report problems writing.

      return 2;
    }

    //
    // Al information has been encoded. It is stored in coder_membuf_, and
    // it is accessible using its data() method. The size of the information
    // thus stored may be retrieved using the size() method.
    //
    // Send the encoded data!!!
    //

    try
    {
      // Send data.
      socket_->send(coder_membuf_->data(), coder_membuf_->size());

      // Wait for the acknowledgement sent by the receiver.
      getACK();
    }
    catch(SocketException &)
    {
      // Error writing (sending) data.

      return 2;
    }

    //
    // If we've just written an ACTIVE l-record, we don't need extra
    // l-records the next time data is written, so o-records are legal now.
    // Note that inactive records will never change flags.
    //

    if (active) first_l_written_ = true;

    // That's all.

    return 0;
  }
}

int
obs_e_based_socket_writer::
write_o
(bool         active,
 char*        identifier,
 double       time,
 int          n_parameter_iids,
 int*         the_parameter_iids,
 int          n_observation_iids,
 int*         the_observation_iids,
 int          n_instrument_iids,
 int*         the_instrument_iids)
{
  {
    int   sizeof_identifier;

    // Won't work if no o-records are allowed.

    if (!o_records_allowed_) return 3;

    // Won't work if the socket is not ready.

    if (!socket_is_open_) return 1;

    //
    // Won't work neither if no l-records have been written yet for the
    // current time tag. This test, however, makes sense only when
    // writing ACTIVE o-records. Inactive ones are always written,
    // no matter what the situation is.
    //

    if (active && (!first_l_written_)) return 3;

    // identifier must contain something.

    if (identifier == NULL) return 4;

    sizeof_identifier = (int)strlen(identifier);
    if (sizeof_identifier < 1) return 4;

    //
    // We'll still refuse to do anything if the number of identifier does not
    // match with what we're expecting.
    //

    if (n_parameter_iids   <= 0) return 7;
    if (n_observation_iids <= 0) return 7;
    if (n_instrument_iids  <  0) return 7;

    //
    // Check if an epoch change is just arriving... Here, the problem
    // of the no l-records written could arise too.
    //
    // Note that this only matters when the record is active. Otherwise,
    // we simply write data but change NO flags at all.
    //

    if (active && (time != last_time_tag_))
    {
      //
      // Ooops! This is the first record in an epoch and we are trying to
      // write an o-record. This is an error!
      //

      return 3;
    }

    //
    // Write the o-record to the socket.
    //
    // Note that ALL information is coded using the XDR standard.
    //
    // The actual information (always coded) sent is the following:
    //
    // - Type - a single "o" character (denoting this one is an o-record).
    // - Active flag. A single character, either a '1' (active) or a
    //   '0' (removed)
    // - Length of the identifier, as an integer
    // - The identifier itself.
    // - The time tag, as a double.
    // - The number of parameter instance identifiers, as an integer.
    // - The array of parameter instance identifiers, which contains
    //   n_parameter_iids int values
    // - The number of observation instance identifiers, as an integer.
    // - The array of observation instance identifiers, which contains
    //   n_observation_iids int values
    // - The number of instrument instance identifiers, as an integer. This
    //   value may be zero.
    // - If the number of instrument instance identifiers is positive, then
    //   the array of parameter instance identifiers, which contains
    //   n_parameter_iids int values
    //

    try
    {

      // Reset the memory buffer in the XDR stream so we start from scratch.

      coder_membuf_->rewind();

      // The type.

      coder_stream_->cstring("o", 1);

      // The active flag.

      if (active) coder_stream_->cstring("1",1);
      else        coder_stream_->cstring("0",1);

      // Length of the identifier.

      coder_stream_->simple(sizeof_identifier);

      // The code itself.

      coder_stream_->cstring(identifier, sizeof_identifier);

      // The time tag.

      coder_stream_->simple(time);

      // The number of parameter instance identifiers.

      coder_stream_->simple(n_parameter_iids);

      // The list of parameter instance identifiers

      coder_stream_->vector(the_parameter_iids, n_parameter_iids);

      // The number of observation instance identifiers.

      coder_stream_->simple(n_observation_iids);

      // The list of observation instance identifiers

      coder_stream_->vector(the_observation_iids, n_observation_iids);

      // The number of instrument instance identifiers.

      coder_stream_->simple(n_instrument_iids);

      // The list of instrument instance identifiers, if any at all.

      if (n_instrument_iids > 0) coder_stream_->vector(the_instrument_iids, n_instrument_iids);

    }
    catch (...)
    {
      // Problems encoding the data. We'll report writing problems.

      return 2;
    }

    //
    // Al information has been encoded. It is stored in coder_membuf_, and
    // it is accessible using its data() method. The size of the information
    // thus stored may be retrieved using the size() method.
    //
    // Send the encoded data!!!
    //

    try
    {
      // Send data
      socket_->send(coder_membuf_->data(), coder_membuf_->size());

      // Wait for the acknowledgement sent by the receiver.
      getACK();
    }
    catch(SocketException &)
    {
      // Error sending the end-of-file command.

      return 2;
    }

    // That's all.

    return 0;
  }
}

