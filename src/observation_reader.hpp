/** \file observation_reader.hpp
  \brief  Class able to read observation-event data from all kinds of
          ASTROLABE supported data channels.
*/

#ifndef OBSERVATION_READER_HPP
#define OBSERVATION_READER_HPP

#include "observation_socket_reader.hpp"
#include "observation_file_reader_bin_backward.hpp"
#include "observation_file_reader_bin_forward.hpp"
#include "observation_file_reader_txt_backward.hpp"
#include "observation_file_reader_txt_forward.hpp"

#include "gp_utils.hpp"
#include "gp_astrolabe_header_file_parser.hpp"

#include "segmented_file_utils.hpp"


#include <list>
#include <string>

using namespace std;

/**
   \brief  Class able to read observation-event data from all kinds of
   ASTROLABE supported data channels.

   \ingroup ASTROLABE_data_IO

   This class is able to read observation-event data from all the different
   types of data channels defined by the ASTROLABE I/O interface.

   This includes both server sockets and files; files may be either binary or
   text.

   THEREFORE, THIS IS THE CLASS TO USE WHEN READING ASTROLABE OBSERVATION DATA. All other
   classes in this library implement the different readers needed to
   deal with the different kind of ASTROLABE data channels and should never
   be invoked directly.

   The way to use this reader is pretty straightforward:

   - Instantiate the observation_reader object (either automatically or using explicitly
     its constructor).
   - Set the full file name and path of the validating XML schema defining the
     syntax of ASTROLABE header files using method set_schema_file().
     This step, however, is optional. Production-level software should always
     rely on an XML schema to validate the correctness of the syntax of
     ASTROLABE header files. Only developers improving THIS library should
     work with no schemas.
   - Set the full file name and path of the ASTROLABE header file.
   - Read the ASTROLABE header file calling method get_metadata(). The
     purpose of this method is to ascertain what kind of underlying data channel
     (socket connection, file, either binary or text,...) is being used. Errors
     may be detected in this step if the ASTROLABE header file is not
     correctly written.
   - Call method set_data_channel to state whether socket connections will
     behave as server or client ones. NOTE that although this method is
     useful only when the data channel to read is a socket, it is harmful
     to use it when files are used, so the recommendation is to call it
     always to obtain homogeneous code no matter what's the kind of channel
     being read.
   - Open the reader using method open(). Here it is possible to select
     a forward or backward reader (note that socket readers may not work
     in reverse mode). Note that this reader, will directly read the underlying
     data channel (parameter data).
   - Then, a loop must be implemented until the "end-of-file" (or end-of-data)
     condition is detected. Each iteration must read a complete record in
     the following way:
     - Read the type (l or o) of the record to come using read_type(). This method
       is the one able to detect an end of data condition, so after calling
       it, this situation must be checked in order to break the loop if
       necessary.
     - Read the active flag using read_active_flag(). Note that this method
       may report that the record being read is not active. No matter what's
       the value of the active flag, the rest of the record must be FULLY
       read; otherwise, errors will arise.
     - Read the identifier calling read_identifier().
     - ONLY when reading an l-record (this may be ascertained after reading
       the type of the record), read the identifier using read_instance_id().
     - Read the time tag for the record. This is done calling read_time().
       Note that at this point, the reader is able to tell whether a new
       epoch is arriving. So just after calling read_time() AND NOT BEFORE,
       it is possible to call epoch_changed() to learn if such epoch
       change has taken place.
     - Depending on the type of the record (l or o) call read_l_data() or
       read_o_data() correspondingly to obtain the specific data for
       each kind of record.
       to call 
   - Once out of the read loop, call close() to close the reader.
   - Destroy the reader (this is not necessary, of course, if it was
     instantiated automatically).

   Note that all methods in this class are completely independent of the kind of
   output channel (sockets, files either text of binary...) being used.

   This class also implements the restrictions that apply on how l- and o- records
   must be found. For instance, new epochs must start with an l-record. Violating
   such requisite will be reported by the proper error code.

   Note that this class relies on the XTL or eXternalization Template Library
   (http://xtl.sourceforge.net/) which is a portable implementation of the
   XDR standard (http://en.wikipedia.org/wiki/External_Data_Representation).

   This library is used by the observation_reader class to decode the data when
   received via a socket channel, to avoid the problems that arise when different
   computer architectures are involved in the process of sending / receiving
   information. Such problems are endianness and the specific representation
   format choosen for binary data as integer or double values.

   The XDR standard is NOT used when reading or writing from / to file channes;
   only sockets are affected by this technique.

   This reader, therefore, use XTL to decode the information sent by observation_writer
   when the underlying data channel is a socket connection.
*/

class observation_reader
{
  public:

    /**
     * @brief Closes the reader, disabling any other further action on it.
     * @return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the reader.
     */

    int                  close                     (void);

    /**
     * @brief Checks if the data channel being read is a socket.
     * @return True if the data channel in use is a socket, false
     *         otherwise.
     *
     */

    bool                 data_channel_is_file      (void);

    /**
     * @brief Checks if the data channel being read is a server socket.
     * @return True if the data channel in use is a server socket, false
     *         otherwise.
     *
     */

    bool                 data_channel_is_socket    (void);

    /**
     * @brief Epoch change detection.
     *
     * @return True if the time tag of the record being read differs from that
     *         in the previous fully read record, or, in other words, when
     *         the record being read belongs to a new epoch.
     *
     * All records in an observation channel are ordered by time. All those sharing
     * the same time tag constitute an epoch.
     *
     * When reading successive records in an observation channel, soon or later
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

    bool                 epoch_changed             (void);

    /**
     * @brief Reset the epoch changed flag, acknowledging a change in epoch.
     *
     * Once that a calling module has detected an epoch change using method
     * epoch_changed(), it is possible to tell the reader that it must reset
     * (to false) this flag using epoch_change_ack() (this method).
     *
     * The reader will only reset the value of the epoch changed flag when
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

    void                 epoch_change_acknowledged (void);

    /**
     * @brief Get the list of errors that have been detected when retrieving
     *        the ASTROLABE header file metadata.
     * @param list_of_errors The textual description of the different errors found.
     * @return The number of errors included in the list. May be zero if no errors
     *        were detected when retrieving the metadata.
     *
     * Call this method to retrieve the list of errors found in the
     * ASTROLABE header file metadata.
     *
     * The list of errors contain a textual description of every error found, in
     * the same order in which these were detected.
     *
     * If no errors were found, the list will be empty.
     *
     * This method returns the total number of errors in the list, which may be
     * zero. See method get_metadata() to learn how to tell if errors were
     * detected when metadata was retrieved.
     *
     * Note also that the input / output parameter list_of_errors is always
     * emptied before adding any errors to it.
     *
     */

    int                  get_metadata_errors       (list<string>& list_of_errors);

    /**
     * @brief Get the list of warnings that have been detected when retrieving
     *        the ASTROLABE header file metadata.
     * @param list_of_warnings The textual description of the different warnings found.
     * @return The number of warnings included in the list. May be zero if no warnings
     *        were detected when retrieving the metadata.
     *
     * Call this method to retrieve the list of warnings found in the
     * ASTROLABE header file metadata.
     *
     * The list of warnings contain a textual description of every warning found, in
     * the same order in which these were detected.
     *
     * If no warnings were found, the list will be empty.
     *
     * This method returns the total number of warnings in the list, which may be
     * zero. See method get_metadata() to learn how to tell if warnings were
     * detected when metadata was retrieved.
     *
     * Note also that the input / output parameter list_of_warnings is always
     * emptied before adding any warnings to it.
     *
     */

    int                  get_metadata_warnings     (list<string>& list_of_warnings);


    /**
     * @brief Check if the data in the observation channel has been completely read.
     *
     * @return True if all data in the observation channel has already been read,
     *        false otherwise.
     *
     * Use this method to check if the contents of the observation channel has
     * been exhausted (completely read) or not. In other words, check if
     * the "end of the file" has been reached.
     *
     * Strictly speaking, backwards file readers will never reach the end
     * of the file but its beginning, since the read process traverses
     * the file from bottom to top.
     *
     * However, the concept of data exhaustion is valid in all cases
     * (socket, forward and reverse file readers) and this is the method to
     * use in all cases to check if there is anything left to read. That is
     * why an unique method has been provided for this purpose (instead of
     * adding an hypothetical is_bof() or "is beginning of file" to be used
     * only with reverse file readers. In this way, the interface for any
     * reader is kept uniform.
     */

    bool                 is_eof                    (void) const;

    /**
     * @brief Check if the underlying data channel (file or socket) has already
     * been opened.
     *
     * @return True if the data channel has already been opened, false otherwise.
     */

    bool                 is_open                   (void) const;

    /// \brief Constructor.

                         observation_reader              (void);

    /// \brief Destructor

                         ~observation_reader             (void);

    /**
     * @brief Opens the underlying data channel, enabling I/O operations.
     *
     * @param reverse_mode Open the data channel to read it in reverse mode.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The data channel was already open.
     *        - 2: Unable to open the data channel. Several reasons may be
     *             the culprit. When working with files, there may be
     *             problems with the names of the segmented files used
     *             to "simulate" actual data. Another reason may be
     *             that it is not possible to open the first segmented
     *             file because of I/O problems. Finally, when working
     *             with sockets, an incorrect host:port specification
     *             in the ASTROLABE header file may be the cause of the problem.
     *        - 3: Not enough memory to allocate internal buffers.
     *        - 4: End of file condition detected - the data channel is empty.
     *        - 5: I/O error detected while reading the channel.
     *        - 6: Malformed records found.
     *        - 7: Internal buffer too small to read data.
     *         -8: Reverse mode not allowed in socket data channels.
     *
     * Once a reader has been instantiated, and the metadata included in
     * the input ASTROLABE header file loaded, it is possible to open
     * the underlying data channel (either a file or a socket connection)
     * to enable actual read operations.
     *
     * This method opens the channel.
     *
     * It is possible to specify that such data channel be read in reverse mode
     * (see parameter reverse_mode) but only if it is a file, and not a socket
     * connection.
     *
     * To check whether the channel is a socket connection or not, methods
     * data_channel_is_file() or data_channel_is_socket() should be
     * called prior to invoking open().
     *
     */

    int                  open                      (bool reverse_mode=false);

    /**
     * @brief Reads the active flag for either l- or o-records.
     *
     * @param active Value of the l- or o-record's active flag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the observation
     *              channel has not been opened).
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

    int                  read_active_flag          (bool&       active);

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
     *         - 2: I/O error while reading (might happen if the observation
     *              channel has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: The identifier found is longer than the maximum
     *              dimension allowed (parameter id_len);
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
     * If an identifier is found in the input observation channel whose length is greater than
     * the maximum allowed size (id_len), an error code will be returned.
     *
     */

    int                  read_identifier           (char*& identifier,
                                                    int    id_len);

    /**
     * @brief Read the instance identifier in an l-record.
     *
     * @param instance_identifier The instance identifier to read.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the observation channel
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
     * After a successful completion, input parameter instance identifier will contain the
     * l-record instance identifier.
     *
     */

    int                  read_instance_id          (int& instance_identifier);
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
     *         - 2: I/O error while reading (might happen if the observation channel
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. At least one of the values corresponding to
     *              auxiliars, expectations or standard deviation is not a
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
     */

    int                  read_l_data               (int      n_tag_values,
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
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the observation channel
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
     */

    int                  read_o_data               (int   n_parameter_iids,
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
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the observation
     *              channel has not been opened).
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
     */

    int                  read_time                 (double& time);

    /**
     * @brief Read the event record type.
     *
     * @param record_type The type of the record being read, either an 'l' or
     *        an 'o' (both lowercase).
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End-of-file (no more data).
     *         - 2: I/O error while reading (might happen if the observation channel
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
     *         . 7: Error while closing / opening current / next chunk file.
     *
     * read_type() is the method that starts the process of reading a
     * full observation record, either l- or o-.
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
     */

    int                  read_type                 (char& record_type);

    /**
     * @brief Set the parameters controlling the behaviour of socket
     *        readers,when these are used.
     *
     * @param socket_works_in_server_mode Flag stating how socket readers
     *        instantiated by this reader must behave concerning the
     *        way to connect: like servers (true) waiting for someone else
     *        to start a connection or, on the contrary, like clients (false)
     *        so the socket reader will connect actively to a host and port
     *        before starting to read.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Invalid call sequence. This method may be called
     *             only once, and never after open() has been invoked.
     *
     * This method must be used to state how a reader using a socket
     * data channel must behave concerning connection issues. Sockets
     * are normally classified as servers and clients. Server sockets
     * wait until someone else (the client) starts an active connection.
     * On the contrary, client sockets starts a connection immediately,
     * hoping that someone else (a server) is already waiting for them
     * to establish such connection.
     *
     * Parameter socket_works_in_server_mode is used to do this. Setting
     * its value to true means that if a socket reader is instantiated
     * by this class, it will behave as a server socket, that is, will
     * wait for someone else to actually connect. When set to false,
     * these socket readers will work, conversely, as clients, starting
     * the connection by themselves.
     *
     * The information needed to connect (host for server socket readers,
     * host and port for client socket readers) will be obtained from
     * the ASTROLABE header file, so it is not necessary to set it here.
     *
     * This method may be called only once, and immediately before
     * open() is invoked. An error condition is reported otherwise.
     *
     * NOTE THAT IT IS POSSIBLE TO CALL THIS METHOD even if the
     * underlying data channel used by this class is NOT a socket
     * reader. Therefore, it is recommended to call it always; when
     * no sockets be used, it will have no effect on the reader.
     *
     */

    int                  set_data_channel          (bool socket_works_in_server_mode);

    /**
     * @brief Set the name of the input astrolabe header file
     *        and read its metadata information.
     *
     * @param header_file_name Name of the astrolabe header file.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The name of the astrolabe header file may not be NULL.
     *        - 2: Invalid call sequence. This method may be called
     *             only once, and never after open() has been invoked.
     *        - 3: Warning and / or errors detected when loading
     *             the astrolabe header file metadata. These
     *             might impede the correct reading of
     *             actual data.
     *
     * Use this method to inform the reader about the name of the
     * input astrolabe header file to read and to load the
     * metadata defining how actual data is stored.
     *
     * Note that the name of the astrolabe header file may not be NULL.
     *
     * This method may be called only once, and immediately before
     * open() is invoked. An error condition is reported otherwise.
     *
     * Once the name of astrolabe header file is set, this method
     * also parses its contents in order to retrieve the metadata
     * defining how actual data is stored. This process may
     * detect warnings or errors related to the correctness
     * of this metadata (either syntactic or semantic problems).
     * This problem is notified by means of an error code (see
     * the list of error codes at the beginning of this method's
     * description).
     *
     * Should this situation arise, the calling module should
     * invoke get_metadata_warnings() and get_metadata_errors()
     * to obtain the amount and list of warnings and errors
     * respectively.
     *
     * If only warnings have been detected, the calling module
     * may continue with the process of reading actual data, although
     * it would be very convenient to correct the ASTROLABE header
     * file to avoid these warnings to keep appearing.
     *
     * However, when errors are detected, it is not possible to
     * continue; the ASTROLABE header file must be corrected
     * immediately before being able to read its contents.
     *
     */

    int                  set_header_file           (const char* header_file_name);

    /**
     * @brief Set the name of the XML schema defining the grammar of
     *        ASTROLABE header files.
     *
     * @param schema_file_name Name of XML schema file.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The name of the XML schema file may not be NULL.
     *        - 2: The reader has already set the name of the
     *             observations data file, so it is not possible
     *             to call this method.
     *
     * Use this method to inform the reader about the name of the
     * XML schema file to use to validate the syntax of the input
     * ASTROLABE header file.
     *
     * Note that the use of this method is optional, since an XML
     * schema may not be available. However, if used, it must
     * be called right after the observation_reader has been instantiated
     * and before set_header_file is called. This is the
     * only right call sequence to use. If this is not respected,
     * an error code will be returned to notify this violation
     * of the protocol.
     *
     * It is very convenient to specify an XML schema file since
     * doing so the analysis of the syntax of the ASTROLABE header
     * file is much more reliable; the errors reported in the
     * case that errors are found are much more precise.
     *
     * However, it is possible to read an ASTROLABE header file
     * without a backing XML schema. This is only recommended
     * when development of new features of ASTROLABE header files
     * is taking place, since the analysis of the input file
     * can not be precise enough as to determine the cause of
     * errors. When no schema is provided, developers may concentrate
     * on implementing these new features, delaying the adaptation
     * of the XML schema until all the coding is over.
     *
     * In other words, software using this reader should always
     * use an XML schema when run in production environments.
     *
     * Note that the name of the ASTROLABE header schema file may not be NULL.
     *
     * This method will not work if the reader has already opened
     * an input obbservatios file.
     *
     */

    int                  set_schema_file           (const char* schema_file_name);

  protected:

    /**
     * @brief Read the metadata included in the ASTROLABE header file.
     * @param total_metadata_warnings Number of warnings detected while retrieving
     *        the ASTROLABE header file metadata.
     * @param total_metadata_errors Number of warnings detected while retrieving
     *        the ASTROLABE header file metadata.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Either the metadata has already been read or the
     *             ASTROLABE header file has already been opened (open()) so
     *             this method can't be called (again).
     *        - 2: The name of the input ASTROLABE header file file has not been set.
     *        - 3: Warnings detected when retrieving the metadata.
     *
     * Once a reader has been instantiated, it is necessary to retrieve the metadata
     * defining the underlying data channel (either files or sockets). This must
     * be done before any open() or read operation is started.
     *
     * The ASTROLABE header file may contain warnings or errors. When this happens, an
     * specific error code is returned (see the list of error codes above).
     *
     * Warnings may be ignored and the process of reading the observation data
     * file may also continue. However, it is recommended that these warnings be
     * corrected, checking the input file and solving the issues detected.
     *
     * On the contrary, errors imply that the observation data file may not
     * be read.
     *
     * When either warnings or errors have been detected retrieving the metadata,
     * it is possible to obtain a textual description of these using methods
     * get_metadata_warnings() and get_metadata_errors() respectively. This method
     * also set parameters total_metadata_warnings and total_metadata_errors to the
     * number of warnings and errors detected respectively.
     *
     * This method may be called only once per ASTROLABE header file. That is,
     * metadata may be retrieved only once!
     */

    int                  get_metadata              (int& total_metadata_warnings,
                                                    int& total_metadata_errors);

  protected:

    /// \brief The chunk files name generator.

    segmented_file_utils            chunk_names_generator_;

    /// \brief Flag. When true, indicates that the underlying observation
    /// channel is open.

    bool                            data_channel_is_open_;

    /// \brief Flag. Used to indicate that the channel parameters have
    /// already been set.

    bool                            data_channel_is_set_;

    /// \brief Flag. Used to ascertain if the input channel is a socket.
    /// When true, we are using sockets, when false, files.

    bool                            data_channel_is_socket_;

    /// \brief The data parsed from the input astrolabe header file.

    gp_astrolabe_header_file_data   header_data_;

    /// \brief Name of the input astrolabe header file to read.

    string                          header_file_;

    /// \brief The parser used to read the metadata in the input astrolabe
    /// header file.

    gp_astrolabe_header_file_parser header_parser_;

    /// \brief The polymorphic file reader that will be in charge of
    /// reading data from files whatever its format is.

    obs_e_based_file_reader*        file_reader_;

    /// \brief Flat that indicates that the reader must work backwards.

    bool                            is_backwards_mode_set_;

    /// \brief Flag used to signal the end of file condition.

    bool                            is_eof_;

    /// \brief Flag that shows whether the input astrolabe header
    /// file name has been set.

    bool                            is_header_filename_set_;

    /// \brief Flag that shows whether the metadata has been read.

    bool                            is_metadata_read_;

    /// \brief Flag that shows whether the schema file name
    /// has been set.

    bool                            is_schema_filename_set_;

    /// \brief Active / removed flag for the last record (either l- or o-) read.

    bool                            last_record_is_active_;

    /// \brief Flag indicating that, when reading a new record, a new epoch
    /// has been detected.

    bool                            last_epoch_changed_;

    /// \brief Time associated to all the records in the last full epoch read.

    double                          last_epoch_time_;

    /// \brief Flag. When true, it states that the time of the last epoch read
    /// is available to check.

    bool                            last_epoch_time_available_;

    /// \brief Control flag. True when the first epoch in the file is being
    /// read, false otherwise.

    bool                            reading_first_epoch_;

    /// \brief Name of the XML schema file used to validate the syntax
    /// of the ASTROLABE header file to read. When empty, no schema
    /// is available.

    string                          schema_file_;

    /// \brief The socket reader to use when the underlying channel is
    /// a socket connection.

    observation_socket_reader*      socket_reader_;

    /// \brief When using underlying socket readers, this flag states how
    /// these must behave when connecting: as a server or client socket.

    bool                            socket_works_in_server_mode_;

};

#endif // OBSERVATION_READER_HPP
