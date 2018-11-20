/** \file r_matrix_based_file_reader_bin.hpp
  \brief Abstract class to define the interface of binary r_matrix_based file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_BASED_FILE_READER_BIN_HPP
#define R_MATRIX_BASED_FILE_READER_BIN_HPP

#include "r_matrix_based_file_reader.hpp"

/**
 * @brief Abstract class to define the interface of binary r_matrix_based file readers.
 *
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * r_matrix_based_file_reader, reading r_matrix_based files stored in binary format.
 *
 */
class r_matrix_based_file_reader_bin : public r_matrix_based_file_reader
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
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int  open                            (const char* file_name) = 0;

    /**
     * @brief Default constructor.
     */

                 r_matrix_based_file_reader_bin  (void);

    /**
     * @brief Destructor.
     *
     * Destroys the binary reader.
     */

    virtual      ~r_matrix_based_file_reader_bin (void);

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
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int  read_active_flag                (bool& active) = 0;

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
     *              represented by this error code; for instance, an invalid
     *              or missing closing tag might produce this error when
     *              reading text files; in the case of binary r_matrix files,
     *              this error would state that the record being read is
     *              not complete (the limits of the record have been
     *              reached while reading data).
     *
     * This method must be called right after read_time() has been invoked.
     * Calling this method at any other moment will arise an error condition.
     *
     * A successful execution will provide the values of the correlation values
     * stored in the the_correlation_values output parameter.
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int  read_r_data                     (int&     n_correlation_values_found,
                                                  double*& the_correlation_values) = 0;

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
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int  read_time                       (double& time) = 0;

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
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int  read_type                       (char& record_type) = 0;

};

#endif // R_MATRIX_BASED_FILE_READER_BIN_HPP
