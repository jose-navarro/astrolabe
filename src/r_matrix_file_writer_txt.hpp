/** \file r_matrix_file_writer_txt.hpp
  \brief Class defining the interface of text r_matrix based file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_FILE_WRITER_TXT_HPP
#define R_MATRIX_FILE_WRITER_TXT_HPP

#include "r_matrix_based_file_writer.hpp"

//
// Define the type of newline character used depending on the operating
// system where this code is compiled. The first if covers all windows
// operating systems; the elif clause is targetted at Macintosh OS up to
// version 9.0. The last else covers all UNIX and Linux based systems (which
// includes Macintosh OS version 10 and up).
//

/// @brief Definition of the newline character sequence. The actual
/// value for this macro will depend on the operating system and compiler
/// used to build the code.

#ifdef _WIN32
    #define NEWLINE "\r\n"
#elif defined macintosh
    #define NEWLINE "\r"
#else
    #define NEWLINE "\n"
#endif

/**
 * @brief Class defining the interface of text r_matrix based file writers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * r_matrix_based_file_writer, creating text files as result.
 *
 * The output is formatted according to ASTROLABE conventions.
 *
 *
 */
class r_matrix_file_writer_txt : public r_matrix_based_file_writer
{
  public:

    /**
     * @brief Closes the text file writer, disabling any other
     *        further action on it.
     *
     * Closes the text file writer, flushing any pending data
     * to the underlying file. Once close is executed, it is not possible
     * to perform any other actions on the writer besides destroying it.
     *
     * @return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the file.
     *        - 2: I/O error while writing data to disk.
     */
    virtual int  close                    (void);

    /**
     * @brief Default constructor.
     *
     */
                 r_matrix_file_writer_txt (void);

    /**
     * @brief Destructor.
     *
     * Destroys the text writer. It also closes the underlying file
     * if no close() has been issued by the calling module, in order
     * to guarantee that all data is flushed to disk.
     */

    virtual     ~r_matrix_file_writer_txt (void);

    /**
     * @brief Write an r-record.
     *
     * @param active The r-record is active (true) or removed (false).
     * @param time Time tag of the r-record.
     * @param n_correlation_values Number of correlation values to
     *        write. Must be > 0.
     * @param the_correlation_values The correlation valus to write.
     *
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: File is not open so it is not possible to write.
     *        - 2: I/O error detected while writing to disk.
     *        - 3: Invalid number of correlation values. It must be > 0.
     *
     */

    virtual int  write_r                  (bool         active,
                                           double       time,
                                           int          n_correlation_values,
                                           double*      the_correlation_values);



  protected:

    /// @brief Default format used to write double values.

    char* format_default_double_format_;

    /// @brief Fixed part of the format used for r-records.

    char* format_fixed_r_;

    /// @brief Size in characters, once written, taken by any r tag.
    /// Includes the preceding whitespace, the tag itself. The ending new line
    /// is also tallied; newline characters may be implemented in different
    /// ways (LF, CR, LF+CR, CR + LF) by different operating systems; the right
    /// newline character combination is selected in compilation time and the
    /// size of this combination is computed afterwards automatically.

    int sizeof_closing_tag_;

    /// @brief Size in characters, once written, taken by a single double value
    /// (time tag, correlation values). Includes the whitespace preceding
    /// the double value itself.

    int sizeof_double_value_;

    /// @brief Size in characters, once written, of the fixed par of the r-record.
    /// Includes opening tag, 1 whitespace, the code (always 3 characters),
    /// 1 whitespace, identifier, 1 whitespace and time tag. REVISAR!!!!!!!

    int sizeof_fixed_r_;
};

#endif // R_MATRIX_FILE_WRITER_TXT_HPP
