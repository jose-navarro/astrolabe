/** \file r_matrix_file_reader_bin_forward.hpp
  \brief Class defining the interface of forward r_matrix binary file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_FILE_READER_BIN_FORWARD_HPP
#define R_MATRIX_FILE_READER_BIN_FORWARD_HPP

#include "r_matrix_based_file_reader_bin.hpp"

/**
 * @brief Class defining the interface of forward r_matrix binary file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * r_matrix_based_file_reader_bin, reading r_matrix files stored in binary format in
 * forward direction.
 *
 */

class r_matrix_file_reader_bin_forward : public r_matrix_based_file_reader_bin
{
  public:

    /**
     * @brief Opens the reader, enabling I/O operations.
     *
     * @param file_name Name of the file to read.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The file was already open.
     *        - 2: Unable to open the selected file.
     *        - 3: Not enough memory to allocate the internal buffer.
     *        - 4: End of file condition detected - the file is empty.
     *        - 5: I/O error detected while reading the file.
     *        - 6: NOT USED.
     *        - 7: NOT USED.
     *
     * Once a reader has been instantiated (and, optionally, the
     * size of the internal buffer set using set_buffer_size()), it is
     * possible to open the underlying file to enable I/O operations.
     *
     * This method opens the file and instantiates the internal memory
     * buffer used to speed up I/O operations, enabling the different
     * read operations available in this class.
     */

    virtual int  open                              (const char* file_name);

    /**
     * @brief Default constructor.
     */

                 r_matrix_file_reader_bin_forward  (void);

    /**
     * @brief Destructor
     */

                 ~r_matrix_file_reader_bin_forward (void);

    /**
     * @brief Reads the active flag.
     *
     * @param active Value of the r-record's active flag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *
     * Once the type of the record has been obtained using read_type(), it
     * is possible to read its active flag. This is the task of this method.
     *
     * Note that read_active_flag() must be called right after read_type() has
     * been invoked and before any other read_* method is used; otherwise,
     * an error condition will arise.
     *
     */

    virtual int  read_active_flag                  (bool& active);

    /**
     * @brief Read correlation matrix values in an r-record.
     *
     * @param n_correlation_values_found The number of correlation values
     *        read from the r-record.
     * @param the_correlation_values Array that will store the correlation
     *        values. The number of actual values read will be stored in parameter
     *        n_correlation_values_found.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: NOT USED.
     *         - 5: NOT USED.
     *
     * This method must be called right after read_time() has been invoked.
     * Calling this method at any other moment will arise an error condition.
     *
     * A successful execution will provide the values of the correlation values
     * stored in the the_correlation_values output parameter.
     */

    virtual int  read_r_data                       (int&     n_correlation_values_found,
                                                    double*& the_correlation_values);

    /**
     * @brief Read the time tag.
     *
     * @param time The time tag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: NOT USED
     *
     * This method must be called right after a call to read_active_flag() has
     * been made.  Using this method in a different moment will arise an
     * error condition.
     *
     * Once run, time will contain the value of the time tag for the
     * record being read.
     */

    virtual int  read_time                         (double& time);

    /**
     * @brief Read the event record type.
     *
     * @param record_type The type of the record being read, which must
	   *        be always an "r".
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End-of-file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Malformed record found.
     *         - 5: Unexpected end of file. Invalid file. This error code
     *              differs from code 1 in the sense that for code 1,
     *              absolutely no data was found in the file when trying
     *              to read a new type tag; this means that the end
     *              of the file has been legally reached. On the contrary,
     *              code 5 means that some data could be read before all
     *              the needed information could be retrieved; this points
     *              to a corrupted file.
     *         - 6: Internal buffer to small to be able to parse input file.
     *
     * read_type() is the method that starts the process of reading a
     * full r_matrix record.
     *
     * It may be invoked right after open() or read_r_data() have
     * been called only. Using this method at any other moment will
     * produce an error condition.
     *
     * Note that since this method starts the reading of a brand new
     * record, an end-of-file (or beginning-of-file) condition may
     * arise (see is_eof() for a discussion on end-of-file and
     * beginnin-of-file conditions treated uniformly by this library).
     *
     * read_type() will indicate that an end-of-file condition has been
     * detected by means of an specific error code. Other error codes
     * besides the one indicating the end of file are also possible.
     *
     * Therefore, the calling module must check the error code returned
     * by read_type() and, if not zero, check immediatelly if it
     * corresponds to either a legal end-of-file condition (using
     * is_eof()) or to any other problem.
     *
     * End-of-file conditions must not therefore be treated as errors, but
     * as the normal way to detect that the data in the input file has
     * been exhausted. See the example in the detailed description of
     * this class for an example on how to check and treat this situation.
     *
     * After a successful completion, the record_type parameter will contain
     * an 'r' (always lowercas) denoting the type or record
     * being read (always an r-record).
     */

    virtual int  read_type                         (char& record_type);

  protected:

    /**
     * @brief Moves the bytes still not processed in the data buffer to
     *        the beginning of this buffer.
     * @return The total number of bytes actually moved. May be zero.
     */

    int          move_remaining_up                 (void);

    /**
     * @brief Read as many bytes as possible from the input file to fill the
     *        data buffer.
     * @param start_at First position in the data buffer to be filled when
     *        reading the input file. The preceding positions in the buffer
     *        are left undisturbed.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: End of file (no more data).
     *        - 2: I/O error while reading (might happen if the file
     *             has not been opened).
     *
     * Read the input file starting at the current (fseek) position as many
     * bytes as possible to either exhaust the data available or to fill
     * the data buffer.
     *
     * The data already stored in positions 0..start_at of the buffer are
     * left undisturbed.
     */

    int          read_next_chunk                   (int start_at);

    /**
     * @brief Read (to skip) the backtracking information found at the end
     *        of each r-record.
     *
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: Unexpected end of file.
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: NO proper backtracking information found.
     *
     * This method is called by read_r_data to skip the backtracking information
     * present at the end of each r-record and that is absolutely unuseful when
     * reading r-matrix_files in forward direction.
     *
     */

    int          skip_backtrak_tag                 (void);

  protected:

    /// @brief Number of bytes actually stored in the data buffer.

    int          data_buffer_bytes_;

    /// @brief Pointer to the first non-processed position (data not yet
    /// "read") in the data buffer.

    int          data_buffer_current_;

    /// @brief Flag to show that no proper backtracking information has
    /// been found at the end of an r-record.

    bool         invalid_backtrack_info_found_;
};

#endif // R_MATRIX_FILE_READER_BIN_FORWARD_HPP
