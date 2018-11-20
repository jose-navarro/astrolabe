/** \file r_matrix_based_file_writer.cpp 
  \brief Implementation file for r_matrix_based_file_writer.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "r_matrix_based_file_writer.hpp"

int
r_matrix_based_file_writer::
_buffer_size
(void)
const
{
  {
    return data_buffer_size_ ;
  }
}

int
r_matrix_based_file_writer::
_default_buffer_size
(void)
const
{
  {
    return _ASTROLABE_R_MATRIX_FILE_WRITER_DEFAULT_BUFFER_SIZE ;
  }
}

char*
r_matrix_based_file_writer::
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

r_matrix_based_file_writer::
r_matrix_based_file_writer
(void)
{
  {
    data_buffer_         = NULL;
    data_buffer_cur_     = 0;
    data_buffer_size_    = _ASTROLABE_R_MATRIX_FILE_WRITER_DEFAULT_BUFFER_SIZE;
    file_file_           = NULL;
    file_name_           = NULL;
    file_is_open_        = false;
  }
}

r_matrix_based_file_writer::
~r_matrix_based_file_writer
(void)
{
  {
    if (data_buffer_ != NULL) delete [] data_buffer_;
    data_buffer_      = NULL;
    data_buffer_cur_  = 0;
    data_buffer_size_ = 0;
    file_file_        = NULL;
    if (file_name_ != NULL)   free(file_name_);
    file_is_open_     = false;
  }
}

int
r_matrix_based_file_writer::
flush
(void)
{
  {
    size_t bytes_written;

    // If there's no data to dump, simply return (no errors).

    if (data_buffer_cur_ == 0) return 0;

    // Write the current contents of the data buffer to disk.

    bytes_written = fwrite((void*)data_buffer_, 1, (size_t)data_buffer_cur_, file_file_);
    if (bytes_written != (size_t)data_buffer_cur_) return 1;

    fflush(file_file_);

    // Reset pointers and flags.

    data_buffer_cur_ = 0;

    // That's all.

    return 0;
  }
}

bool
r_matrix_based_file_writer::
is_open
(void)
const
{
  {
    return file_is_open_;
  }
}

int
r_matrix_based_file_writer::
open
(const char*  file_name)
{
  {
    // Won't accept open requests if the file is already open.

    if (file_is_open_) return 1;

    // Try to open the file.

    file_file_ = fopen(file_name, "wb");

    // If the open command failed, report that and go away.

    if (file_file_ == NULL) return 2;

    // Try to create the buffer used for block reads.

    try
    {
      //
      // Instantiate a data buffer with the proper size.
      //

      data_buffer_ = new char[data_buffer_size_];
      file_is_open_ = true;
    }
    catch (...)
    {
      //
      // If there's not enough memory, close again the file, flag it
      // as not open and return the error code.
      //

      close();
      file_is_open_ = false;
      return 3;
    }

    // Set the name of the file just open.

    file_name_ = strdup(file_name);

    // Successful completion.

    return 0;
  }
}

int
r_matrix_based_file_writer::
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

