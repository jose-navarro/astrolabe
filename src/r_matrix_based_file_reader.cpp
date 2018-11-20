/** \file r_matrix_based_file_reader.cpp 
  \brief Implementation file for r_matrix_based_file_reader.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_based_file_reader.hpp"

r_matrix_based_file_reader::
r_matrix_based_file_reader
(void)
{
  {
    data_buffer_                           = NULL;
    data_buffer_size_                      = _ASTROLABE_R_MATRIX_FILE_READER_DEFAULT_BUFFER_SIZE ;
    file_file_                             = NULL;
    file_name_                             = NULL;
    file_is_open_                          = false;
    file_size_                             = 0;

    last_record_is_active_                 = false;

    is_eof_                                = false;

    read_active_flag_                      = false;
    read_completed_                        = true;
    read_r_data_                           = false;
    read_time_                             = false;
    read_type_                             = false;
  }
}

r_matrix_based_file_reader::
~r_matrix_based_file_reader
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
r_matrix_based_file_reader::
_buffer_size
(void)
const
{
  {
    return data_buffer_size_ ;
  }
}

char*
r_matrix_based_file_reader::
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
r_matrix_based_file_reader::
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
r_matrix_based_file_reader::
_default_buffer_size
(void)
const
{
  {
    return _ASTROLABE_R_MATRIX_FILE_READER_DEFAULT_BUFFER_SIZE ;
  }
}

bool
r_matrix_based_file_reader::
is_eof
(void)
const
{
  {
    return is_eof_;
  }
}

bool
r_matrix_based_file_reader::
is_open
(void)
const
{
  {
    return file_is_open_;
  }
}

int
r_matrix_based_file_reader::
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
