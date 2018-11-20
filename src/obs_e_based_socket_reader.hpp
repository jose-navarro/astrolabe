/** \file obs_e_based_socket_reader.hpp
  \brief Socket reader for obs-e based data.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_E_BASED_SOCKET_READER_HPP
#define OBS_E_BASED_SOCKET_READER_HPP

#include "PracticalSocket.h"

#include <xtl/objio.h>
#include <xtl/xdr.h>

/// \brief Default buffer size: 1 Kb

#define _EVENT_SOCKET_READER_DEFAULT_BUFFER_SIZE 1024

/**
 * @brief Socket reader for obs-e based data.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements a socket reader for data based on the obs-e specification,
 *  that is, reads (receives) obs-e based data through a socket connection.
 *
 * Ths class inherits from no one, since the interface it implements is
 * slightly different to the rest of readers.
 *
 * Note that all the information received using this reader must be encoded according
 * to the XDR standard (http://en.wikipedia.org/wiki/External_Data_Representation).
 *
 * Such encoding is performed using the eXternalization Template Library (XTL)
 * which may be found here: http://xtl.sourceforge.net/.
 *
 * Therefore, socket writers willing to send the information read by this
 * method must encode data using the same technique.
 *
 * This technique is used to avoid the problems related to endianness (some
 * architectures represent data using the little endian convention, others
 * use big endian) or the different ways used to represent binary data
 * (integers, doubles) in a computer. Encoding the data to send using a
 * neutral, architecture-independent representation guarantees that the
 * receiver will be able to rebuild such information flawlessly.
 *
 * The overall usage philosophy is described below:
 *
 * - A socket reader object must be instantiated.
 * - The reader must be associated to a socket using
 *   method open(). This action enables actual I/O operations (data reception).
 * - The actual read (receive) process may start now. Since two different types of
 *   records exist (l- and o-records) and the type of record to come next is unknown,
 *   the actual read action is split in several methods that must be used as
 *   stated below:
 *   -# read_type(). Retrieves the type of the record to come, that is, either
 *     an l- or o-record.
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
 *    of auxiliars, expectations and standard deviations; when reading an
 *    o-record, the information contains the set of identifiers used.
 * - Note that an end-of-file condition may be issued during the previous
 *   process. This condition is reported ONLY by method read_type() as a
 *   specific error code. Therefore, it is necessary to check the error code
 *   returned by read_type() in order to ascertain if more data is still
 *   available. Any other errors issued by either read_type() or any other
 *   reading method must be treated according to their specific nature.
 * - The reader must be closed using method close() provided to fullfil such
 *   purpose.
 *   Closing the reader will disable any further I/O read operations.
 * - The reader object may be destroyed.
 *
 */

class obs_e_based_socket_reader
{
  public:

    /**
     * \brief Closes the obs-e based socket reader, disabling any other
     *        further action on it.
     * \return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the socket connection.
     *        - 2: NOT USED.
     *
     * Closes the obs-e based socket reader, disabling any further read operations.
     *
     */

    int  close                      (void);

    /**
     * \brief Epoch change detection.
     *
     * \return True if the time tag of the record being read (received) differs
     *         from that in the previous fully read record, or, in other words,
     *         when the record being read belongs to a new epoch.
     *
     * All obs-e based records are ordered by time. All those sharing the same time
     * tag constitute an epoch.
     *
     * When reading (receiving) successive obs-e based records, soon or later
     * the time tag read will change. At this moment, an epoch change
     * has just happened.
     *
     * Epoch changes ARE DETECTED JUST AFTER read_time() is successfully
     * invoked, but not before. Therefore, to obtain valid results,
     * epoch_changed() must be called only AFTER read_time()
     * has been invoked and BEFORE read_type() (the method that triggers
     * the read process for a full new record) is called again.
     *
     * When this method returns true, it means that the record being read
     * belongs to a new epoch, that is, its time tag differs from those
     * found in the records read previously.
     *
     * Of course, calling modules may ascertain this time change by themselves,
     * just tracking the time tag obtained each time a full record
     * (either l- or o-) is completely read. However, this method is provided
     * for convenience and commodity reasons.
     *
     */

    bool epoch_changed              (void);

    /**
     * \brief Reset the epoch changed flag, acknowledging a change in epoch.
     *
     * Once that a calling module has detected an epoch change using method
     * epoch_changed(), it is possible to tell the reader that it must reset
     * (to false) this flag using epoch_change_acknowledged() (this method).
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

    void epoch_change_acknowledged  (void);

    /**
     * \brief Default constructor.
     */

         obs_e_based_socket_reader  (void);

    /**
     * \brief Destructor
     */

         ~obs_e_based_socket_reader (void);

    /**
     * \brief Check if the data in socket channel has been completely read.
     *
     * @return True if all data in the socket channel has already been read,
     *         false otherwise.
     *
     * Use this method to check data has been exhausted (completely read)
     * or not. In other words, check if the "end of the file" has been reached.
     *
     */

    bool is_eof                     (void) const;

    /**
     * \brief Opens the socket reader, enabling read (receive) operations.
     *
     * \param host Name or IP address of the server to connect to when the
     *        socket is configured in client mode. May be NULL if the
     *        socket reader is configured to work in server mode.
     * \param port Number of the port through which communications will
     *        be performed.
     * \param server_mode When set to true, the socket reader will work in
     *        server mode, that is, will accept a connection instead of
     *        connecting itself. When set to false, the socket reader will
     *        actively connect to some host and port instead of waiting for
     *        someone else to connect.
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The socket was already open.
     *        - 2: Unable to open the selected socket.
     *        - 3: Not enough memory to allocate internal buffers.
     *        - 4: NOT USED.
     *        - 5: NOT USED.
     *
     * Once a socket reader has been instantiated it is possible to open
     * the underlying server socket to enable read (receive) operations.
     *
     * This method opens the socket connection and instantiates the internal data
     * buffer, enabling the different read operations available in this class.
     *
     * Note that host must be a valid host name or IP address. IP
     * addresses adhere to the usual quad syntax XXX.XXX.XXX.XXX
     * 
     * The port number must be in the range [1..65535].
     * An incorrect specification of either the host or the port may
     * result in an error.
     *
     * The host parameter is ONLY necessary if the reader socket has been
     * configured to work as a CLIENT socket (parameter server_mode set to false).
     * In this situation, the reader socket will actively connect to some writer
     * socket using the aforementioned host and port values, failing if such
     * writer is not ready to listen.
     *
     * On the contrary, when the reader socket is set to server mode (parameter
     * server_mode set to true) only the port parameter is meaningful. The
     * value of the host parameter will be completely ignored. In this situation,
     * the reader socket will wait for a writer socket to connect.
     *
     */

    int  open                       (const char* host, int port, bool server_mode);

    /**
     * \brief Reads (receives) the code active flag for either l- or o-records.
     *
     * \param active Value of the l- or o-record's active flag.
     * \return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: Error detected while reading (socket not open, actual
     *              problems with data reception, decoding problems...).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *
     * Once the type of the record has been obtained using read_type(), it
     * is possible to read (receive) its active flag. This is the task of this method.
     *
     * Note that read_active_flag() must be called right after read_type() has
     * been invoked and before any other read_* method is used; otherwise,
     * an error condition will arise.
     *
     */

    int  read_active_flag           (bool& active);

    /**
     * \brief Reads (receives) the identifier field for either l- or o-records.
     *
     * \param identifier Value of the l- or o-record's identifier. Must be
     *        preallocated and its size must be the one declared
     *        in parameter id_len.
     * \param id_len The size in chars of parameter identifier.
     * \return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: Error detected while reading (socket not open, actual
     *              problems with data reception, decoding problems...).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: The identifier found in the input stream is longer than
     *              the maximum dimension allowed (parameter id_len);
     *
     * Once the active flag of the record has been obtained using
     * read_active_flag(), it is possible to read (receive) its identifier.
     * This is the task of this method.
     *
     * Note that read_identifier() must be called right after read_active_flag() has
     * has been invoked and before any other read_* method is used; otherwise,
     * an error condition will arise.
     *
     * Parameter identifier must have been preallocated, and its actual size
     * must conform to the one stated by parameter id_len. If these
     * conditions are not met, this method may crash.
     *
     * If an identifier is found in the input stream whose length is greater than
     * the maximum allowed size (id_len), error code 4 will be returned.
     *
     */

    int  read_identifier            (char*& identifier,
                                     int    id_len);

    /**
     * \brief Reads (receives) the tags, expectations and covariance
     *        matrix values in an l-record.
     *
     * \param n_tag_values Number of tag values that must be read from
     *        the l-record. May be zero, since tags are optional.
     * \param the_tags Array that will store the values of the n_tag_values
     *        tags recovered from the l-record. Must be big enough to hold
     *        these values.
     * \param n_expectation_values Number of expectation values that must be
     *        read from the l-record. Must be greater than 0, since expectations
     *        are mandatory.
     * \param the_expectations Array that will store the values of the
     *        n_expectation_values expectations recovered from the l-record.
     *        Must be big enough to hold these values.
     * \param n_covariance_values_found Actual number of covariance matrix
     *        values found in the l-record. The calling module must NOT set
     *        this parameter, but read it once the method is over. This is
     *        so because the number of covariance matrix values is variable.
     *        it may be (a) zero, because no covariance matrix is given at
     *        all for the l-record, being used a default one, (b) as many
     *        values as the number of expectations (that is, only the standard
     *        deviation values are provided) or (c) assuming than n is the
     *        number of expectation values, (n * (n + 1))/2, that is, both
     *        standard deviations and correlation coefficients are retrieved.
     * \param the_covariance_values Array that will store the covariance matrix
     *        values. It must be able to hold as many values as returned in the
     *        worst case (standard deviations plus correlation coefficients).
     * \return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: Error detected while reading (socket not open, actual
     *              problems with data reception, decoding problems...).
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
     *
     * If an I/O error is detected (error code 2) it may be originated by
     * an unmatched number of double elements to read. This method checks
     * how many elements have to be read according to the information
     * received; if this number does not agree with the theoretical
     * number of elements to load (parameter n_double_elements) this error
     * code is returned.
     *
     */

    int  read_l_data                (int      n_tag_values,
                                     double*& the_tags,
                                     int      n_expectation_values,
                                     double*& the_expectations,
                                     int&     n_covariance_values_found,
                                     double*& the_covariance_values);

    /**
     * \brief Reads (receives) the instance identifier in an l-record.
     *
     * \param instance_identifier The instance identifier to read.
     * \return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: Error detected while reading (socket not open, actual
     *              problems with data reception, decoding problems...).
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
     *
     */

    int  read_instance_id           (int& instance_identifier);

    /**
     * @brief Read (receive) the lists of parameter, observation and instrument
     *        instance identifiers from an o-record.
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
     *         - 2: Error detected while reading (socket not open, actual
     *              problems with data reception, decoding problems...).
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

    int  read_o_data                (int   n_parameter_iids,
                                     int*& the_parameter_iids,
                                     int   n_observation_iids,
                                     int*& the_observation_iids,
                                     int   n_instrument_iids,
                                     int*& the_instrument_iids);

    /**
     * \brief Reads (receives) the time tag, either for l- or o-records.
     *
     * \param time The time tag.
     * \return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: NOT USED.
     *         - 2: Error detected while reading (socket not open, actual
     *              problems with data reception, decoding problems...).
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
     *
     */

    int  read_time                  (double& time);

    /**
     * \brief Reads (receives) the event record type.
     *
     * \param record_type The type of the record being read, either an 'l' or
     *        an 'o' (both lowercase).
     * \return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End-of-file (no more data).
     *         - 2: Error detected while reading (socket not open, actual
     *              problems with data reception, decoding problems...).
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
     * arise (see is_eof()).
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
     * as the normal way to detect that the data has
     * been exhausted. See the detailed description of this class
     * for an example on how to check and treat this situation.
     *
     * After a successful completion, the record type parameter will contain
     * either an 'l' or an 'o' character (always lowercas) denoting the
     * type of record being read (l- and o-records correspondingly).
     *
     */

    int  read_type                  (char& record_type);

  protected:

    /**
     * \brief Reads a full record using the open socket connection.
     *
     * \throw SocketException Problems detected while reading (receiving) a
     *        data record.
     *
     * Records of either type are sent at once by socket writers. Therefore,
     * these must also be read (received) at once. This method the task of
     * retrieving such data using the open socket connection.
     *
     * Data received by this method is stored in the internal buffer, and
     * is left there so the different read_* methods may retrieve it
     * when these are called.
     *
     * This method also sends the reception acknowledgement, which
     * is a single byte (its contents does not matter at all; what is
     * important is to send one single byte back to the writer).
     *
     */

    void get_full_record            (void);


  protected:

    /// \brief The data_buffer_ where received data will be stored.

    char*                               data_buffer_;

    /// \brief Actual size in bytes of data_buffer_.

    int                                 data_buffer_size_;

    /// \brief Underlying mem_buffer object used in XDR coding operatinos.

    mem_buffer*                         decoder_membuf_;

    /// \brief Object used to format data according to the XDR estandard.

    XDR_format<mem_buffer>*             decoder_formatter_;


    /// \brief Stream used to read data in XDR coded form.

    obj_input<XDR_format<mem_buffer> >* decoder_stream_;

    /// \brief Flag. When true, the first read record in an epoch was an l-one.

    bool                                first_l_read_;

    /// \brief Name of the host or IP quad to connect to (in client socket mode).

    string                              host_;

    /// \brief Flag. When true, it means that the writer signaled the end of
    /// the transmission, so no more data is available to read.

    bool                                is_eof_;

    /// \brief Flag indicating that, when reading a new record, a new epoch
    /// has been detected.

    bool                                last_epoch_changed_;

    /// \brief Time associated to all the records in the last full epoch read.

    double                              last_epoch_time_;

    /// \brief Flag. When true, it states that the time of the last epoch read
    /// is available to check.

    bool                                last_epoch_time_available_;

    /// \brief Active / removed flag for the last record (either l- or o-) read.

    bool                                last_record_is_active_;

    ///
    /// \brief Flag that states whether reading o-records is legal. Some descendant
    /// classes (as parameter readers for example) do not accept o-records. Setting
    /// this flag to true or false will allow or not the reception of o-record related
    /// information.
    ///

    bool                                o_records_allowed_;


    /// \brief Control flag. When true, it indicates that a call to read_active_flag()
    /// has already been made for the current record.

    bool                                read_active_flag_;

    /// \brief Control flag. When true, it indicates that a full record has
    /// been read. In other words, a full sequence of calls to the different
    /// read_* calls has been made.

    bool                                read_completed_;

    /// \brief Control flag. When true, it indicates that a call to read_identifier()
    /// has already been made for the current record.

    bool                                read_identifier_;

    /// \brief Control flag. When true, it indicates that a call to read_instance_id()
    /// has already been made for the current record.

    bool                                read_iid_;

    /// \brief Control flag. When true, it indicates that a call to read_l_data()
    /// has already been made for the current record.

    bool                                read_l_data_;


    /// \brief Control flag. When true, it indicates that a call to read_o_data()
    /// has already been made for the current record.

    bool                                read_o_data_;

    /// \brief Control flag. When true, it indicates that a call to read_time()
    /// has already been made for the current record.

    bool                                read_time_;

    /// \brief Control flag. When true, it indicates that a call to read_type()
    /// has already been made for the current record.

    bool                                read_type_;

    /// \brief Control flag. When true, it indicates that the record being
    /// read if an l-record.

    bool                                reading_l_;

    /// \brief Control flag. When true, it indicates that the record being
    /// read if an o-record.

    bool                                reading_o_;

    /// \brief Control flag. True when the first epoch in the file is being
    /// read, false otherwise.

    bool                                reading_first_epoch_;

    /// \brief The server socket used to set the connection in server mode.

    TCPServerSocket*                    server_socket_;

    /// \brief The regular TCP/IP socket used to actually read data.

    TCPSocket*                          socket_;

    /// \brief Flag showing wheter the socket connection is open.

    bool                                socket_is_open_;

};

#endif // OBS_E_BASED_SOCKET_READER_HPP
