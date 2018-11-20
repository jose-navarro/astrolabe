/** \file r_matrix_socket_reader.hpp
  \brief Socket reader for correlation matrices data.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_SOCKET_READER_HPP
#define R_MATRIX_SOCKET_READER_HPP

#include "PracticalSocket.h"

#include <xtl/objio.h>
#include <xtl/xdr.h>

/// \brief Default buffer size: 100 Kb

#define _R_MATRIX_SOCKET_READER_DEFAULT_BUFFER_SIZE 102400

/**
 * @brief Socket reader for r-matrix (correlation matrices for observations, states
          and residuals) data.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements a socket reader for data based on the \<r_matrix_file\> specification,
 *  that is, reads (receives) r_matrix based data through a socket connection.
 *
 * Ths class inherits from no one, since the interface it implements is
 * slightly different to the rest of readers of this kind of data.
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
 * - The actual read (receive) process may start now. The actual read action
 *   is split in several methods that must be used as stated below:
 *   -# read_type(). Retrieves the type of the record to come. In r_matrix
 *    based files, all records must be of type "r".
 *   -# read_active_flag(). To ascertain whether the record is active or not.
 *   -# Call read_time(), to retrieve the time tag in the r-record being read.
 *   -# Finally, call read_r_data() to retrieve the specific information for these
 *    records. This information is a correlation matrix.
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

class r_matrix_socket_reader
{
  public:

    /**
     * \brief Closes the r_matrix socket reader, disabling any other
     *        further action on it.
     * \return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the socket connection.
     *        - 2: NOT USED.
     *
     * Closes the r_matrix socket reader, disabling any further read operations.
     *
     */

    int  close                      (void);

    /**
     * \brief Default constructor.
     */

         r_matrix_socket_reader     (void);

    /**
     * \brief Destructor
     */

         ~r_matrix_socket_reader    (void);

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
     *        - 4: NOT USED
     *        - 5: NOT USED
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
     * \brief Reads (receives) the active flag of the r-record.
     *
     * \param active Value of the r-record's active flag.
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
    * @brief Read correlation matrix values in an r-record.
    *
    * @param n_correlation_values_found The number of correlation values
    *        read from the r-record.
    * @param the_correlation_values Array that will store the correlation
    *        values. The number of actual values read will be stored in parameter
    *        n_correlation_values_found.
    * @return Error code. Error code values:
    *         - 0: Successful completion.
    *         - 1: NOT USED.
    *         - 2: I/O error while reading (might happen if the file
    *              has not been opened).
    *         - 3: Invalid call sequence (see description below about
    *              the proper call sequence).
    *         - 4: NOT USED.
    *         - 5: NOT USED.
    *
    * This method must be called right after read_time() has been invoked.
    * Calling this method at any other moment will arise an error condition.
    *
    * A successful execution will provide the values of the correlation values
    * stored in the the_correlation_values output parameter.
    *
    */

    int  read_r_data                (int&     n_correlation_values_found,
                                     double*& the_correlation_values);

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
     *
     * It must be called right after a call to read_active_flag() has
     * been made.  Using this method in a different moment will arise an
     * error condition.
     *
     * Once run, time will contain the value of the time tag for the
     * record being read.
     *
     */

    int  read_time                  (double& time);

    /**
     * \brief Reads (receives) the event record type.
     *
     * \param record_type The type of the record being read (always
     *        a lowercase 'r').
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
     * either an 'r' (always lowercase) denoting the type or record
     * being read (always an r-record).
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

    /// \brief Name of the host or IP quad to connect to (in client socket mode).

    string                              host_;

    /// \brief Flag. When true, it means that the writer signaled the end of
    /// the transmission, so no more data is available to read.

    bool                                is_eof_;

    /// \brief Active / removed flag for the last r-record read.

    bool                                last_record_is_active_;

    /// \brief Control flag. When true, it indicates that a call to read_active_flag()
    /// has already been made for the current record.

    bool                                read_active_flag_;

    /// \brief Control flag. When true, it indicates that a full record has
    /// been read. In other words, a full sequence of calls to the different
    /// read_* calls has been made.

    bool                                read_completed_;

    /// \brief Control flag. When true, it indicates that a call to read_r_data()
    /// has already been made for the current record.

    bool                                read_r_data_;

    /// \brief Control flag. When true, it indicates that a call to read_time()
    /// has already been made for the current record.

    bool                                read_time_;

    /// \brief Control flag. When true, it indicates that a call to read_type()
    /// has already been made for the current record.

    bool                                read_type_;

    /// \brief The server socket used to set the connection in server mode.

    TCPServerSocket*                    server_socket_;

    /// \brief The regular TCP/IP socket used to actually read data.

    TCPSocket*                          socket_;

    /// \brief Flag showing wheter the socket connection is open.

    bool                                socket_is_open_;

};

#endif // R_MATRIX_SOCKET_READER_HPP
