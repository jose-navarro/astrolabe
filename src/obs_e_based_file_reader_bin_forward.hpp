/** \file obs_e_based_file_reader_bin_forward.hpp
  \brief Class defining the interface of forward obs_e_based binary file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_E_BASED_FILE_READER_BIN_FORWARD_HPP
#define OBS_E_BASED_FILE_READER_BIN_FORWARD_HPP

#include "obs_e_based_file_reader_bin.hpp"

/**
 * @brief Class defining the interface of forward obs_e_based binary file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * obs_e_based_file_reader_bin, reading obs_e_based files stored in binary format in
 * forward direction.
 *
 */

class obs_e_based_file_reader_bin_forward : public obs_e_based_file_reader_bin
{
  public:

    /**
     * @brief Default constructor.
     */

                 obs_e_based_file_reader_bin_forward  (void);

    /**
     * @brief Destructor
     */

                 ~obs_e_based_file_reader_bin_forward (void);

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
     *
     * Once a reader has been instantiated (and, optionally, the
     * size of the internal buffer set using set_buffer_size()), it is
     * possible to open the underlying file to enable I/O operations.
     *
     * This method opens the file and instantiates the internal memory
     * buffer used to speed up I/O operations, enabling the different
     * read operations available in this class.
     */

    virtual int  open                                 (const char* file_name);

    /**
     * @brief Reads the active flag for either l- or o-records.
     *
     * @param active Value of the l- or o-record's active flag.
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

    virtual int  read_active_flag                     (bool& active);

    /**
     * @brief Reads the identifier field for either l- or o-records.
     *
     * @param identifier Value of the l- or o-record's identifier. Must be
     *        preallocated and its size must be the one declared
     *        in parameter id_len.
     * @param id_len The size in chars of parameter identifier.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
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
     * If a identifier is found in the input file whose length is greater than
     * the maximum allowed size (id_len), an error code will be returned.
     */

    virtual int  read_identifier                      (char*& identifier,
                                                       int    id_len);

    /**
     * @brief Read the instance identifier in an l-record.
     *
     * @param instance_identifier The instance identifier to read.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: NOT USED.
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

    virtual int  read_instance_id                     (int& instance_identifier);

    /**
     * @brief Read the tags, expectations and covariance
     *        matrix values in an l-record.
     *
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
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: NOT USED.
     *         - 5: NOT USED.
     *         - 6: Invalid dimension for n_tag_values or n_expectation_values.
     *              n_tag_values must be >= 0. n_expectation_values must be > 0.
     *         - 7: The number of tags or expectation values found in the l-record
     *              does not match with those specified by input parameters
     *              n_tag_values or n_expectation_values. The number of covariance
     *              matrix values found is neither 0, n, nor (n * (n + 1)) / 2,
     *              where n stands for the number of expectation values.
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
     */

    virtual int  read_l_data                          (int      n_tag_values,
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
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: NOT USED.
     *         - 5: NOT USED.
     *         - 6: Either the number of parameter or observation instance
     *              identifiers are equal to or less than zero. These values
     *              must be strictly positive. Also, this error code may
     *              be issued if the number of instrument instance identifiers
     *              requested is negative (but it may be zero).
     *          -7: The number of (parameter, observation, instrument) identifiers
     *              found in the o-record does not match with those specified
     *              when calling this method.
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
     * This is an abstract class. This method must be implemented by
     * the descendant classes.
     */

    virtual int  read_o_data                          (int   n_parameter_iids,
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
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
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
     * This is an abstract class. This method must be implemented by
     * the descendant classes.
     */

    virtual int  read_time                            (double& time) = 0;

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
     * event_file_reader for an example on how to check and treat this situation.
     *
     * After a successful completion, the record_type parameter will contain
     * either an 'l' or an 'o' character (always lowercas) denoting the
     * type of record being read (l- and o-records correspondingly).
     *
     * This is an abstract class. This method must be implemented by
     * the descendant classes.
     */

    virtual int  read_type                            (char& record_type) = 0;

  protected:

    /**
     * @brief Moves the bytes still not processed in the data buffer to
     *        the beginning of this buffer.
     * @return The total number of bytes actually moved. May be zero.
     */

    int          move_remaining_up                    (void);

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

    int          read_next_chunk                      (int start_at);

  protected:

    /// @brief Number of bytes actually stored in the data buffer.

    int          data_buffer_bytes_;

    /// @brief Pointer to the first non-processed position (data not yet
    /// "read") in the data buffer.

    int          data_buffer_current_;
};

#endif // OBS_E_BASED_FILE_READER_BIN_FORWARD_HPP
