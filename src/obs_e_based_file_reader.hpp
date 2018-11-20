/** \file obs_e_based_file_reader.hpp
  \brief Abstract class to define the interface of several ASTROLABE
         obs_e based file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_E_BASE_FILE_READER_HPP
#define OBS_E_BASE_FILE_READER_HPP

#include <cstring>
#include <cstdio>

using namespace std;

/// @brief Default buffer size: 5 Mbyte (5 * 1024 * 1024 bytes)

#define _ASTROLABE_OBS_E_FILE_READER_DEFAULT_BUFFER_SIZE 5242880

/**
 * @brief Abstract class to define the interface of several ASTROLABE
 *        obs_e based file readers.
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
 * - The actual read process may start now. Since two different types of
 *   records may be stored in the file being read (l- and o-records) and
 *   the type of record to come next is unknown, the actual read action is
 *   split in several methods that must be used as stated below:
 *   -# read_type(). Retrieves the type of the record to come, that is, either
 *     an l- or o-record.
 *   -# read_active_flag(). To ascertain whether the l- or o-record is active or not.
 *   -# read_identifier(). No matter the type returned by the previous call to
 *    read_type(), this method must be used to retrieve the identifier associated
 *    to the record being read.
 *   -# Call read_instance_id() ONLY if an l-record has been found; retrieves
 *    the l-record identifier.
 *   -# Call read_time(), no matter the type of record being read, to retrieve
 *    the time tag in the l- or o-record being read.
 *   -# Finally, call read_l_data() or read_o_data() depending on the type
 *    of record being read to retrieve the specific information for these
 *    two records. In the case of l-records, this information is the set
 *    of expectations, standard deviations and auxiliars; when reading an
 *    o-record, the information contains the set of identifiers used.
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

class obs_e_based_file_reader
{

  public:

    /**
     * @brief Default constructor.
     *
     * Since this is an abstract class, it can not be instantiated.
     * Use the specific constructors and destructors of descendant classes
     * to instantiate or destroy the corresponding specialized readers.
     */

                  obs_e_based_file_reader     (void);

    /**
     * @brief Destructor.
     *
     * Since this is an abstract class, it can not be instantiated.
     * Use the specific constructors and destructors of descendant classes
     * to instantiate or destroy the corresponding specialized readers.
     */

    virtual       ~obs_e_based_file_reader    (void);

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
     * @brief Epoch change detection.
     *
     * @return True if the time tag of the record being read differs from that
     *         in the previous fully read record, or, in other words, when
     *         the record being read belongs to a new epoch.
     *
     * All records in an ASTROLABE obs_e file are ordered by time. All those sharing
     * the same time tag constitute an epoch.
     *
     * When reading successive records in an ASTROLABE obs_e file, soon or later
     * the time tag read will change. At this moment, an epoch change
     * has just happened.
     *
     * Epoch changes ARE DETECTED JUST AFTER read_time() is successfully
     * invoked, but not before. Therefore, to obtain valid results,
     * epoch_changed() must be called only after read_time()
     * has been invoked and before read_type() (the method that triggers
     * the read process for a full new record) is called again.
     *
     * When this method returns true, it means that the record being read
     * belongs to a new epoch, that is, its time tag differs from those
     * found in the records read previously.
     *
     * Of course, calling modules may ascertain this time change by themselves, just
     * tracking the time tag obtained each time a full record (either l- or o-)
     * is completely read. However, this method is provided for convenience
     * and commodity reasons.
     */

    virtual bool  epoch_changed             (void);

    /**
     * @brief Reset the epoch changed flag, acknowledging a change in epoch.
     *
     * Once that a calling module has detected an epoch change using method
     * epoch_changed(), it is possible to tell the reader that it must reset
     * (to false) this flag using epoch_change_ack() (this method).
     *
     * Readers will only reset the value of the epoch changed flag when
     * a new read operation is started. Therefore, repeated calls to
     * epoch_changed() will always return the same value if no new
     * read operations are initiated.
     *
     * If, by any reason, a calling module needs to be notified only once
     * about an epoch change, this method may be used.
     *
     * Note, however, that the epoch changed flag will be set conveniently
     * as soon as new read_* operations are issued again.
     */

    virtual void  epoch_change_acknowledged (void);

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
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int   read_active_flag          (bool& active) = 0;

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
     * If an identifier is found in the input file whose length is greater than
     * the maximum allowed size (id_len), an error code will be returned.
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int   read_identifier           (char*& identifier,
                                             int    id_len) = 0;

    /**
     * @brief Read the instance identifier in an l-record.
     *
     * @param instance_id The instance identifier to read.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. The value corresponding to the
     *              instance identifier read from the input file is not a valid
     *              integer.
     *
     * This method may be used only when an l-record is being read. Otherwise,
     * an error condition will arise.
     *
     * It must be called right after read_identifier() has been invoked. Calling
     * this method at any other time will arise an error condition.
     *
     * After a successful completion, input parameter instance_id will contain the
     * l-record instance identifier.
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int   read_instance_id          (int& instance_id) = 0;
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
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. At least one of the values corresponding to
     *              tags, expectations or covariance matrix values is no a
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
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int   read_l_data               (int      n_tag_values,
                                             double*& the_tags,
                                             int      n_expectation_values,
                                             double*& the_expectations,
                                             int&     n_covariance_values_found,
                                             double*& the_covariance_values) = 0;

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
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int   read_o_data               (int   n_parameter_iids,
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
     *
     * Note that since this is an abstract class, this particular method
     * is not implemented here. Descendant classes must provide with
     * an specific implementation adhering to the interface described here.
     */

    virtual int   read_time                 (double& time) = 0;

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
     * been exhausted. See the example in the detailed description of
     * this class for an example on how to check and treat this situation.
     *
     * After a successful completion, the record_type parameter will contain
     * either an 'l' or an 'o' character (always lowercas) denoting the
     * type of record being read (l- and o-records correspondingly).
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

    /// @brief Flag. When true, the first read record in an epoch was an l-one.

    bool          first_l_read_;

    /// @brief Flag. When true, the file has been completely read, so no
    /// more data is available.

    bool          is_eof_;

    /// @brief Active / removed flag for the last record (either l- or o-) read.

    bool          last_record_is_active_;

    /// @brief Flag indicating that, when reading a new record, a new epoch
    /// has been detected.

    bool          last_epoch_changed_;

    /// @brief Time associated to all the records in the last full epoch read.

    double        last_epoch_time_;

    /// @brief Flag. When true, it states that the time of the last epoch read
    /// is available to check.

    bool          last_epoch_time_available_;

    /// @brief Control flag. When true, it indicates that a call to read_active_flag()
    /// has already been made for the current record.

    bool          read_active_flag_;

    /// @brief Control flag. When true, it indicates that a full record has
    /// been read. In other words, a full sequence of calls to the different
    /// read_* calls has been made.

    bool          read_completed_;

    /// @brief Control flag. When true, it indicates that a call to read_identifier()
    /// has already been made for the current record.

    bool          read_identifier_;

    /// @brief Control flag. When true, it indicates that a call to
    /// read_instance_id() has already been made for the current record.

    bool          read_iid_;

    /// @brief Control flag. When true, it indicates that a call to read_l_data()
    /// has already been made for the current record.

    bool          read_l_data_;

    /// @brief Control flag. When true, it indicates that a call to read_o_data()
    /// has already been made for the current record.

    bool          read_o_data_;

    /// @brief Control flag. When true, it indicates that a call to read_time()
    /// has already been made for the current record.

    bool          read_time_;

    /// @brief Control flag. When true, it indicates that a call to read_type()
    /// has already been made for the current record.

    bool          read_type_;

    /// @brief Control flag. True when the first epoch in the file is being
    /// read, false otherwise.

    bool          reading_first_epoch_;

    /// @brief Control flag. When true, it indicates that the record being
    /// read if an l-record.

    bool          reading_l_;

    /// @brief Control flag. When true, it indicates that the record being
    /// read if an o-record.

    bool          reading_o_;

};

#endif // OBS_E_BASE_FILE_READER_HPP
