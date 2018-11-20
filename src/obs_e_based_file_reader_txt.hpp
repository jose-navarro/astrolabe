/** \file obs_e_based_file_reader_txt.hpp
  \brief  Abstract class to define the interface of obs_e_based text file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_E_BASED_FILE_READER_TXT_HPP
#define OBS_E_BASED_FILE_READER_TXT_HPP

#include <string>
#include <map>

#include "obs_e_based_file_reader.hpp"

/**
 * @brief Abstract class to define the interface of obs_e_based text file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * obs_e_based_file_reader, reading obs_e_based files stored in text format.
 *
 */

class obs_e_based_file_reader_txt : public obs_e_based_file_reader
{
  public:

    /**
     * @brief Default constructor.
     */
                 obs_e_based_file_reader_txt  (void);

    /**
     * @brief Destructor.
     *
     * Destroys the text reader.
     */
    virtual      ~obs_e_based_file_reader_txt (void);

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
    virtual int  open                         (const char* file_name) = 0;

    /**
     * @brief Reads the active flag for either l- or o-records.
     *
     * @param active Value of the l- or o-record's active flag.
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

    virtual int  read_active_flag             (bool& active);

    /**
     * @brief Reads the identifier field for either l- or o-records.
     *
     * @param identifier Value of the l- or o-record's identifier. Must be
     *        preallocated and its size must be the one declared
     *        in parameter id_len.
     * @param id_len The size in chars of parameter identifier.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: NOT USED.
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: The identifier found in the input file is longer than
     *              the maximum dimension allowed (parameter id_len);
     *
     * Once the active flag of the record has been obtained using
     * read_active_flag(), it is possible to read its identifier. This is the task
     * of this method.
     *
     * Note that read_identifier() must be called right after read_active_flag() has
     * has been invoked and before any other read_* method is used; otherwise,
     * an error condition will arise.
     *
     * Parameter identifier must have been preallocated, and its actual size
     * must conform to the one stated by parameter id_len. If these
     * conditions are not met, this method may crash.
     *
     * If an identifier is found in the input file whose length is greater than
     * the maximum allowed size (id_len), an error code will be returned.
     */

    virtual int  read_identifier              (char*& identifier,
                                               int    id_len);

    /**
     * @brief Read the instance identifier in an l-record.
     *
     * @param instance_identifier The instance identifier to read.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: NOT USED.
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. The value corresponding to the
     *              identifier read from the input file is not a valid
     *              integer.
     *
     * This method may be used only when an l-record is being read. Otherwise,
     * an error condition will arise.
     *
     * It must be called right after read_identifier() has been invoked. Calling
     * this method at any other time will arise an error condition.
     *
     * After a successful completion, input parameter instance identifier will contain the
     * l-record instance identifier.
     */

    virtual int  read_instance_id             (int& instance_identifier);

    /**
     * @brief Read the tags, expectations and covariance
     *        matrix values in an l-record.
     *
     * @param n_tag_values Number of tag values that must be read from
     *        the l-record. May be zero, since tags are optional.
     * @param the_tags Array that will store the values of the n_tag_values
     *        tags recovered from the l-record. Must be big enough to hold
     *        these values.
     * @param n_expectation_values Number of expectation values that must be
     *        read from the l-record. Must be greater than 0, since expectations
     *        are mandatory.
     * @param the_expectations Array that will store the values of the
     *        n_expectation_values expectations recovered from the l-record.
     *        Must be big enough to hold these values.
     * @param n_covariance_values_found Actual number of covariance matrix
     *        values found in the l-record. The calling module must NOT set
     *        this parameter, but read it once the method is over. This is
     *        so because the number of covariance matrix values is variable.
     *        it may be (a) zero, because no covariance matrix is given at
     *        all for the l-record, being used a default one, (b) as many
     *        values as the number of expectations (that is, only the standard
     *        deviation values are provided) or (c) assuming than n is the
     *        number of expectation values, (n * (n + 1))/2, that is, both
     *        standard deviations and correlation coefficients are retrieved.
     * @param the_covariance_values Array that will store the covariance matrix
     *        values. It must be able to hold as many values as returned in the
     *        worst case (standard deviations plus correlation coefficients).
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: NOT USED.
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. At least one of the values corresponding to
     *              expectations, standard deviations or auxiliars is no a
     *              valid double value.
     *         - 5: Invalid l-record. Several conditions may arise that are
     *              represented by this error code; for instance:
     *            -   Extra information found between the last valid value and
     *                the end of the l-record. For instance, extra characters
     *                after a complete set of values would produce this error.
     *            -   Premature end of record. The end of record is found before
     *                all the information is read. For instance, an incorrect
     *                number of covariance matrix values (not 0, not n, not (n*(n+1))/2)
     *                would produce this error.
     *         - 6: Invalid dimension for n_tag_values or n_expectation_values.
     *              n_tag_values must be >= 0. n_expectation_values must be > 0.
     *         - 7: NOT USED.
     *
     * This method may be used only when an l-record is being read. Otherwise,
     * an error condition will arise.
     *
     * It must be called right after read_time() has been called. Calling this
     * method at any other moment will arise an error condition.
     *
     * A successful execution will provide the values of the tags (if any),
     * expectations and covariance matrix coefficients (which my be empty) stored
     * in the corresponding data buffers. The number of covariance matrix values
     * found is also returned.
     *
     */

    virtual int  read_l_data                  (int      n_tag_values,
                                               double*& the_tags,
                                               int      n_expectation_values,
                                               double*& the_expectations,
                                               int&     n_covariance_values_found,
                                               double*& the_covariance_values);


    /**
     * @brief Read the lists of parameter, observation and instrument instance
     *        identifiers from an o-record.
     *
     * @param n_parameter_iids Total number of parameter instance identifiers
     *        to read. Must be greater than 0.
     * @param the_parameter_iids Buffer that will store the parameter instance
     *        identifiers read.
     * @param n_observation_iids Total number of observation instance identifiers
     *        to read. Must be greater than 0.
     * @param the_observation_iids Buffer that will store the observation instance
     *        identifiers read.
     * @param n_instrument_iids Total number of instrument instance identifiers
     *        to read. May be zero.
     * @param the_instrument_iids Buffer that will store the instrument instance
     *        identifiers read. 
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: NOT USED.
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. At least one of the values corresponding to
     *              identifiers of the o-record are not valid integers.
     *         - 5: Invalid end of the o-record. Other characters besides
     *              whitespace or a legal closing o-record tag have been
     *              found.
     *         - 6: Either the number of parameter or observation instance
     *              identifiers are equal to or less than zero. These values
     *              must be strictly positive. Also, this error code may
     *              be issued if the number of instrument instance identifiers
     *              requested is negative (but it may be zero).
     *          -7: NOT USED.
     *
     * This method may be used only when an o-record is being read. Otherwise,
     * an error condition will arise.
     *
     * It must be called right after read_time() has been called. Calling this
     * method at any other moment will arise an error condition.
     *
     * A successful execution will set the different instance identifiers
     * arrays with the values read from the o-record.
     *
     * All data arrays (the_parameter_iids, the_observation_iids and
     * the_instrument_iids) must be preinstantiated and its size must be
     * big enough as to hold the requested number of elements as requested
     * in the corresponding dimensioning parameters (n_parameter_idds,
     * n_observation_iids and n_instrument_iids). No tests are made to
     * check these dimensions, since int* values provide no mechanisms
     * to do it. Therefore, it is the responsibility of the calling
     * method to guarantee that these conditions are met.
     *
     * Note that even when the set of identifiers has been correctly parsed,
     * still an additional error may arise: the remaining characters in the
     * record after these integer values may be invalid. After these values,
     * only whitespace and a legal o-record closing tag may be found. If this
     * is not the case, an error condition is arised via return code 5.
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     *
     */

    virtual int  read_o_data                  (int   n_parameter_iids,
                                               int*& the_parameter_iids,
                                               int   n_observation_iids,
                                               int*& the_observation_iids,
                                               int   n_instrument_iids,
                                               int*& the_instrument_iids) = 0;

    /**
     * @brief Read the time tag, either for l- or o-records.
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
     *         - 5: An epoch change has been detected when reading
     *              the time. The record being read, which is the
     *              first of the new epoch, is an o-record, which
     *              is illegal.
     *
     * This method may be used when reading either l- or o-records.
     *
     * When reading an l-record, it must be called right after a call
     * to read_l_id() has been made; on the contrary, if an o-record
     * is being read, it must be called just after a call to read_identifier().
     * Using this method in a different moment will arise an error
     * condition.
     *
     * Once run, time will contain the value of the time tag for the
     * record being read (either l- or o-record).
     *
     * This method may detect the illegal situation produced when
     * the first record in an epoch is an o-record instead of an l one.
     * This is so because it is not possible to detect such anomalous
     * situations until the time is read and new epochs may be
     * therefore detected.
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     *
     */

    virtual int  read_time                    (double& time) = 0;

    /**
     * @brief Read the event record type.
     *
     * @param record_type The type of the record being read, either an 'l' or
     *        an 'o' (both lowercase).
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
     * event_file_reader for an example on how to check and treat this
     * situation.
     *
     * After a successful completion, the record_type parameter will contain
     * either an 'l' or an 'o' character (always lowercas) denoting the
     * type of record being read (l- and o-records correspondingly).
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int  read_type                    (char& record_type) = 0;

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
     *        l record is made just of whitespace plus a valid closing
     *        l-record tag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: Invalid characters / closing l-tag found.
     *
     * Once all data (type, identifier, instance identifier, time, expetactions, standard
     * deviations and tags) have been completely parsed, the remaining
     * characters in the current record are scanned to check that only
     * whitespace plus a valid closing l-record tag is present.
     *
     * The idea is to ensure that the l-record is correct and that neither
     * extra (invalid) data nor an incorrect closing l-tag is included in an
     * up-to-the-moment legitimate l-record.
     *
     * Note that this method updates member cur_record_cur_ while scanning
     * the data buffer. It relies on parse_closing_tag().
     */

    int          parse_closing_l_tag          (void);

    /**
     * @brief Checks that the character after the last valid data in an
     *        o record is made just of whitespace plus a valid closing
     *        o-record tag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: Invalid characters / closing o-tag found.
     *
     * Once all data (type, identifier, time and lists of identifiers)
     * have been completely parsed, the remaining characters in the current
     * record are scanned to check that only whitespace plus a valid closing
     * o-record tag is present.
     *
     * The idea is to ensure that the o-record is correct and that neither
     * extra (invalid) data nor an incorrect closing o-tag is included in an
     * up-to-the-moment legitimate o-record.
     *
     * Note that this method updates member cur_record_cur_ while scanning
     * the data buffer. It relies on parse_closing_tag()
     */

    int          parse_closing_o_tag          (void);

    /**
     * @brief Checks that the character after the last valid data in either
     *        an l- or o-record is made just of whitespace plus a valid
     *        closing tag.
     * @param lctag Closing tag to check, lowercase version.
     * @param uctag Closing tag to check, uppercase version.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: Invalid characters / closing tag found.
     *
     * This is the base function used by both parse_closing_l_tag() and
     * parse_closing_o_tag(). The idea is to reduce the amount of duplicated
     * code needed to implement these functions.
     *
     * The specific closing tags to check are parameters lctag and uctag,
     * which correspond to the lowercase and uppercase versions (respectively)
     * of the aforementioned tag.
     *
     * For instance, to check a closing l-tag, this method must be called
     * setting lctag to "</l>" and uctag to "</L>".
     */

    int          parse_closing_tag            (char* lctag, char *uctag);

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

    /**
     * @brief Sets the new value of the last record identifier taking care of
              redimensioning its size if the new value is too big.
       @param identifier The new identifier to assign the the last record identifier

       last_record_identifier_ is used to store the different identifiers loaded from
       the input files. These identifiers are variable length strings.

       To avoid the need to delete and instantiate last_record_identifier_
       each time a new record is read, this member is instantiated at first
       with a given size. When a new value has to be set and the length
       of such value is smaller than the actual dimension of last_record_identifier_,
       this member is simply reused. Only when the new identifier is bigger
       than the available space allocated to last_record_identifier_ is this
       member destroyed and reinstantiated.

       This reduces tha amount of destroy / create operations needed, thus
       improving speed.	

    */

    void         set_last_record_identifier   (const char* identifier);

  protected:

    /// @brief List of attributes included in the opening tag of the
    /// current l- or o-record.

    map<string, string> cur_record_attributes_;

    /// @brief Current position in the data buffer when processing a full
    /// l- or o-record.

    int                 cur_record_cur_;

    /// @brief Last position in the data buffer for the current l- or
    /// o-record.

    int                 cur_record_end_;

    /// @brief Start position in the data buffer for the current l- or
    /// o-record.

    int                 cur_record_start_;

    /// @brief Identifier for the last record read.

    char*               last_record_identifier_;

    /// @brief Size in chars of the last_record_identifier_ read.

    int                 last_record_identifier_size_;

    /// @brief Instance identifier for the last full record read, valid when such record
    /// is of type l.

    int                 last_record_instance_id_;

    /// @brief Internal character buffer used in some parsing operations.

    char*               text_buffer_;

    /// @brief Current size of the internal character buffer used in some
    /// parsing operations.

    int                 text_buffer_size_;
};

#endif // OBS_E_BASED_FILE_READER_TXT_HPP
