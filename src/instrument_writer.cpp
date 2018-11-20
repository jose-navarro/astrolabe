#include "instrument_writer.hpp"

int
instrument_writer::
close
(void)
{
  {
    int status;

    // Ignore close request on non-opened channels.

    if (!data_channel_is_open_) return 0;

    // Close the channel according to its type.

    if (data_channel_is_socket_)
    {
      // Socket channels.

      if (socket_writer_->is_open())
      {
        status = socket_writer_->close();
        delete socket_writer_;
        socket_writer_ = NULL;

        if (status != 0) return status;
      }
    }
    else
    {
      //
      // Files. We must close and destroy the specialized
      // writer object.
      //

      if (file_writer_->is_open())
      {
        status = file_writer_->close();
        delete file_writer_;
        file_writer_ = NULL;

        if (status != 0) return status;
      }
    }

    // Update flags.

    data_channel_is_open_ = false;

    // That's all!

    return 0;
  }
}

instrument_writer::
instrument_writer
(void)
{
  {
    data_channel_is_open_   = false;
    data_channel_is_set_    = false;
    data_channel_is_socket_ = false;
    file_writer_            = NULL;
    header_file_name_       = "";
    host_                   = "";
    instrument_file_name_   = "";
    lineage_info_           = NULL;
    port_                   = 0;
    socket_server_mode_     = false;
    socket_writer_          = NULL;
  }
}


instrument_writer::
~instrument_writer
(void)
{
  {
    if (lineage_info_ != NULL) delete lineage_info_;
    if (data_channel_is_open_) close();
  }
}

bool
instrument_writer::  
is_open
(void)
const
{
  {
    return data_channel_is_open_;
  }
}

int
instrument_writer::  
open
(void)
{
  {
    string                       chunk_filename;
    string                       device_type;
    astrolabe_header_file_writer header_writer;
    int                          status;

    // Check our preconditions.

    if (data_channel_is_open_) return 1;

    if (!data_channel_is_set_) return 4;

    //
    // Decide what's the type of writer we need, depending on how
    // the channel has been set.
    //

    if (data_channel_is_socket_)
    {
      // Sockets.

      socket_writer_ = new instrument_socket_writer();
      status = socket_writer_->open(host_.c_str(), port_, socket_server_mode_);
      if (status != 0) return status;
    }
    else
    {
      //
      // Files (text only!)
      //

      // Write the header with metadata first.
      
      device_type = "obs-e_file";

      header_writer.set_data_channel (header_file_name_,
                                      instrument_file_name_,
                                      device_type,
                                      false, // Meaning "not binary", thus text.
                                      lineage_info_);

      status = header_writer.write();
      if (status != 0) return 2; // Unable to open the data channel.

      //
      // Get the first (and unique) chunk name. We ignore the return error
      // code since this is the first time we call the name generator and it
      // will always work (there's no risk of running out of file names).
      //
      // Note also that we generate a suffixed name for the actual file
      // containing the instruments just to adhere to the ASTROLABE standard
      // (that is, even those data files that need not to be split must
      // be named using a numeric suffix).
      //

      status = chunk_names_generator_.get_next_filename(chunk_filename);
      

      //
      // Instantitate a new instrument text file writer.
      //

      file_writer_ = new instrument_file_writer_txt();

      // Open the writer. Beware of errors!

      status = file_writer_->open(chunk_filename.c_str());

      if (status != 0) return status;
    }

    // Set flags.

    data_channel_is_open_ = true;

    // That's all.

    return 0;
  }
}

int
instrument_writer::  
set_data_channel 
(const char*   astrolabe_header_file_name,
 const char*   instrument_data_file_name,
 const lineage_data*
               lineage_info)
{
  {

    // Check our preconditions.

    // The data channel must be unset.

    if (data_channel_is_set_)               return 1;

    // We need a name for the header file.

    if (astrolabe_header_file_name == NULL) return 2;

    //
    // We need a name for the file that will actually store
    // instrument data.
    //

    if (instrument_data_file_name == NULL)    return 3;

    // Initialize own members.

    instrument_file_name_  = instrument_data_file_name;
    lineage_info_          = new lineage_data(*lineage_info);
    header_file_name_      = astrolabe_header_file_name;

    //
    // Prepare the chunk file name generator in write mode. We do not check
    // the status code because we're sure that all preconditions for
    // method set_mode_write() below are satisfied.
    //
    // Moreover, we'll use a 1-digit file name suffix because we'll generate
    // just a single chunk and need no more numbers to identify it.
    //

    chunk_names_generator_.set_mode_write(instrument_file_name_, 1);

    // Set status flags.

    data_channel_is_set_    = true;
    data_channel_is_socket_ = false;

    // That's all.

    return 0;
  }
}

int
instrument_writer::  
set_data_channel 
(const char*  host,
 int          port,
 bool         server_mode)
{
  {

    // Check our preconditions.

    if (data_channel_is_set_)             return 1;

    if ((!server_mode) && (host == NULL)) return 2;

    if ((port <= 0) || (port > 65535))    return 3;

    // Initialize own members.

    host_               = host;
    port_               = port;
    socket_server_mode_ = server_mode;

    // Set status flags.

    data_channel_is_set_    = true;
    data_channel_is_socket_ = true;

    // That's all.

    return 0;
  }
}

int
instrument_writer::
write_l
(bool    active,
 char*   identifier,
 int     instance_identifier,
 double  time,
 int     n_tags,
 double* buffer_tags,
 int     n_expectations,
 double* buffer_expectations,
 int     n_covariance_values,
 double* buffer_covariance)
{
  {
    int    status;

    // Check our preconditions.

    if (!data_channel_is_open_) return 1;

    //
    // See what's the type of data channel we're using and work
    // correspondingly.
    //

    if (data_channel_is_socket_)
    {
      // Sockets.

      status = socket_writer_->write_l(active, identifier, instance_identifier, time,
                                       n_tags, buffer_tags,
                                       n_expectations, buffer_expectations,
                                       n_covariance_values, buffer_covariance);
      return status;
    }
    else
    {
      // Text files.

      status = file_writer_->write_l(active, identifier, instance_identifier, time,
                                     n_tags, buffer_tags,
                                     n_expectations, buffer_expectations,
                                     n_covariance_values, buffer_covariance);
      return status;
    }
  }
}
