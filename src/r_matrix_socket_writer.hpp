/** \file r_matrix_socket_writer.hpp
  \brief  Correlation matrices socket writer.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_SOCKET_WRITER_HPP
#define R_MATRIX_SOCKET_WRITER_HPP

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "PracticalSocket.h"

#include <xtl/objio.h>
#include <xtl/xdr.h>

using namespace std;

/// @brief Default buffer size: 100 Kb

#define _R_MATRIX_SOCKET_WRITER_DEFAULT_BUFFER_SIZE 102400

/**
  * @brief  Socket writer class. Writes correlation matrices data through a
  * socket connection.
  *
  * \ingroup ASTROLABE_data_IO
  *
  * This class implements a socket writer for ASTROLABE <r-matrix> correlation
  * matrices data, that is, writes (sends) <r-matrix> data through a socket connection.
  *
  * Ths class inherits from no one, since the interface it implements is
  * slightly different to the rest of writers.
  *
  * The logical sequence to follow when writing observation-events based data
  * through a socket connection is the next one:
  *
  * - The calling module instantiates an r_matrix_socket_writer object.
  * - Then, it opens a connection using open().
  * - Afterwards, it is possible to write (send) as many r-records
  *   as necessary using methods write_r().
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

class r_matrix_socket_writer
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
         r_matrix_socket_writer    (void);

    /**
     * \brief Destructor.
     */
         ~r_matrix_socket_writer   (void);

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
     * This method opens the socket, enabling the write operations
     * (see write_l() as well as close()).
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
     * \brief Write (send) an r-record.
     *
     * \param active The r-record is active (true) or removed (false).
     * \param time Time tag of the r-record.
     * \param n_correlation_values The number of correlation values
     *        to send (number of correlation values in correlation matrix).
     * \param buffer_correlations The correlation values to send.
     *
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Socket is not open so it is not possible to write.
     *        - 2: I/O error detected while sending through the socket.
     *        - 3: NOT USED.
     *        - 4: NOT USED.
     *        - 5: Invalid number of values specified in n_correlation_values
     *             (must be > 0).
     *
     * Once that the communications channel has been open by means of open(),
     * it is possible to write (send) r-records. This method writes
     * (sends) an r-record, with the format defined by ASTROLABE.
     *
     * Note that the record type ('r' in this case) needs not to be specified.
     * This method writes (sends) this type automatically.
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
     * - Type - a single "r" character (denoting this one is an r-record).
     * - Active flag. A single character, either a '1' (active) or a
     *   '0' (removed)
     * - The time tag, as a double value.
     * - The number of correlation values (n_correlation_values), as an integer.
     * - The correlation values themselves, an array of n_correlation_values
     *   double values.
     *
     */

    int  write_r                   (bool         active,
                                    double       time,
                                    int          n_correlation_values,
                                    double*      buffer_correlations);

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

    /// \brief Name of the host or IP quad to connect to.

    string                               host_;

    /// \brief Port to connect through.

    unsigned short                       port_;

    /// \brief The server socket used to set the connection in server mode.

    TCPServerSocket*                     server_socket_;

    /// \brief The socket object used to communicate.

    TCPSocket*                           socket_;

    /// \brief Flag showing wheter the socket connection is open.

    bool                                 socket_is_open_;
};

#endif // R_MATRIX_SOCKET_WRITER_HPP
