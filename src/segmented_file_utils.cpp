/** \file segmented_file_utils.cpp 
  \brief Implementation file for segmented_file_utils.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 

#include "segmented_file_utils.hpp"


string
segmented_file_utils::
build_chunk_name
(const string& filename,
 int number,
 int width)
{

  string chunk_filename;

  {
    string extension;
    string numeric_suffix;
    string path_and_basename;

    // Get the path + base name and extension.

    filename_split(filename, path_and_basename, extension);

    //
    // Build the zero-padded suffix for the chunk number according to
    // the width specified.
    //

    numeric_suffix = zero_pad_number(number, width);

    // Build the name of the file to test.

    chunk_filename = path_and_basename + "_" + numeric_suffix;
    if (extension != "") chunk_filename = chunk_filename + "." + extension;

  }

  return chunk_filename;
}

bool
segmented_file_utils::
file_exists
(const string& filename)
{

  bool result;

  {
    result =  (access(filename.c_str(), __CHECK_FILE_EXISTENCE__) != -1 );
  }

  return result;
}

void
segmented_file_utils::
filename_split
(const string& filename,
 string&       path_and_basename,
 string&       extension)
{
  {
    size_t found;

    found = filename.find_last_of(".");

    if (found ==  string::npos)
    {
      path_and_basename = filename;
      extension         = "";
    }
    else
    {
      path_and_basename = filename.substr(0,found);
      if (found == (filename.size() - 1))
      {
        extension = "";
      }
      else
      {
        extension = filename.substr(found+1);
      }
    }
  }
}

int
segmented_file_utils::
find_all_chuncks
(const string& filename,
 int&  number_of_chunks,
 int&  suffix_width)
{
  {

    int    file_number;
    bool   first_chunk_found;
    string test_filename;
    int    width;

    // Test for suffixes whose widths range from 1 to 9 chars wide.

    first_chunk_found = false;

    for (width = 1; width <= 9; width++)
    {
      //
      // Build the name of the first file to test. Its chunk number is 1
      // and the current test width is fixed by variable width.
      //

      test_filename = build_chunk_name(filename, 1, width);

      // Does this file exist?

      if (file_exists(test_filename))
      {
        //
        // Yes! We've found the first file chunk and now know how wide is
        // the numeric suffix!
        //

        first_chunk_found = true;
        suffix_width = width;
        break;
      }
    }

    // Check if we really have the name of the first file chunk.

    if (!first_chunk_found)
    {
      // Not found! This is an error...
      return 1;
    }

    //
    // Now iterate to find how many chunks do we have. Since we've
    // already found chunk number 1, we'll start looking for chunks
    // number 2 and up.
    //

    file_number = 2;

    while (true)
    {

      //
      // Build the name of chunk number file_number, using the
      // suffix width we've found above.
      //

      test_filename = build_chunk_name(filename, file_number, suffix_width);

      // Does this file exist?

      if (!file_exists(test_filename))
      {
        //
        // It does not exist. We've reached the last chunk.
        //
        // This means that we have file_number - 1 chunks available.
        //

        number_of_chunks = file_number - 1;

        // We are over!
        
        break;
      }

      // Go for the next chunk.

      file_number++;
    }

    return 0;
  }
}

int
segmented_file_utils::
get_next_filename
(string& next_filename)
{
  {
    // An operation mode must have been set.

    if (mode_work_ == __SFU_WORK_MODE_UNSET__) return 2;

    if (mode_work_ == __SFU_WORK_MODE_READ__)
    {
      //
      // Read mode. The next file name depends on whether we're working
      // on reverse mode or not.
      //

      if (backwards_reader_)
      {
        //
        // Backwards reader.
        //
        // Decrease the current chunk number. Reaching 0 means that
        // no more chunks exist.
        //

        chunks_current_--;
        if (chunks_current_ == 0) return 1;
      }
      else
      {
        //
        // Forward reader.
        //
        // Increase the current chunk number. Surpassing the value of
        // chunks total means that no more chunks exist.
        //

        chunks_current_++;
        if (chunks_current_ > chunks_total_) return 1;
      }
    }
    else
    {
      //
      // Write mode. we always move forward. Be careful: we may
      // exceed the maximum number of numeric suffixes available
      // according to the suffix width! For instance, if this
      // width is 2, the last available chunk number would be 99.
      // Going beyond this limit means that we can't generate
      // more chunk filenames and an error must be returned!
      //

      chunks_current_++;

      if (chunks_current_ > max_chunks_) return 1;
    }

    // Generate the name of the current chunk.

    next_filename = build_chunk_name(filename_, chunks_current_, numeric_suffix_width_);

    // That's all.

    return 0;
  }
}

segmented_file_utils::
segmented_file_utils
(void)
{
  {
    backwards_reader_ = false;
    chunks_total_         = 0;
    chunks_current_       = 0;
    filename_             = "";
    max_chunks_           = 0;
    mode_work_            = __SFU_WORK_MODE_UNSET__ ;
    mode_read_            = __SFU_READ_MODE_UNSET__;
    numeric_suffix_width_ = 0;
  }
}


segmented_file_utils::
~segmented_file_utils
(void)
{
  {
    // Intentionally left blank.
  }
}

int
segmented_file_utils::
set_mode_read
(const string& filename,
 bool  backwards_reader)
{
  {
    int number_of_chunks;
    int status;
    int suffix_width;

    // The operations mode may be set only once.

    if (mode_work_ != __SFU_WORK_MODE_UNSET__) return 1;

    // The name of the file may not be empty.

    if (filename == "") return 2;

    // Set our flags and attributes.

    mode_work_        = __SFU_WORK_MODE_READ__ ;
    backwards_reader_ = backwards_reader;
    filename_         = filename;

    // Try to find the chunks making our generic file.

    status = find_all_chuncks(filename, number_of_chunks, suffix_width);
    if (status != 0) return 3;

    // Chunks identified. Set more attributes.

    chunks_total_         = number_of_chunks;
    numeric_suffix_width_ = suffix_width;

    if (backwards_reader_)
    {
      chunks_current_ = chunks_total_ + 1;
    }
    else
    {
      chunks_current_ = 0;
    }

    // That's all.

    return 0;
  }
}

int
segmented_file_utils::
set_mode_write
(const string& filename,
 int   numeric_suffix_width)
{
  {

    // The operations mode may be set only once.

    if (mode_work_ != __SFU_WORK_MODE_UNSET__) return 1;

    // The name of the file may not be empty.

    if (filename == "") return 2;

    // The width of the suffix must be a number between 1 and 9.

    if ((numeric_suffix_width < 0) || (numeric_suffix_width > 9)) return 3;

    // Set our flags and attributes.

    mode_work_            = __SFU_WORK_MODE_WRITE__ ;
    numeric_suffix_width_ = numeric_suffix_width;
    filename_             = filename;
    max_chunks_           = (int)pow(10.0, numeric_suffix_width_) - 1;

    // That's all.

    return 0;
  }
}

string
segmented_file_utils::
zero_pad_number
(int number,
 int width)
{
  {
    ostringstream ss;
    ss << setw(width) << setfill( '0' ) << number;
    return ss.str();
  }
}
