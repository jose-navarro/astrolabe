/** \file obs_e_based_file_reader.cpp 
  \brief Implementation file for obs_e_based_file_reader.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "obs_e_based_file_reader.hpp"

obs_e_based_file_reader::
obs_e_based_file_reader
(void)
{
  {
    data_buffer_                           = NULL;
    data_buffer_size_                      = _ASTROLABE_OBS_E_FILE_READER_DEFAULT_BUFFER_SIZE ;
    file_file_                             = NULL;
    file_name_                             = NULL;
    file_is_open_                          = false;
    file_size_                             = 0;

    last_epoch_changed_                    = false;
    last_epoch_time_available_             = false;
    last_epoch_time_                       = 0;

    first_l_read_                          = false;

    last_record_is_active_                 = false;

    is_eof_                                = false;

    read_active_flag_                      = false;
    read_identifier_                       = false;
    read_completed_                        = true;
    read_iid_                              = false;
    read_l_data_                           = false;
    read_o_data_                           = false;
    read_time_                             = false;
    read_type_                             = false;

    reading_first_epoch_                   = true;
    reading_l_                             = false;
    reading_o_                             = false;
  }
}

obs_e_based_file_reader::
~obs_e_based_file_reader
(void)
{
  {
    if (data_buffer_ != NULL) delete [] data_buffer_;

    data_buffer_        = NULL;
    data_buffer_size_   = 0;

    if (file_is_open_)
    {
      fclose(file_file_);
      file_is_open_ = false;
    }

    file_file_          = NULL;

    if (file_name_ != NULL)
    {
      delete file_name_;
      file_name_ = NULL;
    }
  }
}

int
obs_e_based_file_reader::
_buffer_size
(void)
const
{
  {
    return data_buffer_size_ ;
  }
}

char*
obs_e_based_file_reader::
_file_name 
(void)
const
{
  char* result;

  {
    result = NULL;

    if (file_is_open_) result = strdup(file_name_);
  }
  return result;
}

int
obs_e_based_file_reader::
close
(void)
{
  {
    int status;

    // Do not complain if the file is already closed.
    if (!file_is_open_) return 0;

    // Close the file.

    status = fclose(file_file_);

    // Problems? Report these using the return code.

    if (status != 0) return 1;

    //
    // The file is closed now. Update the corresponding status members and
    // destroy buffers.
    //

    file_is_open_ = false;

    delete [] data_buffer_;
    data_buffer_ = NULL;

    // Successful completion.

    return 0;
  }
}

int
obs_e_based_file_reader::
_default_buffer_size
(void)
const
{
  {
    return _ASTROLABE_OBS_E_FILE_READER_DEFAULT_BUFFER_SIZE ;
  }
}

bool
obs_e_based_file_reader::
epoch_changed
(void)
{
  {
    return last_epoch_changed_;
  }
}

void
obs_e_based_file_reader::  
epoch_change_acknowledged
(void)
{
  {
    last_epoch_changed_ = false;
  }
}

bool
obs_e_based_file_reader::
is_eof
(void)
const
{
  {
    return is_eof_;
  }
}

bool
obs_e_based_file_reader::
is_open
(void)
const
{
  {
    return file_is_open_;
  }
}

int
obs_e_based_file_reader::
set_buffer_size
(int buffer_size)
{
  {
    // If the file is already open, it's not legal to change the buffe size.
    if (file_is_open_)    return 1;

    // The buffer size must be a positive value.
    if (buffer_size <= 0) return 2;

    // Set the new buffer size.

    data_buffer_size_ = buffer_size;

    // Bye.

    return 0;
  }
}
