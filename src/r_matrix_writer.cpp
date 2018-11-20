#include "r_matrix_writer.hpp"

int
r_matrix_writer::
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

r_matrix_writer::
r_matrix_writer
(void)
{
  {
    already_written_records_ = 0;
    data_channel_is_open_    = false;
    data_channel_is_set_     = false;
    data_channel_is_socket_  = false;
    external_base_file_      = "";
    file_mode_is_binary_     = false;
    file_writer_             = NULL;
    header_file_name_        = "";
    host_                    = "";
    lineage_info_            = NULL;
    max_records_per_chunk_   = 0;
    numeric_suffix_width_    = 0;
    port_                    = 0;
    socket_server_mode_      = false;
    socket_writer_           = NULL;
  }
}


r_matrix_writer::
~r_matrix_writer
(void)
{
  {
    if (lineage_info_ != NULL) delete lineage_info_;
    if (data_channel_is_open_) close();
  }
}

bool
r_matrix_writer::  
is_open
(void)
const
{
  {
    return data_channel_is_open_;
  }
}

int
r_matrix_writer::  
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

      socket_writer_ = new r_matrix_socket_writer();
      status = socket_writer_->open(host_.c_str(), port_, socket_server_mode_);
      if (status != 0) return status;
    }
    else
    {
    //
    //Files
    //
    // In all cases we must write the astrolabe header file
    // including the metadata defining the data channel.
    //

    // Write the header with metadata.

      device_type = "r-matrix_file";

      header_writer.set_data_channel (header_file_name_,
                                      external_base_file_,
                                      device_type,
                                      file_mode_is_binary_,
                                      lineage_info_);

      status = header_writer.write();
      if (status != 0) return 2; // Unable to open the data channel.


      //
      // Get the first chunk name. We ignore the return error code since
      // this is the first time we call the name generator and it will
      // always work (there's no risk of running out of file names).
      //

      status = chunk_names_generator_.get_next_filename(chunk_filename);

      // Instantiate the proper writer thanks to polymorphism.

      if (file_mode_is_binary_)
      {
        // Binary files.
        file_writer_ = new r_matrix_file_writer_bin();
      }
      else
      {
        // Text files.
        file_writer_ = new r_matrix_file_writer_txt();
      }

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
r_matrix_writer::  
set_data_channel 
(const char*   astrolabe_header_file_name,
 const char*   external_base_file_name,
 bool          binary,
 int           max_records_to_split,
 int           file_suffix_width,
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
    // We need a base name for the different chunks (files) that will
    // be used to write data.
    //

    if (external_base_file_name == NULL)    return 3;


    //
    // Are the maximum number of records to split or the file suffix width
    // within the allowed range?
    //


    if (max_records_to_split <= 0)          return 4;

    if ((file_suffix_width <= 0) ||
        (file_suffix_width >  9))           return 5;


    // Initialize own members.

    external_base_file_     = external_base_file_name;
    file_mode_is_binary_    = binary;
    max_records_per_chunk_  = max_records_to_split;
    lineage_info_           = new lineage_data(*lineage_info);
    numeric_suffix_width_   = file_suffix_width;
    header_file_name_       = astrolabe_header_file_name;


    //
    // Prepare the chunk file name generator in write mode. We do not check
    // the status code because we're sure that all preconditions for
    // method set_mode_write() below are satisfied.
    //

    chunk_names_generator_.set_mode_write(external_base_file_, numeric_suffix_width_);

    // Set status flags.

    data_channel_is_set_    = true;
    data_channel_is_socket_ = false;

    // That's all.

    return 0;
  }
}

int
r_matrix_writer::
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

    host_ = host;
    port_ = port;
    socket_server_mode_ = server_mode;

    // Set status flags.

    data_channel_is_set_ = true;
    data_channel_is_socket_ = true;

    // That's all.

    return 0;
  }
}

int
r_matrix_writer::
write_r
(bool         active,
 double       time,
 int          n_correlation_values,
 double*      the_correlation_values)
{
  {
    string chunk_filename;
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

      status = socket_writer_->write_r(active, time,
                                       n_correlation_values, the_correlation_values);
      return status;
    }
    else
    {
    //
    // Files, either binary or text.
    //
    // We'll use the polymorphic file writer to write the r-record.
    //
    // However, we must check if we've reached the maximum number of
    // records before writing, so we may split the output into different
    // file chunks.
    //

      if (already_written_records_ == max_records_per_chunk_)
      {

        // We've reached the maximum number of epochs per chunk.

        // Close the current chunk.

        status = file_writer_->close();
        if (status != 0) return 2; // Error writing to disk.

        // Destroy the file writer.

        delete file_writer_;
        file_writer_ = NULL;

        //
        // Get the next chunk file name. Note that this operation may fail
        // if the width of the numeric suffix used to christianize new
        // files is too short, and the number of available digits is not
        // enough to assign a number to the new chunk. For instance, if
        // such width is just 1, no more than nine files (with suffixes
        // going from _1 to _9) may be generated.
        //

        status = chunk_names_generator_.get_next_filename(chunk_filename);
        if (status != 0) return 4;

        //
        // Create a new file writer, depending on the kind of file we're
        // dealing with.
        //

        if (file_mode_is_binary_)
        {
          file_writer_ = new r_matrix_file_writer_bin();
        }
        else
        {
          file_writer_ = new r_matrix_file_writer_txt();
        }

        // Open the writer.

        status = file_writer_->open(chunk_filename.c_str());
        if (status != 0) return 5;


        // Reset our counter of written records.

        already_written_records_ = 0;
      }

      // One more record written!

      already_written_records_++;

      // Finally, write the r-record! Use the polymorphic writer to do it.

      status = file_writer_->write_r(active, time,
                                     n_correlation_values, the_correlation_values);
      return status;
    }
  }
}
