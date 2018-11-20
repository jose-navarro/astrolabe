/** \file gp_compression_manager.hpp
\brief Minimalistic set of compression and decompression
       utilities to deal with ASTROLABE (compressed)
       navigation files.
\ingroup ASTROLABE_data_IO
*/

#ifndef _GP_COMPRESSION_MANAGER_HPP_
#define _GP_COMPRESSION_MANAGER_HPP_

#include <string>
#include <vector>
#include <stdlib.h>

#include "ZipFile.h"

using namespace std;

/// \brief Entity class to store a the \<c\> element (covariance matrices)
/// in ASTROLABE metadata files.
/**
  This class provides the minimum set of methods to compress, decompress
  and list the contents of ASTROLABE (compressed) navigation files.

  It relies on four libraries, namely

  - ZipLib (the higher level interface used by this class)
  - zlib, lzma and bzip2 (lower level compression and decompression utilities).

  Besides the usual constructors and destructors, only three methods are
  provided:

  - list - To retrieve the list of files included in a compressed zip archive,
  - unzip - To decomrpress a navigation file, extracting its whole contents and
  - zip - To create a navigation file, that is, compressing the individual
    files making it.

  Note that this class makes no assumptions about the contents (set of files)
  that must be included in a navigation file. It works as a pure
  compressor / decompressor, including no intelligence on the validity of
  the contents of navigation files.

  The following is a fully functional example showing how to use this class.

\code
#include <string>
#include <vector>
#include <stdlib.h>

#include "gp_compression_manager.hpp"

using namespace std;

int
main
(int argc, char** argv)
{
  {
    gp_compression_manager cmanager;
    string                 destination_folder;
    size_t                 i;
    vector<string>         the_files;
    int                    retcode;
    string                 ziparchive;

    // Set the name of the output zip archive.

    ziparchive = "archive.zip";

    // Get the names of the files to zip from the command line.

    the_files.clear();
    for (i = 1; i < (size_t)argc; i++) the_files.push_back(argv[i]);

    // Delete the zip archive should it exist.

    remove(ziparchive.c_str());

    // Zip the files.

    retcode = cmanager.zip(ziparchive, the_files);
    if (retcode != 0)
    {
      cout << "Error adding files to zip archive" << endl;
      return 1;
    }

    // List the contents of the newly zipped file.

    the_files.clear();
    retcode = cmanager.list(ziparchive, the_files);

    if (retcode != 0)
    {
      cout << "Error opening, or getting the number of entries, or" << endl;
      cout << " retrieving the names of the files in the zip archive" << endl;
      return 1;
    }

    cout << "Listing the contents of the newly zipped archive" << endl;
    for (i = 0; i < the_files.size(); i++)
    {
      cout << "  " << the_files[i] << endl;
    }
    cout << endl;

    //
    // Unzip the archive. Note that we use a subfolder to do it.
    // The subfolder must exist or the unzip operation will fail.
    // Note that if an empty destination folder ("") would have
    // been specified, then the current default directory would be
    // used.
    //

    destination_folder = "./unzipped/";
    retcode = cmanager.unzip(ziparchive, destination_folder);
    if (retcode != 0)
    {
      cout << "Error opening or getting the number of entries or" << endl;
      cout << " trying to extract a file from the zip archive" << endl;
      return 1;
    }

  }

  return 0;
}
\endcode
*/

class gp_compression_manager
{
  public:

    /// \brief Default constructor.

        gp_compression_manager  (void);

    /// \brief Destructor.

        ~gp_compression_manager (void);

    /// \brief  List the contents of a (compressed in zip format) navigation file.
    /**
      \param ziparchive Full path and name of the compressed (zipped) navigation file
             whose contents is about to be listed.
      \param the_files The names of the files included in the compressed (zipped)
             navigation file.
      \return Error code. 0 means successful completion. 1 means that it has not been
             possible to open the compressed archive. 2 implies that it has not been
             possible to retrieve the number of entries (files) included in the
             zipped archive. 3 means that it has not been possible to retrieve the
             name of one or more of the files included in the zipped archive.

      The contents of parameter the_files is only reliable when the returned error
      code is zero.
    */

    int list                    (const string& ziparchive, vector<string>& the_files);

    /// \brief  Extract the whole contents of a (compressed in zip format) navigation file.
    /**
      \param ziparchive Full path and name of the compressed (zipped) navigation file
             whose contents is about to be extracted.
      \param destination_folder Path to the folder where the files included in the
             compressed (zipped) navigation files will be extracted. Passing the
             empty string ("") implies that the current default directory
             (whatever it is) will be used. Trailing slashes (either / or \) may
             be either included or omitted (the method takes care of adding
             the necessary trailing slashes if ommited).
      \return Error code. 0 means successful completion. 1 means that it has not been
             possible to open the compressed archive. 2 implies that it has not been
             possible to retrieve the number of entries (files) included in the
             zipped archive. 3 means that it has not been possible to extract 
             one or more of the files included in the zipped archive.
    */

    int unzip                   (const string& ziparchive, const string& destination_folder);

    /// \brief  List the contents of a (compressed in zip format) navigation file.
    /**
      \param ziparchive Full path and name of the compressed (zipped) navigation file
             that is about to be created.
      \param the_files The names of the files to include in the compressed (zipped)
             navigation file.
      \return Error code. 0 means successful completion. 1 means that it has not been
             possible to add one or more files to the compressed navigation file.

      The compressed archive will be correctly zipped only when the returned code
      is zero.
    */

    int zip                     (const string& ziparchive, vector<string>& the_files);
};

#endif // _GP_COMPRESSION_MANAGER_HPP_
