/** \file r_matrix_file_reader_bin_backward.hpp
  \brief Class defining the interface of backwards r_matrix binary file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_FILE_READER_BIN_BACKWARD_HPP
#define R_MATRIX_FILE_READER_BIN_BACKWARD_HPP

#include "r_matrix_based_file_reader_bin.hpp"

/**
 * @brief Class defining the interface of backwards r_matrix binary file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * r_matrix_based_file_reader_bin, reading r_matrix files stored in binary format in
 * backwards direction.
 *
 * The format used to store r_matrix binary file is the same used to store
 * observation-event files. Nonetheless, epochs, in r_files, are made of a
 * single r-record. Therefore, there exist one b (backtracking) record per each
 * r-record. This  means that reading backwards a r_matrix files implies reading
 * single records, not several of these per epoch.
 *
 * The process finishes when the first record in the file has been read
 *(end-of-file or, better put, beginning-of-file condition).
 *
 */

class r_matrix_file_reader_bin_backward : public r_matrix_based_file_reader_bin
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

    virtual int  open                               (const char*  file_name);

    /**
     * @brief Default constructor.
     */

                 r_matrix_file_reader_bin_backward  (void);

    /**
     * @brief Destructor.
     */

    virtual      ~r_matrix_file_reader_bin_backward (void);

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

    virtual int  read_active_flag                   (bool&       active);

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
     *         - 5: Invalid r-record. Several conditions may arise that are
     *              represented by this error code; for instance, this error
     *              would state that the record being read is not complete
     *              (the limits of the record have been reached while
     *              reading data).
     *
     * This method must be called right after read_time() has been invoked.
     * Calling this method at any other moment will arise an error condition.
     *
     * A successful execution will provide the values of the correlation values
     * stored in the the_correlation_values output parameter.
     */

    virtual int  read_r_data                        (int&     n_correlation_values_found,
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

    virtual int  read_time                          (double&      time);

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
     *         - 5: NOT USED.
     *         - 6: NOT USED.
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

    virtual int  read_type                          (char& record_type);


  protected:

    /**
     * @brief Update the current record pointer and size members so these
     *        point to the previous record to be processed.
     *
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Beginning of file (no more data).
     *        - 2: I/O error while reading (might happen if the file
     *             has not been opened).
     *
     * Once that one record has been read, it is necessary to backtrack
     * so the pointers and size members pointing to the previous record
     * in the file are updated.
     *
     * Due to the buffered nature of this reader, the information related
     * to the previous record may be already stored in the data buffer. If so,
     * the method just computes the new record limits and returns.
     *
     * On the contrary, if the data buffer has been exhausted, it will be
     * necessary to read more data from the input file (see read_prev_chunk())
     * and load it in the internal data buffer. Once done, the limits of
     * the previous record may be computed. Note that actually reading the
     * input file may produce I/O errors. See the list or error codes for
     * more information.
     *
     * The information about record limits is stored in members
     * - record_cur_backtrack_index_
     * - record_cur_offset_
     * - record_remaining_bytes_
     */

    int          backtrack_to_previous_record       (void);

    /**
     * @brief Read as many bytes as possible from the input file to fill the
     *        data buffer. The read process is performed in backwards direction.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Beginning of file (no more data).
     *        - 2: I/O error while reading (might happen if the file
     *             has not been opened).
     *
     * Read the input file backwards. Since it is not possible to physically
     * read a file in backwards direction, this is implemented playing with
     * the fseek current positions.
     *
     * The current fseek position is the first byte in the file
     * that must not be read because it has already been read.
     *
     * The dimension of the internal data buffer, as well as the current fseek
     * position, are used to compute the position in the file where the read
     * operation must start at.
     *
     * For instance, if the current fseek position is 1000 and the dimension
     * of the internal data buffer is 300, the new fseek position will
     * be 1000 - 300 = 700. Then, 300 bytes will be read starting at position
     * 700 in the file.
     *
     * Once the actual read process finishes, the new fseek position becomes
     * the current one, thus being ready for a new call to this method.
     *
     * Remember: the data already stored in positions 0..start_at of the buffer
     * is left undisturbed.
     */
    int          read_prev_chunk                    (void);


  protected:

    /// @brief Position in the data buffer where actual data starts (due to
    /// the backwards nature of the read process, the absolute beginning of
    /// the data buffer may not store actual data but just garbage).

    int          chunk_start_index_;

    /// @brief Number of bytes in the input file that have not been read yet.

    int          file_bytes_unread_;

    /// @brief Index (in the data buffer) where the backtracking information
    /// for the current record is stored.

    int          record_cur_backtrack_index_;

    /// @brief Index (in the data buffer) of the next position to read when
    /// processing the current record.

    int          record_cur_offset_;

    /// @brief Number of bytes still to be processed in the current record.

    int          record_remaining_bytes_;
};

#endif // R_MATRIX_FILE_READER_BIN_BACKWARD_HPP
