/** \file r_matrix_socket_reader.cpp 
  \brief Implementation file for r_matrix_socket_reader.hpp
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_socket_reader.hpp"

int
r_matrix_socket_reader::
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

    data_buffer_       = NULL;
    decoder_membuf_    = NULL;
    decoder_formatter_ = NULL;
    decoder_stream_    = NULL;

    // Update flags.

    socket_is_open_ = false;

    // That's all.

    return 0;
  }
}

r_matrix_socket_reader::
r_matrix_socket_reader
(void)
{
  {
    data_buffer_                = NULL;
    data_buffer_size_           = _R_MATRIX_SOCKET_READER_DEFAULT_BUFFER_SIZE;

    decoder_membuf_             = NULL;
    decoder_formatter_          = NULL;
    decoder_stream_             = NULL;

    last_record_is_active_      = false;

    is_eof_                     = false;

    read_active_flag_           = false;
    read_completed_             = true;
    read_r_data_                = false;
    read_time_                  = false;
    read_type_                  = false;

    server_socket_              = NULL;
    socket_                     = NULL;
    socket_is_open_             = false;
  }
}

r_matrix_socket_reader::
~r_matrix_socket_reader
(void)
{
  {
    if (socket_is_open_) close();
  }
}

void
r_matrix_socket_reader::
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
r_matrix_socket_reader::
is_eof
(void)
const
{
  {
    return is_eof_;
  }
}

int
r_matrix_socket_reader::
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
      data_buffer_        = new char[data_buffer_size_];

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
r_matrix_socket_reader::
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
r_matrix_socket_reader::
read_r_data
(int&     n_correlation_values_found,
 double*& the_correlation_values)
{
  {
    int   n_of_elements;

    // Check that the socket connection is open!

    if (!socket_is_open_)          return 2;

    //
    // Check preconditions. This function works only when the time
    // tag has been read and we are reading an l-type record.
    //

    if (!read_time_)               return 3;
    if ( read_completed_)          return 3;

    //
    // Get the number of correlation values to read and
    // the values themselves.
    //

    try
    {

      //
      // Number of values.
      //

      decoder_stream_->simple(n_of_elements);
      n_correlation_values_found = n_of_elements;

      // Get the correlation values themselves.

      decoder_stream_->vector(the_correlation_values, n_correlation_values_found);

    }
    catch (...)
    {
      //
      // Error decoding either the number of elements in the
      // array or the array itself. Report it as a read error condition.
      //

      return 2;
    }

    //
    // Update flags.
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
r_matrix_socket_reader::
read_time
(double& time)
{
  {
    // Check that the socket connection is open!

    if (!socket_is_open_) return 2;

    //
    // Check preconditions. This function works only when the
    // active flag has been read and no other read_* operation
    // has taken place.
    //

    if (!read_active_flag_) return 3;
    if (read_time_)         return 3;

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

    // That's all.

    return 0;
  }
}

int
r_matrix_socket_reader::
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
    // with the record type (l, o, r, or end-of-file command) this method
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

    if (record_type != 'r') return 4;

    // Update flags.

    read_completed_        = false;
    read_type_             = true;

    // That's all.

    return 0;
  }
}
