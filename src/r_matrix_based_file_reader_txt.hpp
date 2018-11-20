/** \file r_matrix_based_file_reader_txt.hpp
  \brief  Abstract class to define the interface of r_matrix based text file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_BASED_FILE_READER_TXT_HPP
#define R_MATRIX_BASED_FILE_READER_TXT_HPP

#include <string>
#include <map>

#include "r_matrix_based_file_reader.hpp"

/**
 * @brief Abstract class to define the interface of r_matrix based text file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * r_matrix_based_file_reader, reading r_matrix based files stored in text format.
 *
 */

class r_matrix_based_file_reader_txt : public r_matrix_based_file_reader
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
     *        - 7: Internal buffer too small
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
                 r_matrix_based_file_reader_txt  (void);

    /**
     * @brief Destructor.
     *
     * Destroys the text reader.
     */
    virtual      ~r_matrix_based_file_reader_txt (void);

    /**
     * @brief Reads the active flag.
     *
     * @param active Value of the r-record's active flag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: NOT USED.
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

    virtual int  read_active_flag                (bool& active);

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
     *              or missing closing tag might produce this error.
     *
     * This method must be called right after read_time() has been invoked.
     * Calling this method at any other moment will arise an error condition.
     *
     * A successful execution will provide the values of the correlation values
     * stored in the the_correlation_values output parameter.
     */

    virtual int   read_r_data                    (int&     n_correlation_values_found,
                                                  double*& the_correlation_values);

    /**
     * @brief Read the time tag.
     *
     * @param time The time tag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: NOT USED.
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. The value corresponding to the
     *              time read from the input file is not a valid
     *              double.
     *
     * This method must be called right after a call to read_active_flag() has
     * been made.  Using this method in a different moment will arise an
     * error condition.
     *
     * Once run, time will contain the value of the time tag for the
     * record being read.
     *
     */

    virtual int  read_time                             (double& time);

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

    virtual int   read_type                      (char& record_type) = 0;

  protected:

    /**
     * @brief Check the current size of text_buffer_ against the required
     *        size and, if not big enough, make it grow to guarantee the
     *        required capacity.
     * @param size Size (in number of characters) to guarantee.
     */

    void         guarantee_text_buffer_size   (int size);

    /**
     * @brief Checks if a given character is considered as whitespace by
     *        this class.
     * @param chr The character to check.
     * @return True if the character received as a parameter is considered
     *         as whitespace, false otherwise.
     *
     * This method recognizes as whitespace the following characters:
     *
     * - Blank
     * - Tab
     * - Carriage return
     * - Line feed
     *
     * The previous list should be updated if the characters recognized as
     * whitespace are changed.
     */

    bool         is_whitespace                (char chr);

    /**
     * @brief Parse de different attributes in an XML tag.
     * @param buffer Text to parse.
       @param parsefrom Index in buffer where the search process must start
              from (included).
       @param parseto   Index in buffer where the search process must stop
              at (included).
       @param attributes A map structure where the attribute names and
              values are stored. May be empty if no attributes are found.
              Set only when the parsing process is correct.
              Flushed each time this method is called.
       @return Error code. Error code values:
               - 0 : Successful completion.
               - 1 : Invalid parsing.
    */

    int          parse_attributes             (char* buffer, int parsefrom, int parseto,
                                               map<string,string>& attributes);

    /**
     * @brief Checks that the character after the last valid data in an
     *        r record is made just of whitespace plus a valid closing
     *        r-record tag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: Invalid characters / closing r-tag found.
     *
     * Once all data (type, time and correlation values) have been completely
     * parsed, the remaining characters in the current record are scanned to
     * check that only whitespace plus a valid closing r-record tag is present.
     *
     * The idea is to ensure that the r-record is correct and that neither
     * extra (invalid) data nor an incorrect closing r-tag is included in an
     * up-to-the-moment legitimate o-record.
     *
     * Note that this method updates member cur_record_cur_ while scanning
     * the data buffer. It relies on parse_closing_tag()
     */

    int          parse_closing_r_tag             (void);

    /**
     * @brief Parses the data buffer trying to convert the first
     *        non-whitespace characters found as an double value.
     * @param dvalue The binary double value corresponding to the parsed
     *        text in the data buffer.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: The first non-whitespace characters in the data buffer
     *              may not be interpreted as a double value.
     *
     * This methods starts scanning the data buffer at position cur_record_start_.
     * First, it skips any whitespace found. Then, the next set of contiguous
     * non-whitespace characters are read, trying to interpret these as
     * a text representation of double value.
     *
     * Note that the scan process stops as soon as a whitespace character
     * following the countiguous set of non-whitespace characters is found.
     *
     * Note also that this method updates the value of cur_record_start_. After
     * a successful completion, it points to the first whitespace character
     * found after the double sought.
     *
     * If the conversion for character to double succeeds, the result of
     * such conversion is stored in parameter dvalue and the function
     * returns a 0 (successful completion). On the contrary, 1 is returned
     * if such conversion is not possible, meaning that the string
     * processed does not contain a valid text representation of a
     * double value.
     *
     */

    int          parse_double_value           (double& dvalue);

    /**
     * @brief Parses the data buffer trying to convert the first
     *        non-whitespace characters found as an integer value.
     * @param ivalue The binary integer value corresponding to the parsed
     *        text in the data buffer.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: The first non-whitespace characters in the data buffer
     *              may not be interpreted as an integer value.
     *
     * This methods starts scanning the data buffer at position cur_record_start_.
     * First, it skips any whitespace found. Then, the next set of contiguous
     * non-whitespace characters are read, trying to interpret these as
     * a text representation of an integer value.
     *
     * Note that the scan process stops as soon as a whitespace character
     * following the countiguous set of non-whitespace characters is found.
     *
     * Note also that this method updates the value of cur_record_start_. After
     * a successful completion, it points to the first whitespace character
     * found after the integer sought.
     *
     * If the conversion for character to integer succeeds, the result of
     * such conversion is stored in parameter ivalue and the function
     * returns a 0 (successful completion). On the contrary, 1 is returned
     * if such conversion is not possible, meaning that the string
     * processed does not contain a valid text representation of an
     * integer value.
     *
     */

    int          parse_int_value              (int& ivalue);

  protected:

    /// @brief List of attributes included in the opening tag of the
    /// current r-record.

    map<string, string> cur_record_attributes_;

    /// @brief Current position in the data buffer when processing a full
    /// r-record.

    int                 cur_record_cur_;

    /// @brief Last position in the data buffer for the current r-record.

    int                 cur_record_end_;

    /// @brief Start position in the data buffer for the current r-record.

    int                 cur_record_start_;

    /// @brief Internal character buffer used in some parsing operations.

    char*               text_buffer_;

    /// @brief Current size of the internal character buffer used in some
    /// parsing operations.

    int                 text_buffer_size_;
};

#endif // R_MATRIX_BASED_FILE_READER_TXT_HPP
