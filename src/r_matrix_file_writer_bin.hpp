/** \file r_matrix_file_writer_bin.hpp
  \brief Class defining the interface of binary r_matrix file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_FILE_WRITER_BIN_HPP
#define R_MATRIX_FILE_WRITER_BIN_HPP

#include "r_matrix_based_file_writer.hpp"

/**
 * @brief Class defining the interface of binary r_matrix file writers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * r_matrix_based_file_writer, creating binary files as result.
 *
 *
 */
class r_matrix_file_writer_bin : public r_matrix_based_file_writer
{
  public:

    /**
     * @brief Closes the binary file writer, disabling any other
     *        further action on it.
     *
     * Closes the binary file writer, flushing any pending data
     * to the underlying file. Once close is executed, it is not possible
     * to perform any other actions on the writer besides destroying it.
     *
     * @return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the file.
     *        - 2: I/O error while writing data to disk.
     */
    virtual int  close                       (void);

    /**
     * @brief Default constructor.
     *
     */
                 r_matrix_file_writer_bin    (void);

    /**
     * @brief Destructor.
     *
     * Destroys the binary writer. It also closes the underlying file
     * if no close() has been issued by the calling module, in order
     * to guarantee that all data is flushed to disk.
     */
    virtual     ~r_matrix_file_writer_bin    (void);

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

};

#endif // R_MATRIX_FILE_WRITER_BIN_HPP
