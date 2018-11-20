/** \file r_matrix_file_reader_txt_backward.hpp
  \brief Class defining the interface of backwards correlation matrix text file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_FILE_READER_TXT_BACKWARD_HPP
#define R_MATRIX_FILE_READER_TXT_BACKWARD_HPP

#include "r_matrix_based_file_reader_txt.hpp"

/**
 * @brief Class defining the interface of backwards correlation matrix text file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * r_matrix_based_file_reader_txt, reading correlation matrix files stored in
 * text format in backwards direction.
 *
 */

class r_matrix_file_reader_txt_backward : public r_matrix_based_file_reader_txt
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
     *        - 6: Malformed records found.
     *        - 7: Internal buffer too small.
     *
     * Once a reader has been instantiated (and, optionally, the
     * size of the internal buffer set using set_buffer_size()), it is
     * possible to open the underlying file to enable I/O operations.
     *
     * This method opens the file and instantiates the internal memory
     * buffer used to speed up I/O operations, enabling the different
     * read operations available in this class.
     *
     * Note that in the specific case of this reader, some additional operation
     * are performed. Once the file is open, this method reads the last
     * part of the file (as many bytes as the size of the internal buffer) and
     * tries to analyze its contents to prepare things for the rest of the
     * methods in this library.
     *
     * During this process, it may happen that malformed records be detected
     * (error code 6) or that the size of the internal buffer proves to be
     * too small to perform such analysis (error code 7).
     */

    virtual int  open                                  (const char*  file_name);

    /**
     * @brief Default constructor.
     */

                 r_matrix_file_reader_txt_backward     (void);

    /**
     * @brief Destructor.
     */
                 ~r_matrix_file_reader_txt_backward    (void);

    /**
     * @brief Read the event record type.
     *
     * @param record_type The type of the record being read.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End-of-file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Malformed record found.
     *         - 5: NOT USED.
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
     * either an 'r' (always lowercas) denoting the type or record
     * being read (always an r-record).
     */

    virtual int  read_type                             (char& record_type);

  protected:

    /**
     * @brief Locates the position where the first complete record starts. The
     *        search is made in the internal data buffer.
     * @param start The position in the internal data buffer where the search
     *        should start.
     * @param position If the first record is found, this contains the absolute
     *        position from the beginning of the internal buffer where such
     *        epoch is located.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Buffer exhausted, first record not found.
     *        - 2: Malformed record found.
     *        - 3: Buffer is empty, it contains whitespace only.
     *
     * This method scans the internal data buffer in order to locate the
     * position where the first COMPLETE record is located.
     *
     * When the first record is found, its starting position in the data buffer
     * is copied to parameter position. Note (1) that this position is absolute,
     * that is, referred to the beginning of the internal buffer and (2) that
     * the beginning of a record is the first opening '<' found in such
     * record.
     *
     * If it is not possible to find a complete record in the buffer, an error
     * code is returned.
     */

    int          find_first_complete_record            (int start, int& position);

    /**
     * @brief Computes the pointers that define the area where the previous
     *        unprocessed epoch is stored. Computes also the number of full
     *        records included in this epoch. The internal data buffer is
     *        searched BACKWARDS.
     * @param start Absolute position in the internal data buffer where the
     *        search for the previous epoch starts.
     * @param record_first Absolute position in the internal data buffer
     *        where the first record in the epoch starts.
     * @param record_last Absolute position in the internal data buffer where
     *        the first record in the epoch ends.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Malformed records found (invalid file).
     */
    int          find_prev_record                      (int  start,
                                                        int& record_first, int& record_last);

     /**
     * @brief Computes the starting and ending positions of the previous
     *        record to read.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: End of file (no more data).
     *        - 2: Malformed record found.
     *        - 3: I/O error detected while reading the file.
     *        - 4: Buffer too small.
     *
     * Computes the starting and ending positions of the previous record.
     *
     * Note that calling find_prev_record_limits() may trigger an I/O operation if
     * the internal data buffer is already exhausted. This, also, may lead
     * to an end-of-file condition as well as errors related to the
     * identification of the first record in the chunk of data read.
     *
     * Record limits are set using members cur_record_start_, cur_record_end_
     * and cur_record_cur_;
     */

    int          find_prev_record_limits               (void);

    /**
     * @brief Locates a record boundary, that is a sequence given by an
     *        opening <, plus whitespace only and then an opening <.
     * @param from Absolute position in the internal data buffer where to start
     *        the search from.
     * @param next_opening The position in the internal data buffer where
     *        the first opening < is found.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Buffer exhausted while looking for the record.
     */
    int          find_record_boundary                  (int from, int& next_opening);

    /**
     * @brief Looks for the absolute limits of a complete r-record.
     *        The search is performed BACKWARDS.
     * @param from Absolute position in the internal data buffer where to start
     *        the search from.
     * @param start Absolute position of the beginning of the record.
     * @param end Absolute position of the end of the record.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Buffer exhausted while looking for the record.
     *        - 2: Malformed record found.
     *
     * The search finishes either successfully when a complete record is found.
     * An unsuccessful search implies a bad record found in the buffer.
     */
    int          find_record_bwd                       (int from,  int& start, int& end);

    /**
     * @brief Looks for the absolute limits of a complete event record.
     *        The search is performed in FORWARD direction.
     * @param from Absolute position in the internal data buffer where to start
     *        the search from.
     * @param start Absolute position of the beginning of the record.
     * @param end Absolute position of the end of the record.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Buffer exhausted while looking for the record.
     *        - 2: Malformed record found.
     *
     * The search finishes either successfully when a complete record is found
     * or unsuccessfully if the end of the internal data buffer is reached.
     */
    int          find_record_fwd                       (int from,  int& start, int& end);

    /**
     * @brief Read as many bytes as possible from the input file to fill the
     *        data buffer. The read process is performed in backwards direction.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Beginning of file (no more data).
     *        - 2: I/O error while reading (might happen if the file
     *             has not been opened).
     *        - 3: Malformed record found.
     *        - 4: Buffer too small to hold required data; unable to find
     *             the first epoch in the data chunk read because of this.
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
     * So, going back to the previous example, if start amounts to 15, the
     * new fseek position is computed as (1000 - (300-15)) = 715, and the
     * actual amount of bytes read is 300 - 15 = 285.
     *
     * Once the actual read process finishes, the new fseek position becomes
     * the current one, thus being ready for a new call to this method.
     *
     * Remember: the data already stored in positions 0..start_at of the buffer
     * is left undisturbed.
     */

    int          read_prev_chunk                       (void);

  protected:


    /// @brief Position in the data buffer where actual data starts (due to
    /// the backwards nature of the read process, the absolute beginning of
    /// the data buffer may not store actual data but just garbage).

    int          chunk_start_index_;

    /// @brief Pointer to the first non-processed position (data not yet
    /// "read") in the data buffer.

    int          data_buffer_current_;

    /// @brief Number of bytes in the input file that have not been read yet.

    int          file_bytes_unread_;
};

#endif // R_MATRIX_FILE_READER_TXT_BACKWARD_HPP
