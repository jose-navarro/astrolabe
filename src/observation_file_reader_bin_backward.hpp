/** \file observation_file_reader_bin_backward.hpp
  \brief Class defining the interface of backwards observation binary file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBSERVATION_FILE_READER_BIN_BACKWARD_HPP
#define OBSERVATION_FILE_READER_BIN_BACKWARD_HPP

#include "obs_e_based_file_reader_bin.hpp"

/**
 * @brief Class defining the interface of backwards observation binary file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * observation_file_reader_bin, reading observation files stored in binary format in
 * backwards direction.
 *
 * The backwards reading process, in fact, does not read the individual
 * l- or o-records in absolute reverse order. In fact, only epochs are
 * read backwards.
 *
 * The process starts with the last epoch included in the input file.
 *
 * Once inside an epoch, all records (either l- or o-) are read in normal
 * forward order. When an epoch is exhausted (that is, no more records
 * belonging to this epoch are available / still unread), this reader
 * jumps to the previous epoch in the file. This is why this class is
 * said to implement a backwards reader.
 *
 * The process finishes when the first epoch in the file has been read
 *(end-of-file or, better put, beginning-of-file condition).
 *
 * Note that an epoch is a set of records (either l- or o-) that have
 * exactly the same time tag.
 *
 */

class observation_file_reader_bin_backward : public obs_e_based_file_reader_bin
{
  public:

    /**
     * @brief Default constructor.
     */

                 observation_file_reader_bin_backward  (void);

    /**
     * @brief Destructor.
     */

    virtual      ~observation_file_reader_bin_backward (void);

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

    virtual int  open                                  (const char*  file_name);

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

    virtual int  read_active_flag                      (bool&       active);

    /**
     * @brief Reads the identifier field for either l- or o-records.
     *
     * @param identifier Value of the l- or o-record's identifier. Must be
     *        preallocated and its size must be the one declared
     *        in id_len The size in chars of parameter identifier.
     * @param id_len The actual length (size) of parameter identifier.
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

    virtual int  read_identifier                       (char*&       identifier,
                                                        int          id_len);

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
     *         - 4: NOT USED.
     *
     * This method may be used only when an l-record is being read. Otherwise,
     * an error condition will arise.
     *
     * It must be called right after read_identifier() has been invoked. Calling
     * this method at any other time will arise an error condition.
     *
     * After a successful completion, input parameter instance_identifier will contain the
     * l-record instance identifier.
     */

    virtual int  read_instance_id                      (int&         instance_identifier);

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
     *         - 1: NOT USED.
     *         - 2: NOT USED.
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

    virtual int  read_l_data                           (int      n_tag_values,
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
     */

    virtual int  read_o_data                           (int   n_parameter_iids,
                                                        int*& the_parameter_iids,
                                                        int   n_observation_iids,
                                                        int*& the_observation_iids,
                                                        int   n_instrument_iids,
                                                        int*& the_instrument_iids);

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
     *         - 4: NOT USED.
     *         - 5: An epoch change has been detected when reading
     *              the time. The record being read, which is the
     *              first of the new epoch, is an o-record, which
     *              is illegal.
     *
     * This method may be used when reading either l- or o-records.
     *
     * When reading an l-record, it must be called right after a call
     * to read_instance_id() has been made; on the contrary, if an o-record
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
     */

    virtual int  read_time                             (double&      time);

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
     *         - 5: NOT USED.
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

    virtual int  read_type                             (char& record_type);


  protected:

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
    int          read_prev_chunk                       (void);

    /**
     * @brief Check if the current epoch still has data to process (read). If
     *        not, the pointers to the previous epoch are computed, loading
     *        data from the input file if data in the internal data buffer
     *        has been exhausted.
     *
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Beginning of file (no more data).
     *        - 2: I/O error while reading (might happen if the file
     *             has not been opened).
     *
     * Each time an l- or o-record needs to be read, this method checks if
     * the current epoch still holds data to be processed.
     *
     * If such is the situation, this method simply returns.
     *
     * On the contrary, when the read process reaches a point where the
     * current epoch (set of l- or o-records with the same time tag) has been
     * exhausted, it is necessary to compute the limits (inside the internal
     * data buffer) where the previous (this class is a backwards reader) epoch
     * data is stored.
     *
     * Due to the buffered nature of this reader, the information related
     * to the previous epoch may be already stored in the data buffer. If so,
     * the method just computes the new epoch limits and returns.
     *
     * On the contrary, if the data buffer has been exhausted, it will be
     * necessary to read more data from the input file (see read_prev_chunk())
     * and load it in the internal data buffer. Once done, the limits of
     * the previous epoch may be computed. Note that actually reading the
     * input file may produce I/O errors. See the list or error codes for
     * more information.
     *
     * The information about epoch limits is stored in members
     * - epoch_cur_backtrack_index_
     * - epoch_cur_offset_
     * - epoch_remaining_bytes_
     */

    int          update_epoch_pointers                 (void);


  protected:

    /// @brief Index (in the data buffer) where the backtracking information
    /// for the current epoch is stored.  An epoch is a set of event
    /// records (either l- or o-) that have the same time tag.

    int          epoch_cur_backtrack_index_;

    /// @brief Index (in the data buffer) of the next position to read when
    /// processing the current epoch. An epoch is a set of event
    /// records (either l- or o-) that have the same time tag.

    int          epoch_cur_offset_;

    /// @brief Number of bytes still to be processed in the current epoch. An
    /// epoch is a set of event records (either l- or o-) that have the same
    /// time tag.

    int          epoch_remaining_bytes_;

    /// @brief Position in the data buffer where actual data starts (due to
    /// the backwards nature of the read process, the absolute beginning of
    /// the data buffer may not store actual data but just garbage).

    int          chunk_start_index_;

    /// @brief Number of bytes in the input file that have not been read yet.

    int          file_bytes_unread_;
};

#endif // OBSERVATION_FILE_READER_BIN_BACKWARD_HPP
