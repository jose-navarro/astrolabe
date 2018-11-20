/** \file gp_navdir_data_writer.hpp
\brief Writes to disk, in XML format, the information stored
       in a gp_navdir_data object.
\ingroup ASTROLABE_metadata
*/

#ifndef _GP_NAVDIR_DATA_WRITER_HPP_
#define _GP_NAVDIR_DATA_WRITER_HPP_

#include <stdio.h>
#include <time.h>
#include <string>
#include <sstream>

#include "gp_navdir_data.hpp"

using namespace std;

/**
\brief  Provides with a writer for ASTROLABE header files.

\ingroup ASTROLABE_metadata

  This class provides with the necessary methods to write ASTROLABE navigation
  directory files.

  The way to use this writer is pretty straightforward:

  - Instantiate the writer.
  - Call method write(), passing both a gp_navdir_data object containing all
    the information that will be written in the output file as well as the
    name of such file.
*/

class gp_navdir_data_writer
{
  public:

    /// \brief Default constructor

        gp_navdir_data_writer  (void);

    /// \brief Destructor

        ~gp_navdir_data_writer (void);


    /// \brief Write the astrolabe navigation directory file.
    /**
      \pre (!navigation_dir_file_name.empty())
      \param navdir_data The navigation directory data to be written in the
             output file.
      \param navigation_dir_file_name Full path and name of the navigation file
             to write.
      \return Error code. Error code values:
      - 0: Successful completion.
      - 1: The name of the output file is the empty string.
      - 2: Error opening / writing / closing the output file.
    */

    int write                  (gp_navdir_data& navdir_data, const string& navigation_dir_file_name);

  protected:

    /// \brief Name of the output navigation directory file.

    string  navigation_directory_file_name_;
};

#endif // _GP_NAVDIR_DATA_WRITER_HPP_
