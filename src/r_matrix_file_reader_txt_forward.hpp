/** \file r_matrix_file_reader_txt_forward.hpp
  \brief Class defining the interface of forward r_matrix text file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_FILE_READER_TXT_FORWARD_HPP
#define R_MATRIX_FILE_READER_TXT_FORWARD_HPP

#include "r_matrix_based_file_reader_txt.hpp"

/**
 * @brief Class defining the interface of forward r_matrix text file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * r_matrix_based_file_reader_txt, reading correlation matrix files stored
 * in text format in forward direction.
 *
 */

class r_matrix_file_reader_txt_forward : public r_matrix_based_file_reader_txt
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

    virtual int  open                              (const char*  file_name);
	
	/**
     * @brief Default constructor.
     */

                 r_matrix_file_reader_txt_forward  (void);

    /**
     * @brief Destructor.
     */
                 ~r_matrix_file_reader_txt_forward (void);

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
     *         - 5: Unexpected end of file. Invalid file. This error code
     *              differs from code 1 in the sense that for code 1,
     *              absolutely no data was found in the file when trying
     *              to read a new type tag; this means that the end
     *              of the file has been legally reached. On the contrary,
     *              code 5 means that some data could be read before all
     *              the needed information could be retrieved; this points
     *              to a corrupted file.
     *         - 6: NOT USED.
     *
     * read_type() is the method that starts the process of reading a
     * full event record, either l- or o-.
     *
     * It may be invoked right after open(), read_l_data() or read_o_data()
     * have been called only. Using this method at any other moment will
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
     * been exhausted. See the detailed description of class
     * observation_file_reader for an example on how to check and treat this situation.
     *
     * After a successful completion, the record_type parameter will contain
     * either an 'l' or an 'o' character (always lowercas) denoting the
     * type of record being read (l- and o-records correspondingly).
     */
    virtual int  read_type                            (char& record_type);

  protected:

    /**
     * @brief Computes the starting and ending positions of the next
     *        record. To do it, the data buffer is scanned starting
     *        at the first non-used position (data_buffer_current_).
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: End of file (no more data).
     *        - 2: Unexpected end of file (corrupted file).
     *        - 3: I/O error detected while reading the file.
     *
     * Search starts at the first available position in the data buffer
     * (data_buffer_current_). If the search process goes beyond the
     * last position of the data buffer (data_buffer_bytes_) then a new
     * chunk of data will be read (using read_next_chunk()). If this
     * last procedure finds that the data is exhausted, find_next_record_limits()
     * will report an end of file.
     *
     * Record limits are set using members cur_record_start_, cur_record_end_
     * and cur_record_cur_;
     */

    int          find_next_record_limits           (void);

    /**
     * @brief Moves the bytes still not processed in the data buffer to
     *        the beginning of this buffer.
     *
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
     *
     */
    int          read_next_chunk                   (int start_at);

  protected:

    /// @brief Number of bytes actually stored in the data buffer.

    int          data_buffer_bytes_;

    /// @brief Pointer to the first non-processed position (data not yet
    /// "read") in the data buffer.

    int          data_buffer_current_;
};

#endif // R_MATRIX_FILE_READER_TXT_FORWARD_HPP
