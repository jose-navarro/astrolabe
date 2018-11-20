/** \file obs_e_based_socket_writer.hpp
  \brief  obs-e based socket writer.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_E_BASED_SOCKET_WRITER_HPP
#define OBS_E_BASED_SOCKET_WRITER_HPP

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "PracticalSocket.h"

#include <xtl/objio.h>
#include <xtl/xdr.h>

using namespace std;

/// @brief Default buffer size: 1 Kb

#define _EVENT_SOCKET_WRITER_DEFAULT_BUFFER_SIZE 1024

/**
 * @brief  Socket writer class. Writes obs-e based data through a
 * socket connection.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements a socket writer for data based on the obs-e specification,
 * that is, writes (sends) obs-e based data through a socket connection.
 *
 * Ths class inherits from no one, since the interface it implements is
 * slightly different to the rest of writers.
 *
 * The logical sequence to follow when writing observation-events based data
 * through a socket connection is the next one:
 *
 * - The calling module instantiates an obs-e based descendant class object.
 * - Then, it opens a connection using open().
 * - Afterwards, it is possible to write (send) as many l- or o- records
 *   as necessary using methods write_l() and write_o() respectively,
 * - Once the writing (sending) is over, the calling module must close() the
 *   connection, so the listener process is aware that no more data will
 *   be sent.
 *
 * Note also that all the information sent using this writer is encoded according
 * to the XDR standard (http://en.wikipedia.org/wiki/External_Data_Representation).
 *
 * Such encoding is performed using the eXternalization Template Library (XTL)
 * which may be found here: http://xtl.sourceforge.net/.
 *
 * Therefore, socket readers willing to retrieve the information sent by this
 * method must decode data using the same technique.
 *
 * This technique is used to avoid the problems related to endianness (some
 * architectures represent data using the little endian convention, others
 * use big endian) or the different ways used to represent binary data
 * (integers, doubles) in a computer. Encoding the data to send using a
 * neutral, architecture-independent representation guarantees that the
 * receiver will be able to rebuild such information flawlessly.
 *
 */

class obs_e_based_socket_writer
{
  public:

    /**
     * \brief Closes the obs_e_based socket writer, disabling any other
     *        further action on it.
     * \return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the socket connection.
     *        - 2: Error while sending the closing command to server.
     *
     * This method sends an end-of-file (or end-of-transmission) message /
     * command to the listener and closes the active connection.
     *
     * Note that this method sends the aforementioned information encoded according
     * to the XDR standard (http://en.wikipedia.org/wiki/External_Data_Representation).
     *
     * Such encoding is performed using the eXternalization Template Library (XTL)
     * which may be found here: http://xtl.sourceforge.net/.
     *
     * Therefore, socket readers willing to retrieve the information sent by this
     * method must decode data using the same technique.
     *
     * This technique is used to avoid the problems related to endianness (some
     * architectures represent data using the little endian convention, others
     * use big endian) or the different ways used to represent binary data
     * (integers, doubles) in a computer. Encoding the data to send using a
     * neutral, architecture-independent representation guarantees that the
     * receiver will be able to rebuild such information flawlessly.
     *
     * Once close() is executed, no other operations, besides calling
     * the destructor, are possible.
     *
     */

    int  close                     (void);

    /**
     * \brief Default constructor.
     */
         obs_e_based_socket_writer (void);

    /**
     * \brief Destructor.
     */
        ~obs_e_based_socket_writer (void);

    /**
     * \brief Check if the socket is already opened.
     *
     * \return True if the socket is opened, false otherwise.
     */

    bool is_open                   (void) const;

    /**
     * \brief Opens the socket writer, enabling send operations.
     *
     * \param host Name or IP address of the server to connect to when the
     *        socket is configured in client mode. May be NULL if the
     *        socket writer is configured to work in server mode.
     * \param port Port to use to open the connection.
     * \param server_mode When set to true, the socket writer will work in
     *        server mode, that is, will accept a connection instead of
     *        connecting itself. When set to false, the socket writer will
     *        actively connect to some host and port instead of waiting for
     *        someone else to connect.
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The socket was already open.
     *        - 2: Unable to open the selected socket.
     *        - 3: Not enough memory to allocate the internal buffer.
     *
     * Once a socket writer has been instantiated it is possible
     * to open the socket connection.
     *
     * This method opens the socket, enabling the different
     * write operations available in this class (see write_l() and
     * write_o(), as well as close()).
     *
     * Note that host must be a valid host name or IP address. IP
     * addresses adhere to the usual quad syntax XXX.XXX.XXX.XXX
     * 
     * The port number must be in the range [1..65535].
     * An incorrect specification of either the host or the port may
     * result in an error.
     *
     * The host parameter is ONLY necessary if the writer socket has been
     * configured to work as a CLIENT socket (parameter server_mode set to false).
     * In this situation, the writer socket will actively connect to some reader
     * socket using the aforementioned host and port values, failing if such
     * reader is not ready to listen.
     *
     * On the contrary, when the writer socket is set to server mode (parameter
     * server_mode set to true) only the port parameter is meaningful. The
     * value of the host parameter will be completely ignored. In this situation,
     * the writer socket will wait for a reader socket to connect.
     */

    int  open                      (const char* host,
                                    int         port,
                                    bool        server_mode);

    /**
     * \brief Write (send) an l-record.
     *
     * \param active The l-record is active (true) or removed (false).
     * \param identifier Identifier of the l-record.
     * \param instance_identifier Instance identifier of the l-record.
     * \param time Time tag of the l-record.
     * \param n_tags Number of tag values to write. May be zero if the
     *        l-record being written has no related tags.
     * \param buffer_tags Array holding the n_tags tag values to write.
     * \param n_expectations Dimension of the expectations array, that is,
     *        the number of expectation values to write.
     * \param buffer_expectations Array holding the n_expectations values
     *        to write.
     * \param n_covariance_values Number of covariance matrix elements to
     *        write. May be either 0 (no covariance matrix is written),
     *        n_expectations (that is, only the standard deviations are written)
     *        or (n_expectations * (n_expectations + 1))/2, (the full covariance
     *        matrix, not duplicating simmetric values).
     * \param buffer_covariance The covariance matrix elements to write.
     *
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Socket is not open so it is not possible to write.
     *        - 2: I/O error detected while sending through the socket.
     *        - 3: NOT USED.
     *        - 4: Parameter identifier must be at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: Invalid number of values specified in n_tags (must be >= 0),
     *             n_expectations (must be > 0) or n_covariance_values (0,
     *             n_expectations or (n_expectations*(n_expectations+1))/2.
     *
     * Once that the communications channel has been open by means of open(),
     * it is possible to write (send) either l- or o-records. This method writes
     * (sends) an l-record, with the format defined by ASTROLABE.
     *
     * Note that the record type ('l' in this case) needs not to be specified.
     * This method writes (sends) this type automatically.
     *
     * Note also that the first record in a set of these containing the same
     * time tag must be always an l-record.
     *
     * Note that this method sends the aforementioned information encoded according
     * to the XDR standard (http://en.wikipedia.org/wiki/External_Data_Representation).
     *
     * Such encoding is performed using the eXternalization Template Library (XTL)
     * which may be found here: http://xtl.sourceforge.net/.
     *
     * Therefore, socket readers willing to retrieve the information sent by this
     * method must decode data using the same technique.
     *
     * This technique is used to avoid the problems related to endianness (some
     * architectures represent data using the little endian convention, others
     * use big endian) or the different ways used to represent binary data
     * (integers, doubles) in a computer. Encoding the data to send using a
     * neutral, architecture-independent representation guarantees that the
     * receiver will be able to rebuild such information flawlessly.
     *
     * The actual information sent is the following (always encoded according to
     * the aforementioned XDR standard):
     *
     * - Type - a single "l" character (denoting this one is an l-record).
     * - Active flag. A single character, either a '1' (active) or a
     *   '0' (removed)
     * - Length of the identifier, as an integer
     * - The identifier itself.
     * - The instance identifier, as an integer.
     * - The time tag, as a double.
     * - The number of tags (n_tags), as an integer.
     * - The tags themselves, an array of n_tags double values.
     * - The number of expectations (n_expectations), as an integer.
     * - The expectations themselves, an array of n_expectations double values.
     * - The number of covariance values (n_covariance_values), as an integer.
     * - The covariance values themselves, an array of n_covariance_values
     *   double values.
     *
     * Note that the number of tags and covariance values may be zero. In such
     * cases, no arrays of tags or no array of covariance values are sent.
     */

    int  write_l                   (bool         active,
                                    char*        identifier,
                                    int          instance_identifier,
                                    double       time,
                                    int          n_tags,
                                    double*      buffer_tags,
                                    int          n_expectations,
                                    double*      buffer_expectations,
                                    int          n_covariance_values,
                                    double*      buffer_covariance);

    /**
     * \brief Write (send) an o-record.
     *
     * \param active The o-record is active (true) or removed (false).
     * \param identifier Identifier of the o-record.
     * \param time Time tag of the o-record.
     * \param n_parameter_iids Total number of parameter instance identifiers
     *        to write. Must be greater than 0.
     * \param the_parameter_iids Buffer storing the parameter instance
     *        identifiers to write.
     * \param n_observation_iids Total number of observation instance identifiers
     *        to write. Must be greater than 0.
     * \param the_observation_iids Buffer storing the observation instance
     *        identifiers to write.
     * \param n_instrument_iids Total number of instrument instance identifiers
     *        to write. May be zero.
     * \param the_instrument_iids Buffer storing the instrument instance
     *        identifiers to write.  
     *
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: File is not open so it is not possible to write.
     *        - 2: I/O error detected while writing to disk.
     *        - 3: An attempt to write an o-record as the first record in the
     *             group sharing the same time tag has been detected. Or an
     *             o-record is not legal in descendant classes, so trying
     *             to call this method produces this error.
     *        - 4: Parameter identifier must at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: NOT USED.
     *        - 6: NOT USED.
     *        - 7: Either the number of parameter or observation instance
     *              identifiers are equal to or less than zero. These values
     *              must be strictly positive. Also, this error code may
     *              be issued if the number of instrument instance identifiers
     *              requested is negative (but it may be zero).
     *
     * Once the communications channel has been opened using open() it is possible
     * to send either l- or o-records. This method writes (sends) an o-record, with
     * the format defined by ASTROLABE.
     *
     * Note that the record type ('o' in this case) needs not to be specified.
     * This method writes (sends) this type automatically.
     *
     * Note also that the first record in a set of these containing the same
     * time tag must be always an l-record. Otherwise, an error condition will
     * be raised (see list of error codes).
     *
     * The number of elements in the different data arrays must be coherent
     * with the number of elements stated in the different dimensioning
     * parameters (n_parameter_iids, n_observation_iids and n_instrument_iids).
     *
     * Note that this method sends the aforementioned information encoded according
     * to the XDR standard (http://en.wikipedia.org/wiki/External_Data_Representation).
     *
     * Such encoding is performed using the eXternalization Template Library (XTL)
     * which may be found here: http://xtl.sourceforge.net/.
     *
     * Therefore, socket readers willing to retrieve the information sent by this
     * method must decode data using the same technique.
     *
     * This technique is used to avoid the problems related to endianness (some
     * architectures represent data using the little endian convention, others
     * use big endian) or the different ways used to represent binary data
     * (integers, doubles) in a computer. Encoding the data to send using a
     * neutral, architecture-independent representation guarantees that the
     * receiver will be able to rebuild such information flawlessly.
     *
     * The actual information sent is the following (always encoded according to
     * the aforementioned XDR standard):
     *
     *  - Type - a single "o" character (denoting this one is an o-record).
     *  - Active flag. A single character, either a '1' (active) or a
     *   '0' (removed)
     *  - Length of the identifier, as an integer
     *  - The identifier itself.
     *  - The time tag, as a double.
     *  - The number of parameter instance identifiers, as an integer.
     *  - The array of parameter instance identifiers, which contains
     *    n_parameter_iids int values
     *  - The number of observation instance identifiers, as an integer.
     *  - The array of observation instance identifiers, which contains
     *    n_observation_iids int values
     *  - The number of instrument instance identifiers, as an integer. This
     *    value may be zero.
     *  - If the number of instrument instance identifiers is positive, then
     *    the array of parameter instance identifiers, which contains
     *    n_parameter_iids int values
     */

    int  write_o                   (bool         active,
                                    char*        identifier,
                                    double       time,
                                    int          n_parameter_iids,
                                    int*         the_parameter_iids,
                                    int          n_observation_iids,
                                    int*         the_observation_iids,
                                    int          n_instrument_iids,
                                    int*         the_instrument_iids);


  protected:

    /**
     * \brief Read a single character from the socket connection to
     *        acknowledge a former write (send) operation.
     *
     * \throw SocketException Unable to read the incoming character.
     *
     * Each write (send) operation (see write_l(), write_o() and
     * close() needs an acknowledgement from the receiver process.
     *
     * The send acknowledgment consists of a single character
     * sent by the receiver.
     *
     * This method takes care of reading such acknowledgement.
     *
     * Note that the single character in the acknowledgement message
     * is NOT encoded using the XDR standard and is transmitted as
     * is. In fact, it does not matter what character is sent; only
     * that it is sent (and received).
     *
     */

    void getACK                    (void);

  protected:

    /// \brief Underlying mem_buffer object used in XDR coding operatinos.

    mem_buffer*                          coder_membuf_;

    /// \brief Object used to format data according to the XDR estandard.

    XDR_format<mem_buffer>*              coder_formatter_;


    /// \brief Stream used to write data in XDR coded form.

    obj_output<XDR_format<mem_buffer> >* coder_stream_;

    /// \brief The data_buffer_ used to store the data to write.

    char*                                data_buffer_;

    /// \brief Actual size in bytes of data_buffer_.

    int                                  data_buffer_size_;


    /// \brief Flag used to check if an l-record has already been written
    /// in the current epoch (set of l- and o- records with the same time
    /// tag).

    bool                                 first_l_written_;

    /// \brief Flag used to indicate that no writes operations have been
    /// executed yet (either write_l() or write_o()) and that therefore
    /// there exist no "previous" time tag to detect an epoch change.

    bool                                 first_time_writing_;

    /// \brief Name of the host or IP quad to connect to.

    string                               host_;

    /// \brief Value of the current time tag. Used to detect new epochs
    /// (set of l- and o-records with the same time tag).

    double                               last_time_tag_;

    ///
    /// \brief Flag that states whether writing o-records is legal. Some descendant
    /// classes (as parameter writers for example) do not accept o-records. Setting
    /// this flag to true or false will allow or not the transmission of o-record related
    /// information.
    ///

    bool                                 o_records_allowed_;

    /// \brief Port to connect through.

    unsigned short                       port_;

    /// \brief The server socket used to set the connection in server mode.

    TCPServerSocket*                     server_socket_;

    /// \brief The socket object used to communicate.

    TCPSocket*                           socket_;

    /// \brief Flag showing wheter the socket connection is open.

    bool                                 socket_is_open_;
};

#endif // OBS_E_BASED_SOCKET_WRITER_HPP
