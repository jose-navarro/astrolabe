/** \file astrolabe_header_file_writer.hpp
  \brief  Provides with a writer for ASTROLABE header files.
  \addtogroup ASTROLABE_data_IO Reading and writing ASTROLABE data
*/

#ifndef ASTROLABE_HEADER_FILE_WRITER_HPP
#define ASTROLABE_HEADER_FILE_WRITER_HPP

#include <stdio.h>
#include <time.h>
#include <string>
#include <sstream>

#include "lineage_data.hpp"

/**
   \brief  Provides with a writer for ASTROLABE header files.

   \ingroup ASTROLABE_data_IO

   This class provides with the necessary methods to write ASTROLABE header
   files.

   Note that only header files pointing to file devices are supported. This
   class is not able to generate header file pointing to socket devices.

   The way to use this writer is pretty straightforward:

   - Instantiate the writer.
   - Configure the writer using method set\_data\_channel. There, the following
     information must be provided:
     - Name of the (output) ASTROLABE header file,
     - Base name for the external files.
     - Kind of file (type attribute).
     - Binary or text file.
     - A lineage object to include in the header files.
   - Call method write().
*/

class astrolabe_header_file_writer
{
  public:

    /// \brief Default constructor.

         astrolabe_header_file_writer (void);

    /// \brief Destructor

        ~astrolabe_header_file_writer (void);

    /// \brief Set the parameters defining the ASTROLABE header file.
    /**
      \param astrolabe_header_file_name The name of the output ASTROLABE header file name.
      \param external_base_file_name Base name for the external files that will actually
             store data.
      \param device_type The kind of file that will be stored in the external files, as
             for instance, obs-e.
      \param binary When true, it will state that the format of the external files will be
             binary. Text otherwise.
      \param lineage_info Pointer to a lineage_data object containing at least the mandatory
             parts of a lineage. It will be written to the output header file.
    */

    void set_data_channel             (const string&       astrolabe_header_file_name,
                                       const string&       external_base_file_name,
                                       const string&       device_type,
                                       bool                binary,
                                       const lineage_data* lineage_info);

    /// \brief Write the astrolabe header file.
    /**
      \return Error code. Error code values:
               - 0: Successful completion.
               - 1: One or more parameters defining the data channel are empty or NULL.
                    Make sure that set_data_channel() has been called properly.
               - 2: Error opening / writing / closing the output file.

      Once that the parameters defining the contents of the ASTROLABE header file
      have been set using set_data_channel, it is possible to call this method
      to write it.

      Note that this method will complain about values not set (error code 2) so
      it is important to guaranteee that all the parameters in set_data_channel are
      properly set.

    */

    int write                         (void);

  protected:

    /// \brief Type of device channel where data will be stored.

    string                            device_type_;

    /// \brief Base name of the external files written when working in
    /// external mode.

    string                            external_base_file_name_;

    /// \brief Name of the output astrolabe header file.

    string                            header_file_name_;

    /// \brief Flag that indicates that a file data channel must be
    /// written in binary mode.

    bool                              file_mode_is_binary_;

    /// \brief The lineage information to write in the header of the
    /// observation-events file.

    const lineage_data*               lineage_info_;

};

#endif // ASTROLABE_HEADER_FILE_WRITER_HPP
