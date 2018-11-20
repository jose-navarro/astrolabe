/** \file segmented_file_utils.hpp
  \brief File name generator for files that are actually stored in different numbered fragments or chunks.
  \ingroup ASTROLABE_data_IO
*/

#ifndef _segmented_file_utils_HPP_
#define _segmented_file_utils_HPP_

/*
   This class need either MS _access or gcc access functions.
   These need different header files to work, and even have
   different names in spite they do the same thing.

   To solve this problem and unify the way this function
   is used, several defines are made below depending on which
   compiler is used.
*/

#ifdef _WIN32

  #include  <io.h>
  #include  <stdio.h>
  #include  <stdlib.h>

  #define access _access

  #define __CHECK_FILE_EXISTENCE__ 00

#endif // _WIN32

#ifdef __GNUC__

  #include <unistd.h>

  #define __CHECK_FILE_EXISTENCE__ F_OK

#endif // __GNUC__

#include <iomanip>
#include <sstream>
#include <string>

#include <iostream>

#include <math.h>

using namespace std;

/// \brief Work mode not set yet.

#define __SFU_WORK_MODE_UNSET__     0

/// \brief Work mode set to read.

#define __SFU_WORK_MODE_READ__      1

/// \brief Work mode set to write.

#define __SFU_WORK_MODE_WRITE__     2

/// \brief Read direction not set yet.

#define __SFU_READ_MODE_UNSET__     0

/// \brief Read direction set to forward.

#define __SFU_READ_MODE_FORWARD__   1

/// \brief Read direction set to backwards.

#define __SFU_READ_MODE_BACKWARDS__ 2

/// \brief File name generator for files that are actually stored in
/// different numbered fragments or chunks.

/**
  \ingroup ASTROLABE_data_IO

  This class implement the ability to generate names for files that
  are actually stored as a set of sequentially numbered fragments or
  chunks.

  This allows applications to derive names automatically starting
  from a generic file name, not having to worry about how these
  names are created.

  To do this, the following must be taken into account:

  - A naming convention, deriving chunk file names from the general
    name must be respected.
  - Names may be generated for applications that intend to read
    existing files or for others that want to create new files.
  - Applications willing to read these split files may want to
    do so in either ascending or descending order.

  The naming convention used to derive chunk file names from general
  names is the following:

  \<general_file_name_base_name\>_\<numeric_suffix\>.\<general_file_name_extension\>

  where:

  - \<general_file_name_base_name\> stands for the base name of the general file name,
    that is, the original file name without the extension.
  - \<numeric_suffix\> is a sequential number, between 1 and 9 digits wide, padded
    with zeros on the left side to reach the desired width.
  - \<general_file_name_extension\> is the extension (excluding the dot) found
    in the original file name.

  The extension part of the former naming convention is optional. If the general
  file name includes no extension, then it will not be added to the chunk file
  name.

  For instance, an original file name as "myfile.dat" could derive chunk file names
  as "myfile_001.dat", "myfile_002.dat" and so on, where the numeric suffix is three
  digits wide. This width, as stated above, may range from 1 to 9 chars, so chunk
  files as "myfile_1.dat" or "myfile_000000001.dat" are valid as well.

  Note that a set of chunk files used to actually store a generic file must use
  the same width for the numeric shift. That is, if, for instance, the first
  chunk file uses a 4-digit numeric suffix, like in "myfile_0001.dat", all
  remaining chunks must use this very same suffix width.

  The width of the numeric suffix is treated in different ways depending on
  the kind of use this class is instended for.

  - When chunk file names are generated in read mode, the width of the
    suffix is automatically determined by this class, which takes care
    of analyzing the naming pattern in these chunk files.
  - On the contrary, when the generation of these names is done with
    the purpose of writing new files, it is the calling module
    which must set this width itself.

  In other words, when a "reader" uses this class to retrieve the name of
  the different chunk files, nothing is invented. The segmented_file_utils
  object simply scans the folder where the chunk files are stored and
  analyzes how these are named to provide with the correct chunk file
  names.
  
  On the contrary, when creating files, the calling module decides
  on the final naming, and this class is just a helper to simplifly the
  way these names are built. Therefore, the width of the numeric suffix
  is set by the caller.

  The way to use this class is roughly sketched below:

  - Instantiate a segmented_file_utils object (constructor)
  - Set it to work either in read or write mode.
    - When using the read mode, specify the general file name to use
      and whether names have to be generated in ascending or descending
      order. Method set_mode_read().
    - If using write mode, the general file name must be specified as
      well, but now the desired numeric suffix with must be set.
      Method set_mode_write().
  - Keep asking for a new chunk file name (method get_next_filename()) until either
    - A "no more files" condition is issued, when working in read mode or,
    - there's no need to generate more names if working in write mode.

  Below, a full working example is shown. Note that it is necessary to
  create several files named "test_NUMERICSUFFIX.dat" in the same
  directory where the code is run so when the read mode is used
  chunk files matching the correct naming pattern are found. Here, NUMERICSUFFIX
  stand for a series of numeric suffixes with any desired with (from
  1 to 9 characters wide) going from 1 to any desired number.

  For instance, four files using a 5-digit wide numeric suffix could
  be created: test_00001.dat, test_00002.dat, test_00003.dat and test_00004.dat.

  \code
  //
  //
  // This example shows how to use class segmented_file_utils to generate the
  // names of the chunks making a generic observation-events file.
  //
  // The code includes three different situations where names need to be
  // created:
  //
  //- For a reader that needs to retrieve these names in ascending order,
  //- for a reader that, in this case, needs to work in reversed order and
  //- for a writer (which always create files in ascending order).
  //

#include <iostream>                 // For cout.
#include "segmented_file_utils.hpp" // To access the segmented_file_util methods.


int
main
(char** argv,
 int    argc)
{
  {

    // These are the three objects that will generate names for a forward reader,
    // a backward reader and an writer respectively.

    segmented_file_utils sfu_for_reading_forward;
    segmented_file_utils sfu_for_reading_backwards;
    segmented_file_utils sfu_for_writing;

    // The name of the chunks we'll be retrieving.

    string chunk_filename;

    // The name of the generic file which is actually split into chunks.

    string filename = "test.dat";

    // Control variable to get error codes.

    int    status;

    // The width of the numeric suffix to use when generating names in the
    // writer example.

    int    suffix_width;

    // Control variable for loops.

    int    total_requested_chunks;

    // **** FIRST EXAMPLE ****

    //
    // Check the segmented_file_utils object in READ mode, FORWARD direction.
    //
    // For this example to work, there must exist some files in the current
    // directory that adhere to the naming conventions used to give names
    // to the chunks making the "virtual", generic input file.
    //

    // Set the sfu object to work in read mode and in forward direction.

    status = sfu_for_reading_forward.set_mode_read(filename, false);
    if (status != 0)
    {
      cout << "set_mode_read returned an error: " << status << endl;
      return 1;
    }

    cout << "Retrieving the names in read mode, forward reader" << endl;
    cout << "-------------------------------------------------" << endl << endl;

    // Get the name of the first chunk.

    status = sfu_for_reading_forward.get_next_filename(chunk_filename);

    if (status > 1)
    {
      // get_next_filename() returns 0 if everything is OK.
      // It returns 1 when NO more chunks are available, so strictly speaking,
      // this return code is not an error, but an "end of files" conditon.
      // OTher codes (>1) are actual error conditions.

      cout << "get_next_filename returned an error: " << status << endl;
      return 1;
    }

    while (status != 1)
    {
      cout << "'" << chunk_filename << "'" << endl;

      // Get the next chunk file name. Check for non-end-of-files condition too.

      status = sfu_for_reading_forward.get_next_filename(chunk_filename);
      if (status > 1)
      {
        cout << "get_next_filename returned an error: " << status << endl;
        return 1;
      }
    }

    cout << endl;

    // **** SECOND EXAMPLE ****

    //
    // Check the segmented_file_utils object in READ mode, BACKWARDS direction.
    //
    // For this example to work, there must exist some files in the current
    // directory that adhere to the naming conventions used to give names
    // to the chunks making the "virtual", generic input file.
    //

    // Set the sfu object to work in read mode and in forward direction.

    status = sfu_for_reading_backwards.set_mode_read(filename, true);
    if (status != 0)
    {
      cout << "set_mode_read returned an error: " << status << endl;
      return 1;
    }

    cout << "Retrieving the names in read mode, backwards reader" << endl;
    cout << "---------------------------------------------------" << endl << endl;

    // Get the name of the first chunk.

    status = sfu_for_reading_backwards.get_next_filename(chunk_filename);

    if (status > 1)
    {
      cout << "get_next_filename returned an error: " << status << endl;
      return 1;
    }

    while (status != 1)
    {
      cout << "'" << chunk_filename << "'" << endl;

      // Get the next chunk file name. Check for non-end-of-files condition too.

      status = sfu_for_reading_backwards.get_next_filename(chunk_filename);
      if (status > 1)
      {
        cout << "get_next_filename returned an error: " << status << endl;
        return 1;
      }
    }

    cout << endl;

    // **** THIRD EXAMPLE ****

    //
    // Check the segmented_file_utils object in WRITE  mode.
    //
    // The width of the numeric suffix is set in this example to 5
    // characters. This is so because it is the responsibility of
    // the calling module (now, this source code), to decide on
    // this value.
    //
    // The same applies to the number of chunks whose file name
    // has to be generated. In this example we ask for 10 of these.
    //

    suffix_width = 5;             // Our decision!
    total_requested_chunks = 10;

    // Set the sfu object to work in write mode.

    status = sfu_for_writing.set_mode_write(filename, suffix_width);

    if (status != 0)
    {
      cout << "set_mode_write returned an error: " << status << endl;
      return 1;
    }

    cout << "Retrieving the names in write mode" << endl;
    cout << "----------------------------------" << endl << endl;

    //
    // Get the name of the first chunk.
    //
    // Note that in write mode, method get_next_filename() will
    // never return 1 as error code, since the the "no more files"
    // condition will never happen.
    //
    // Therefore, the test of the error code for this method
    // only checks values greater than 1.
    //

    status = sfu_for_writing.get_next_filename(chunk_filename);

    if (status > 1)
    {
      cout << "get_next_filename returned an error: " << status << endl;
      return 1;
    }


    while ((status < 2) && (total_requested_chunks > 0))
    {
      total_requested_chunks--;

      cout << "'" << chunk_filename << "'" << endl;

      // Get the next chunk file name. Check for non-end-of-files condition too.

      status = sfu_for_writing.get_next_filename(chunk_filename);
      if (status > 1)
      {
        cout << "get_next_filename returned an error: " << status << endl;
        return 1;
      }
    }

    cout << endl;


    // That's all!!!

    return 0;
  }
}
\endcode

 */

class segmented_file_utils
{
  public:

    /// \brief Get the full file name for the next chunk making the
    /// generic file.
    /**
      \param next_filename On output, this parameter contains the name of the
             next chunk file.
      \return Error code. Error code values:
              - 0: Successful completion.
              - 1: No more files available.
              - 2: Operation mode (read or write) not set.

      Once a segmented_file_utils object has been instantiated and its mode
      of operation (set_mode_read() or set_mode_write()) it is possible to
      start querying for the names of the different chunks constituting the
      generic file.

      Each call to this method will provide with a new chunk file name, according
      to the settings given in the current operation mode.

      For instance, if a read reverse mode is used, the names of the successive
      chunk will be provided in descending order, starting with the higher
      available sequence number and ending at 1. On the contrary, if the read
      mode is set in forward direction, the numbering of the successive chunk
      file names will adhere to an ascending order.

      When the operation mode is write, the order always follows an ascending
      pattern.

      Again, when working in read mode, a "no more files" condition may arise.
      This is signaled by error code 1. This, in fact, is not an error; the
      method is just informing the calling module that no more chunks are
      available; therefore, when reading, error code 1 simply means that
      no more chunk file names may be provided because these have been exhausted.

      On the contrary, error code 1 is a real error; it means that the available
      number of chunk file names may be generated using the maximum numeric
      suffix width set by method set_mode_write(). For instance, if such
      width is 2, the maximum suffix that may be generated is 99 (2 digits);
      trying to generate one more (which would mean a suffix equal to 100)
      is impossible and therefore error code 1 is returned.

      Error code 2 means that this method has been called in the wrong
      sequence, that is, before an operation mode (see again set_mode_read()
      and set_mode_write()) has been selected.
     */

    int    get_next_filename     (string& next_filename);

    /// \brief Constructor.

           segmented_file_utils  (void);

    /// \brief Destructor.

           ~segmented_file_utils (void);

    /// \brief Set the operation mode to "read"
    /**
      \param filename The generic name of the file for which the read
             mode is about to be set.
      \param backwards_reader If true, the retrieval of the names of
             the chunks constituting a generic filename will be
             generated in reversed order. Otherwise, an ascending
             naming pattern will be followed.
      \return Error code. Error code values:
              - 0: Successful completion.
              - 1: The operation mode was already set.
              - 2: The file name may not be empty.
              - 3: Unable to identify the chunks making the generic file.

      Instruct segmented_file_utils object to work in read mode, which
      means that it will be responsible for finding how many chunks
      exist on disk and the width of the numeric suffix used to identify
      these automatically. If this method fails to identify these chunks,
      an error code will be issued.

      Parameter backwards_reader also instructs the segmented_file_utils
      object to retrieve the names of the aforementioned chunks in
      reversed order, that is returning the name of the last chunk
      first and then going down until the first one.

      This method may called only once, right after the segmented_file_utils
      has been instantiated. Should additional calls be made, an error
      code would be returned.

      Additionally, this method may not be called if the operation mode
      has been set to "write" (see set_mode_write()).

     */

    int    set_mode_read         (const string& filename, bool backwards_reader=false);

    /// \brief Set the operation mode to "write".
    /**
      \param filename The generic name of the file for which the write
             mode is about to be set.
      \param numeric_suffix_width Width in characters to use
             to generate the zero-padded numeric suffix used
             to build the names of the different chunks composing
             the generic file.
      \return Error code. Error code values:
              - 0: Successful completion.
              - 1: The operation mode was already set.
              - 2: The file name may not be empty.
              - 3: The value of parameter numeric_suffix_width
                   is out of range. Legal values: 1 to 9.

      Instruct segmented_file_utils object to work in write mode.

      Parameter numeric_suffix_width is used to build the string
      version of the numeric suffix used when building the names
      of the different chunks the generic file is composed of.

      This width must be in the range 1..9. An error code is
      issued if this precondition is not satisfied.

      This method may called only once, right after the segmented_file_utils
      has been instantiated. Should additional calls be made, an error
      code would be returned.

      Additionally, this method may not be called if the operation mode
      has been set to "read" (see set_mode_read()).

     */

    int    set_mode_write        (const string& filename, int numeric_suffix_width);

  protected:

    /// \brief Build the name of a chunk file given the generic file name,
    /// the chunk number, and numeric suffix width.
    /**
      \param filename The generic name for the files that are split into chunks
             using a numerical convention.
      \param number The number of the chunk whose file name is about to be generated.
      \param width The number of characters assigned to convert the chunk number
             to string form.
      \return The canonical file name for the chunk number "number" using a
             numerical, zero-padded suffix of width characters wide.

      This method builds the canonical name of the chunks actually making
      a file split into fragments.

      THIS IS THE PLACE TO IMPLEMENT THE NAMING CONVENTIONS FOR THESE CHUNKS.

      The convention now implemented by this method is:

      \<path_if_any\>\<generic_file_base_name_without_extension\>_\<numeric_suffix\>.\<original_extension_if_any\>

      where,

      - \<path_if_any\> is the path (including the last slash) as found in input
        parameter filename.
      - \<generic_file_base_name_without_extension\> is the base file name as
        found in input parameter filename, without the path, without the extension.
      - \<numeric_suffix\> is a string representation of the chunk number found in
        input parameter number. The width of this string is specified by input
        parameter widht. This suffix is padded with zeros if necessary.
      - \<original_extension_if_any\> is the file extension as found in input
        parameter filename.

      For exampke, assuming the following values:

      - filename is set to "/home/someuser/data/mydata.obs" (excluding quotes),
      - number is set to 3 and
      - width is set to 4,

      then the return value of this method would be:

      "/home/someuser/data/mydata_0003.obs" (excluding quotes)

      This method works for both forward- and backward-slash path specifications.

     */

    string build_chunk_name      (const string& filename, int number, int width);

    /// \brief Checks if a file exists.
    /**
       \param filename The name of the file whose existence needs to be checked.
       \return True if the file exist, false otherwise.
     */

    bool   file_exists           (const string& filename);

    /// \brief Split a full filename in two components: full path and base name
    /// and then the extension.
    /**
      \param filename The ful path and name of the file whose components have
             to be found.
      \param path_and_basename On output, it will contain the full path and
             base name (file name without extension) of the file name.
      \param extension Out output, it will contain the extension of the
             file name (no dot included).
     */

    void   filename_split        (const string& filename, string& path_and_basename, string& extension);

    /// \brief Given a filename, find out how many chunks the file is made of and
    /// determine the width of the numerical suffix used to label these.
    /**
      \param filename The generic file name whose actual chunks must be identified.
      \param number_of_chunks The number of actual chunks composing the generic file.
      \param suffix_width Width of the numeric suffix used to identify the individual
             chunks composing the generic file.
      \return Error code. Error code values:
              - 0: Successful completion.
              - 1: Unable to find the chunks composing the file.
     */

    int    find_all_chuncks      (const string& filename, int&  number_of_chunks, int&  suffix_width);

    /// \brief Returns a string version of a integer, left-padded with zeros up
    // up to the desired width.
    /**
      \param number The number to convert to string format.
      \param width The final width in characters of the output string.
      \return A string of at most "width" characters wide containing the string
             version of the input number, padded with zeros on the left side.

      If the requested width is smaller than the minimum width necessary to represent
      the input number as a string, this minimum width will be used.

      For example, if number 123 is converted to string using this method and
      the requested width is 2 (which is less than 3, the actual number of
      characters required) the output will be "123", which is 3 and not 2
      characters wide.
     */

    string zero_pad_number       (int number, int width);

  protected:

    /// \brief When working in read mode, this flag indicates wheter to work
    /// in forward or backwards direction.

    bool   backwards_reader_;

    /// \brief Number of the current chunk.

    int    chunks_current_;

    /// \brief Total of chunks making the generic file.

    int    chunks_total_;

    /// \brief The generic name of the file  which is actuall split into chunks.

    string filename_;

    /// \brief Maximum number of files that may be generated according to the
    /// width of the numeric file suffix.

    int    max_chunks_;

    /// \brief Read mode (unset, forward, backwards).

    int    mode_read_;

    /// \brief Operation mode (unset, reader, writer).

    int    mode_work_;

    /// \brief Width in characters of the string version of the numeric suffix.

    int    numeric_suffix_width_;


};

#endif // _segmented_file_utils_HPP_

