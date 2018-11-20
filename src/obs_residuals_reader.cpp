/** \file obs_residuals_reader.cpp
  \brief  Implementation file for obs_residuals_reader.hpp.
  \ingroup ASTROLABE_data_IO
*/

#include "obs_residuals_reader.hpp"

int
obs_residuals_reader::
close
(void)
{
  {
    int status;

    status = 0;

    // If we were already closed, we'll simply ignore the request.

    if (!data_channel_is_open_) return status;

    if (file_reader_ != NULL)
    {
      status = file_reader_->close();
      delete file_reader_;
      file_reader_ = NULL;
    }

    data_channel_is_open_ = false;
    return status;
  }
}

bool
obs_residuals_reader::
epoch_changed
(void)
{
  {
    return last_epoch_changed_;
  }
}

void
obs_residuals_reader::
epoch_change_acknowledged
(void)
{
  {
    last_epoch_changed_ = false;
  }
}

obs_residuals_reader::
obs_residuals_reader
(void)
{
  {
    data_channel_is_open_        = false;
    data_channel_is_set_         = false;
    file_reader_                 = NULL;
    is_eof_                      = false;
    is_header_filename_set_      = false;
    is_metadata_read_            = false;
    is_schema_filename_set_      = false;

    last_epoch_changed_          = false;
    last_epoch_time_available_   = false;
    last_epoch_time_             = 0;
    last_record_is_active_       = false;

    header_file_                 = "";
    schema_file_                 = "";

    reading_first_epoch_         = true;
  }
}


obs_residuals_reader::
~obs_residuals_reader
(void)
{
  {
    // Close the reader if still open.

    if (data_channel_is_open_) close();
  }
}

int
obs_residuals_reader::
get_metadata
(int& total_metadata_warnings,
 int& total_metadata_errors)
{
  {

    vector<string> error_description;  // To build full error messages.
    gp_utils gp;                       // Utilitary class. Used to format error messages
                                       // in our case.
    int retcode;                       // Return code.
    string error_message;              // A full error message.
    string error_tag;                  // Tag in the input header file where an error has
                                       // been found.

    // Don't try to retrieve metadata if it's already been retrieved or
    // we've already opened (open()) the underlying data channel.

    if ((is_metadata_read_) | (data_channel_is_open_)) return 1;

    //
    // We need at least the name of the astrolabe header file. If
    // not set, we must report an error.
    //

    if (!is_header_filename_set_) return 2;

    // It looks like everything is correct. Set the total number of issues to 0.

    total_metadata_warnings = 0;
    total_metadata_errors   = 0;

    //
    // We have to parse the file containing the astrolabe header with
    // the description of the data channel (socket, file...).
    //
    // We'll may use the parser in two different ways; this depends on having
    // or not a schema file.
    //

    if (is_schema_filename_set_)
    {
      // We've got an schema to check the input file.
      header_parser_.set_parser_parameters(header_file_, schema_file_, &header_data_);
    }
    else
    {
      // No schema available. We'll work in "unsafe" mode.
      header_parser_.set_parser_parameters_developer(header_file_, &header_data_);
    }

    // We are ready to parse...

    header_parser_.parse();

    //
    // Check if warnings of errors have been detected and set the return
    // code accordingly.
    //

    total_metadata_warnings = header_parser_.warnings_dim();
    total_metadata_errors   = header_parser_.errors_dim();

    retcode = 0;
    if ((total_metadata_warnings != 0) | (total_metadata_errors != 0)) retcode = 3;

    //
    // We indicate that the metadata has been retrieved, no matter the what
    // the return code is. This is to avoid repeated calls to this method.
    //

    is_metadata_read_ = true;

    // That's all.

    return retcode;
  }
}

int
obs_residuals_reader::
get_metadata_errors
(list<string>& list_of_errors)
{
  {
    int    i;
    string message;
    int    total_items;

    list_of_errors.clear();

    total_items = header_parser_.errors_dim();

    for (i = 0; i < total_items; i++)
    {
      message = header_parser_.errors_get(i);
      list_of_errors.push_back(message);
    }

    return total_items;
  }
}

int
obs_residuals_reader::
get_metadata_warnings
(list<string>& list_of_warnings)
{
  {
    int    i;
    string message;
    int    total_items;

    list_of_warnings.clear();

    total_items = header_parser_.warnings_dim();

    for (i = 0; i < total_items; i++)
    {
      message = header_parser_.warnings_get(i);
      list_of_warnings.push_back(message);}

    return total_items;
  }
}

bool
obs_residuals_reader::
is_eof
(void)
const
{
  {
    return is_eof_;
  }
}

bool
obs_residuals_reader::
is_open
(void)
const
{
  {
    return data_channel_is_open_;
  }
}

int
obs_residuals_reader::
open
(void)
{
  {

    string       data_channel_filename;
    string       chunk_filename;
    bool         is_binary_file;
    int          status;

    // If the data channel is already open, report an error.

    if (data_channel_is_open_) return 1;

    //
    // Now, check what kind of underlying data channel we are
    // going to deal with.
    //
    // We're dealing only with files. We need to know if we want to read
    // a text or a binary file.
    //

    is_binary_file = 
      (header_data_.get_device_format() == gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE);

    //
    // Retrieve too the name of the "general" external file that will keep (in fact,
    // possibly split into chunks) the actual data.
    //

    data_channel_filename = header_data_.get_device_file_name();

    //
    // Set up the segmented_file_utils object that will take care of generating
    // the names of the chunk files making the "general" underlying data file.
    //

    chunk_names_generator_.set_mode_read(data_channel_filename, false);

    //
    // Get the name of the first chunk making the general file.
    // Note that if the next call fails, it will mean that the
    // actual chunk files are not located in the same folder
    // than the astrolabe header file or that these chunks
    // do not exist or that are named incorrectly.
    //

    status = chunk_names_generator_.get_next_filename(chunk_filename);
    if (status != 0) return 2; // Unable to open data channel.

    //
    // Instantiate the appropriate kind of reader depending on the
    // file type (text or binary).
    //

    if (is_binary_file)
    {
      // Forward, binary file reader.
      file_reader_ = new obs_residuals_file_reader_bin_forward();
    }
    else
    {
      // Forward, text file reader.
      file_reader_ = new obs_residuals_file_reader_txt_forward();
    }

    //
    // We may now try to open the actual underlying file using
    // the reader (whatever it is) just created. This should never
    // fail, since our chunk filenames generator already has checked
    // that these chunks do exist. However, we'll check...
    //

    status = file_reader_->open(chunk_filename.c_str());
    if (status != 0) return status;

    // We've opened the underlying data channel!!!

    data_channel_is_open_ = true;

    // That's all!

    return 0;

  }
}

int
obs_residuals_reader::
read_active_flag
(bool& active)
{
  {
    int status;

    status = file_reader_->read_active_flag(active);

    if (status == 0) last_record_is_active_ = active;

    return status;
  }
}

int
obs_residuals_reader::
read_identifier
(char*& identifier,
 int    id_len)
{
  {
    return file_reader_->read_identifier(identifier, id_len);
  }
}

int
obs_residuals_reader::
read_l_data
(int      n_tag_values,
 double*& the_tags,
 int      n_expectation_values,
 double*& the_expectations,
 int&     n_covariance_values_found,
 double*& the_covariance_values)
{
  {
    return file_reader_->read_l_data(n_tag_values,              the_tags,
                                     n_expectation_values,      the_expectations,
                                     n_covariance_values_found, the_covariance_values);
  }
}

int
obs_residuals_reader::
read_instance_id
(int& instance_identifier)
{
  {
    return file_reader_->read_instance_id(instance_identifier);
  }
}

int
obs_residuals_reader::
read_time
(double& time)
{
  {
    int status;

    status = file_reader_->read_time(time);

    // Check how things went.
    
    if (status != 0) return status;

    //
    // Update last record read information, last time /
    // epoch related flags and values.
    //
    // VERY IMPORTANT: this class takes care by itself of
    // the epoch change flag instead of using the
    // method epoch_changed() available in all kinds of
    // readers.
    //
    // This is so because we may read data that is split into
    // several independent files (chunks) and use lower
    // level readers to read them. These readers consider
    // that the first epoch in the file does not raise
    // an epoch change condition. While this is valid
    // when reading a single file, this is a problem
    // when a "file" is made, in fact, of a concatination
    // of several "chunks", since each time we open a
    // new chunk we'd get a false result when asking
    // for an epoch change.
    //
    // Note that flags are NOT changed when dealing
    // with INACTIVE records. These are read, but do NOT
    // change the status of the reader concerning epochs.
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
obs_residuals_reader::  
read_type
(char& record_type)
{
  {
    string chunk_filename;
    bool   is_binary_file;
    int    status;

    //
    // Use the file generic file reader's read_type method to actually
    // read the type.
    //

    status = file_reader_->read_type(record_type);

    //
    // Note that the following test is inside a while loop; this strange
    // way of testing tries to cover the situation where a series of chunk
    // files are empty, so opening the next chunk will raise again
    // an end of file condition.
    //
    // The look is broken either when a chunk is opened and its first type
    // value read correctly or when NO more chunks are available.
    //

    while (status == 1)
    {
      //
      // This is an end-of-file condition. Since we're working with split
      // files (chunks) we must try to open the next one instead of returning
      // directly this flag. Only when no more files are available we'll
      // return and "end-of-file" code.
      //

      //
      // First, close the current reader. This could return an error code,
      // that we'll rewrite as "error closing / opening current / next chunk".
      //

      status = file_reader_->close();
        
      delete file_reader_;
      file_reader_ = NULL;

      if (status != 0) return 7;

      //
      // Generate the name for the next chunk.
      //
      // If we've got an error here, it means that there are no more files
      // to read, so we'll return the end-of-file condition.
      //

      status = chunk_names_generator_.get_next_filename(chunk_filename);
      if (status != 0)
      {
        is_eof_ = true;
        return 1; // End of file (no more chunks are available).
      }

      //
      // Instantiate the appropriate kind of reader depending on the
      // file type (text or binary)
      //

      is_binary_file
        = (header_data_.get_device_format() == gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE);

      if (is_binary_file)
      {
        // Forward, binary file reader.
        file_reader_ = new obs_residuals_file_reader_bin_forward();
      }
      else
      {
        // Forward, text file reader.
        file_reader_ = new obs_residuals_file_reader_txt_forward();
      }

      //
      // Open the new chunk. If this fails, we'll return an
      // "error closing / opening current / next chunk".
      //

      status = file_reader_->open(chunk_filename.c_str());
      if (status != 0) return 7;

      //
      // Now we must read the type from the new chunk!
      //

      status = file_reader_->read_type(record_type);
    }

    //
    // At this point we may have a correct type read or an error code that
    // will never be an end-of-file condition. Whatever it is, we may
    // safely return.
    //
    // If the type was correctly read, the status will be 0; if we've had
    // a problem, it will not be an end of file, but some other kind of
    // problem that we must report.
    //

    return status;





  }
}

int
obs_residuals_reader::
set_header_file
(const char*  header_file_name)
{
  {
    int status;
    int total_errors;
    int total_warnings;

    if (header_file_name == NULL)
    {
      // We need a filename.
      return 1;
    }
    if ((is_header_filename_set_) | (data_channel_is_open_))
    {
      // It is not possible to set the filename once it's been opened.
      return 2;
    }

    // Copy the input file name and set control flags.

    header_file_            = header_file_name;
    is_header_filename_set_ = true;

    // Let's load the metadata!!!

    status = get_metadata(total_warnings, total_errors);

    //
    // If errors or warnings have been detected, report the issue. The
    // caller module may retrieve the amount and list of warnings or
    // errors using get_metadata_warnings() and get_metadata_errors()
    // respectively.
    //

    if (status != 0) return 3;

    // That's all.

    return 0;

  }
}

int
obs_residuals_reader::
set_schema_file
(const char*  schema_file_name)
{
  {
    if (schema_file_name == NULL)
    {
      // We need a filename.
      return 1;
    }
    if (is_header_filename_set_)
    {
      //
      // It is not possible to set the name of the XML schema if the
      // name of the astrolabe header file has already been set.
      //

      return 2;
    }

    // Copy the input file name and set control flags.

    schema_file_            = schema_file_name;
    is_schema_filename_set_ = true;

    return 0;

  }
}
