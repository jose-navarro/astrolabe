/** \file r_matrix_socket_writer.cpp 
  \brief Implementation file for r_matrix_socket_writer.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_socket_writer.hpp"

int
r_matrix_socket_writer::
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

r_matrix_socket_writer::
r_matrix_socket_writer
(void)
{
  {

    coder_membuf_        = NULL;
    coder_formatter_     = NULL;
    coder_stream_        = NULL;
    data_buffer_         = NULL;
    data_buffer_size_    = _R_MATRIX_SOCKET_WRITER_DEFAULT_BUFFER_SIZE;
    host_                = "";
    port_                = 0;
    server_socket_       = NULL;
    socket_              = NULL;
    socket_is_open_      = false;

  }
}

r_matrix_socket_writer::
~r_matrix_socket_writer
(void)
{
  {
    if (socket_is_open_) close();
  }
}

void
r_matrix_socket_writer::
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
r_matrix_socket_writer::
is_open
(void)
const
{
  {
    return socket_is_open_;
  }
}

int
r_matrix_socket_writer::
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
r_matrix_socket_writer::
write_r
(bool         active,
 double       time,
 int          n_correlation_values,
 double*      buffer_correlations)
{
  {
    // We need an open socket connection to work.

    if (!socket_is_open_)      return 1;

    // The dimensions of the several double arrays must be correct.

    if (n_correlation_values <= 0)  return 5;

    //
    // Write the r-record to the socket.
    //
    // Note that ALL information is coded using the XDR standard.
    //
    // The actual information (always coded) sent is the following:
    //
    // - Type - a single "r" character (denoting this one is an r-record).
    // - Active flag. A single character, either a '1' (active) or a
    //   '0' (removed)
    // - The time tag, as a double value.
    // - The number of correlation values, as an integer.
    // - The correlations array, as n_correlation_values double values.
    //

    try
    {

      // Reset the memory buffer in the XDR stream so we start from scratch.

      coder_membuf_->rewind();

      // The type.

      coder_stream_->cstring("r", 1);

      // The active flag.

      if (active) coder_stream_->cstring("1",1);
      else        coder_stream_->cstring("0",1);

      // The time tag.

      coder_stream_->simple(time);

      // The number of correlation matrix values.

      coder_stream_->simple(n_correlation_values);

      // The correlation matrix values themselves.

      coder_stream_->vector(buffer_correlations, n_correlation_values);

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

    // That's all.

    return 0;
  }
}
