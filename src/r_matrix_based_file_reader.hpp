/** \file r_matrix_based_file_reader.hpp
  \brief Abstract class to define the interface of several ASTROLABE
         r_matrix based file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_BASE_FILE_READER_HPP
#define R_MATRIX_BASE_FILE_READER_HPP

#include <cstring>
#include <cstdio>

using namespace std;

/// @brief Default buffer size: 5 Mbyte (5 * 1024 * 1024 bytes)

#define _ASTROLABE_R_MATRIX_FILE_READER_DEFAULT_BUFFER_SIZE 5242880

/**
 * @brief Abstract class to define the interface of several ASTROLABE
 *        r_matrix based file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * All reader classes derived from this abstract class MUST implement
 * a buffered I/O model, since the goal of this library is to reduce
 * the amount of disk operations
 *
 * Additionally, all descendant reader classes must adhere to the same
 * interface described in this abstract class, no matter how specialized these
 * might be.
 *
 * The overall usage philosophy is described below:
 *
 * - A reader object must be instantiated using the appropriate, specialised
 *   constructor.
 * - Optionally, the size of the internal buffer used to reduce disk I/O
 *   may be set using method set_buffer_size(). Note that all descendant
 *   classes must provide with a default buffer size that would be used
 *   if a calling module makes no decisions on this subject.
 * - The reader must be associated to a physical output file using
 *   method open(). This action enables actual I/O operations.
 * - The actual read process may start now. The actual read action is
 *   split in several methods that must be used as stated below:
 *   -# read_type(). Retrieves the type of the record to come. In r_matrix
 *    based files, all records must be of type "r".
 *   -# read_active_flag(). To ascertain whether the record is active or not.
 *   -# Call read_time(), to retrieve the time tag in the r-record being read.
 *   -# Finally, call read_r_data() to retrieve the specific information for these
 *    records. This information is a correlation matrix.
 * - Note that an end-of-file condition (or beginning-of-file in the case of
 *   backwards readers) may be issued during the previous process. This
 *   condition is reported ONLY by method read_type() as a specific error
 *   code. Therefore, it is necessary to check the error code returned by
 *   read_type() in order to ascertain if more data is still available. Any
 *   other errors issued by either read_type() or any other reading method
 *   must be treated according to their specific nature.
 * - The reader must close the associated input file; method close() i
 *   provided to fullfil such purpose.
 *   Closing the associated file will disable any further I/O read operations.
 * - The reader object may be destroyed. Destructors must guarantee that
 *   close() (see previous point) has been called. If not, the destructor
 *   will call close() by itself, in an automatic, transparent way.
 *
 * Note that when it comes to the detection of the "end of file" condition, it
 * could be argued that backward readers will never reach such a situation but
 * that a beginning (not the end) of file condition will show up instead.
 * This is true, but to provide an uniform interface among the different
 * readers in this package, an unique method, namely is_eof() is used for
 * such purposes. Therefore, the interpretation of is_eof() must be "is anything
 * left in the file to read?" which is valid for both forward and backward
 * readers.
 *
 */

class r_matrix_based_file_reader
{

  public:

    /**
     * @brief Default constructor.
     *
     * Since this is an abstract class, it can not be instantiated.
     * Use the specific constructors and destructors of descendant classes
     * to instantiate or destroy the corresponding specialized readers.
     */

                  r_matrix_based_file_reader  (void);

    /**
     * @brief Destructor.
     *
     * Since this is an abstract class, it can not be instantiated.
     * Use the specific constructors and destructors of descendant classes
     * to instantiate or destroy the corresponding specialized readers.
     */

    virtual       ~r_matrix_based_file_reader (void);

    /**
     * @brief Returns the current size of the buffer internally
     *        used by the reader.
     * @return Current size in bytes of the internal reader's buffer.
     *
     * All readers use an internal buffer to reduce the amount
     * of I/O operations. The size of this buffer may be, optionally,
     * set using method set_buffer_size().
     *
     * This method return the size of the buffer at the moment it is called;
     * if no calls to set_buffer_size() are made, it will be equivalent
     * to _default_buffer_size(). On the contrary, the result will vary
     * if set_buffer_size() is invoked. In this case, the size set by
     * the user will be returned.
     */

    virtual int   _buffer_size              (void) const;

    /**
     * @brief Closes the file reader, disabling any other
     *        further action on it.
     * @return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the file.
     */

    virtual int   close                     (void);

    /**
     * @brief Returns the default size of the buffer internally
     *        used by the reader.
     * @return Default size in bytes of the internal reader's buffer.
     *
     * All readers use an internal buffer to reduce the amount
     * of I/O operations. The size of this buffer may be, optionally,
     * set using method set_buffer_size().
     *
     * This method return this size in bytes.
     *
     * Note that when no size is set, readers use a default buffer size (either
     * the default one or the one set by the calling module using
     * set_buffer_size())
     */

    virtual int   _default_buffer_size      (void) const;

    /**
     * @brief Returns a copy of the name of the file the readers are
     *        dealing with.
     * @return The name of the file when the file has been opened, a
     *        NULL value otherwise.
     */

    virtual char* _file_name                (void) const;

    /**
     * @brief Check if the data in the file has been completely read.
     *
     * @return True if all data in the file has already been read, false
     *        otherwise.
     *
     * Use this method to check if the contents of the file has been
     * exhausted (completely read) or not. In other words, check if
     * the "end of the file" has been reached.
     *
     * Strictly speaking, backwards readers will never reach the end
     * of the file but its beginning, since the read process traverses
     * the file from bottom to top.
     *
     * However, the concept of data exhaustion is valid in both cases
     * (forward and reverse readers) and this is the method to use
     * in all cases to check if there is anything left to read. That is
     * why an unique method has been provided for this purpose (instead of
     * adding an hypothetical is_bof() or "is beginning of file" to be used
     * only with reverse readers. In this way, the interface for any
     * reader is kept uniform.
     */

    virtual bool  is_eof                    (void) const;

    /**
     * @brief Check if the underlying data file has already been opened.
     *
     * @return True if the file has already been opened, false otherwise.
     */

    virtual bool  is_open                   (void) const;

    /**
     * @brief Opens the reader, enabling I/O operations.
     *
     * @param file_name Name of the file to open for reading.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The file was already open.
     *        - 2: Unable to open the selected file.
     *        - 3: Not enough memory to allocate the internal buffer.
     *        - 4: End of file condition detected - the file is empty.
     *        - 5: I/O error detected while reading the file.
     *        - 6: Malformed records found.
     *        - 7: Internal buffer too small
     *
     * Once a reader has been instantiated (and, optionally, the
     * size of the internal buffer set using set_buffer_size()), it is
     * possible to open the underlying file to enable I/O operations.
     *
     * This method opens the file and instantiates the internal memory
     * buffer used to speed up I/O operations, enabling the different
     * read operations available.
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int   open                      (const char* file_name) = 0;

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

    virtual int   read_active_flag          (bool& active) = 0;

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
     *         - 4: Illegal value. At least one of the correlation values
     *              is not a valid double value.
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

    virtual int   read_r_data               (int&     n_correlation_values_found,
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
     *         - 4: Illegal value. The value corresponding to the
     *              time read from the input file is not a valid
     *              double.
     *
     * It must be called right after a call to read_active_flag() has
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

    virtual int   read_time                 (double& time) = 0;

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
     * either an 'r' (always lowercas) denoting the type or record
     * being read (always an r-record).
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int   read_type                 (char& record_type) = 0;

    /**
     * @brief Set the size of the internal buffer used to reduce I/O.
     *
     * @param buffer_size The new size in bytes of the internal buffer.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The file is already open and therefore it is not
     *             possible to change its buffer size.
     *        - 2: Invalid buffer size. Must be greater than zero.
     *
     * All reader classes must provide with an internal buffer to
     * reduce the amount of I/O operations and therefore improve speed.
     * The size of this buffer is set by default by all reader classes if
     * no further actions are taken by calling modules.
     *
     * This method provides with a mechanism to set a user-defined
     * buffer size.
     *
     * This buffer size must be strictly greater than zero and may never
     * be greater than maximum value that may be stored in a signed
     * integer variable. However, reasonable buffer sizes must be used
     * to avoid triggering the operating built-in paging mechanisms. For
     * instance, it is reasonable to think of a buffer size of about
     * 50Mb. It would be completely unrealistic to use a buffer size of
     * 1GB, because, most probably, such an amount of memory would not
     * fit in the computer's memory without triggering the paging
     * mechanism, which, in turn, would increase I/O.
     *
     * Moreover, the efficiency of the library will not increase once the
     * size of the internal buffer goes beyond some values. The difference
     * between a buffer of 20Mb and another of 50Mb becomes unnoticeable.
     * Therefore, it makes no sense to use buffer sizes beyond these
     * magnitudes.
     *
     * On the other side, buffer sizes ridiculously small will reduce the
     * performance of the library or even block it. For instance, a
     * buffer size that is smaller than the size of the single largest record
     * in the file will produce a malfunction, since there will be not enough
     * memory to store it. Obviously, the smaller the size of the internal
     * buffer, the more I/O operations will take place, thus reducing
     * performance. It is not recommended to use buffer sizes below 5Kb.
     *
     * This method may be called only after the reader object has been
     * instantiated and before open() has been invoked. Calling
     * set_buffer_size() at any other moment will arise an error
     * condition.
     */

    virtual int   set_buffer_size           (int buffer_size);

  protected:

    /// @brief The status returned by backtrack_to_previous_record. It must
    /// be store for some time before it may be used.

    int           backtrack_status_;

    /// @brief The internal buffer used to store the data read.

    char*         data_buffer_;

    /// @brief Actual size in bytes of data_buffer_.

    int           data_buffer_size_;

    /// @brief The file handled by the reader.

    FILE*         file_file_;

    /// @brief The name of the underlying file being dealt with.

    char*         file_name_;

    /// @brief Flag. When true, the underlying file has been opened.

    bool          file_is_open_;

    /// @brief Actual size of the file read (in bytes).

    long          file_size_;

    /// @brief Flag. When true, the file has been completely read, so no
    /// more data is available.

    bool          is_eof_;

    /// @brief Active / removed flag for the last record (either l- or o-) read.

    bool          last_record_is_active_;

    /// @brief Control flag. When true, it indicates that a call to read_active_flag()
    /// has already been made for the current record.

    bool          read_active_flag_;

    /// @brief Control flag. When true, it indicates that a full record has
    /// been read. In other words, a full sequence of calls to the different
    /// read_* calls has been made.

    bool          read_completed_;

    /// @brief Control flag. When true, it indicates that a call to read_r_data()
    /// has already been made for the current record.

    bool          read_r_data_;

    /// @brief Control flag. When true, it indicates that a call to read_time()
    /// has already been made for the current record.

    bool          read_time_;

    /// @brief Control flag. When true, it indicates that a call to read_type()
    /// has already been made for the current record.

    bool          read_type_;

};

#endif // R_MATRIX_BASE_FILE_READER_HPP
