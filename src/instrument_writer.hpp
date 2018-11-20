/** \file instrument_writer.hpp
  \brief  Class able to write instrument data to some of the
          data channel defined by ASTROLABE.
  \ingroup ASTROLABE_data_IO
*/

#ifndef INSTRUMENT_WRITER_HPP
#define INSTRUMENT_WRITER_HPP

#include "instrument_socket_writer.hpp"
#include "instrument_file_writer_txt.hpp"

#include "astrolabe_header_file_writer.hpp"

#include "segmented_file_utils.hpp"
#include "lineage_data.hpp"


/**
   \brief  Class able to write instrument data to some of the
   data channels supported by ASTROLABE.

   \ingroup ASTROLABE_data_IO

   This class is able to write instrument data to some of the data channels
   defined by the ASTROLABE I/O interface. These includes sockets and text
   files only. Binary files are NOT supported.

   THIS IS THE CLASS TO USE WHEN WRITING ASTROLABE INSTRUMENT DATA. All other
   classes in this library implement the different writers needed to
   deal with the different kind of ASTROLABE data channels and should never
   be invoked directly.

   The way to use this writer is pretty straightforward:

   - Set the kind of output channel depending on its type (file or socket). See
     the different set_data_channel() methods available to do so.
   - Open the channel using method open().
   - Write all the needed information to the channel using method write_l()
     repeatedly until data is exhausted and then,
     close the channel using method close().

   Note that all methods but set_data_channel() are completely independent
   of the kind of output channel being used.

   Note that this class relies on the XTL or eXternalization Template Library
   (http://xtl.sourceforge.net/) which is a portable implementation of the
   XDR standard (http://en.wikipedia.org/wiki/External_Data_Representation).

   This library is used to encode the data when sent via a socket channel, to
   avoid the problems that arise when different computer architectures are
   involved in the process of sending / receiving information. Such problems
   are endianness and the specific representation format choosen for binary
   data as integer or double values.

   The XDR standard is NOT used when writing to file channes; only sockets are
   affected by this technique.
 */

class instrument_writer
{
  public:

    /**
     * \brief Closes the instrument writer, disabling any other
     *        further action on it.
     * \return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the data channel.
     *        - 2: I/O error while writing data to disk.
     */

    int  close                         (void);

    /**
     * \brief Default constructor.
     */
         instrument_writer             (void);

    /**
     * \brief Destructor.
     */
        ~instrument_writer             (void);

    /**
     * \brief Check if the underlying data channel has already been opened.
     *
     * \return True if the data channe has already been opened, false otherwise.
     */

    bool is_open                       (void) const;

    /**
     * \brief Opens the writer, enabling I/O operations.
     *
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The channel was already open.
     *        - 2: Unable to open the selected channel.
     *        - 3: Not enough memory to allocate the internal buffer.
     *        - 4: The channel has not been correctly set (invalid call
     *             sequence).
     *
     * Once a writer has been instantiated and its parameters set using
     * either of the methods set_data_channel(), it is possible to open
     * the underlying data channel to enable I/O operations.
     *
     * This method opens the channel (whatever it is), enabling the different
     * write operations available in this class (see write_l() and close()).
     *
     * Note that open() may be called only once and just after either
     * of the methods set_data_channel has been invoked.
     *
     */

    int  open                          (void);

    /**
     * \brief Set the data channel to work with files
     *
     * \param astrolabe_header_file_name Name (may include the full path) of the
     *        astrolabe header file to write that will describe how data is
     *        organized.
     * \param instrument_data_file_name Name of the file where actual instrument data
     *        will be stored. A suffix ("_1") will be added to the base name (see
     *        the detailed description for details).
     * \param lineage_info The lineage of the astrolabe header file, where information
     *        about (for instance) authors, project, task, etc. is written.
     *
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The data channel is already set.
     *        - 2: The name of output astrolabe header file may not be NULL.
     *        - 3: The file name for actual instrument data is NULL.
     *
     * After an instrument writer has been instantiated, the parameters defining the
     * type of data channel used must be specified.
     *
     * This methods sets the output channel as a TEXT file.
     *
     * Note that unlike observation, parameter, and correlation matrices writers,
     * instrument files are never split into several chunks. This is so because
     * files defining instrument constants are usually very small, containing,
     * maybe, a few l-records. This means that providing an interface able
     * to split these files is an overkill. Consequently, the interface of
     * this method provides no parameters to configure the file-splitting
     * process.
     *
     * However, and to adhere to the ASTROLABE standard, the actual (and
     * unique) file containing instrument data will be renamed adding
     * always the "_1" suffix to the instrument file name used. This is
     * done automatically.
     *
     * Furthermore, no binary files are supported because of the same reason:
     * usually, binary files are provided to save space when big amounts
     * of data must be stored. Instrument constants will never constitute
     * such a big amount of information, so only text file formats are supported.
     * Again, as a consequence, no parameters to configure the text or write
     * modes are provided, since when working with files as the output data
     * channel, text files will be the unique option.
     *
     * Note that once a channel is set, it may no be set again; this means
     * that calling this method twice or more times (or after any other
     * channel setting method has already been invoked) will return an
     * error condition (see the list of returned error codes).
     *
     */

    int  set_data_channel              (const char*  astrolabe_header_file_name,
                                        const char*  instrument_data_file_name,
                                        const lineage_data*
                                                     lineage_info);

    /**
     * \brief Set the data channel to work with sockets.
     *
     * \param host IP address or hostname to write to.
     * \param port Port to be used to write data through the socket channel.
     * \param server_mode When set to true, the socket writer will work in
     *        server mode, that is, will accept a connection instead of
     *        connecting itself. When set to false, the socket writer will
     *        actively connect to some host and port instead of waiting for
     *        someone else to connect.
     *
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The data channel is already set.
     *        - 2: host may not be NULL (when working in client mode).
     *        - 3: port out of range.
     *
     * After an instrument writer has been instantiated, the parameters defining the
     * type of data channel used must be specified.
     *
     * This methods sets the channel as a TCP / IP socket.
     *
     * To define a TCP / IP socket channel both the host (or its IP address)
     * and the port used to write data must be specified when working in
     * CLIENT mode (see the description about parameter server_mode below).
     * On the contrary, if the socket is going to work in SERVER
     * mode, specifying a port number is enough.
     *
     * Port numbers must be in the range [1..65535]. Note, however, that
     * the range [1..1024] is normally reserved to typical application as
     * ftp, telnet or browser applications. so the use of port numbers in
     * this last range is not recommended.
     *
     * The server_mode parameter states how the connection with the other
     * counterpart will start; when server_mode is set to true, the
     * socket will behave as a server (listener) waiting for someone else
     * to connect. In this case the host parameter is not necessary, since
     * a port number suffices to connect in this way.
     *
     * On the contrary, if server_mode is set tot false, this means that
     * the socket will try actively to connect to some server and port,
     * that is, it will behave as a client, expecting that a counterpart
     * socket behaving as a server is ready to accept connections.
     *
     * Note that once a channel is set, it may no be set again; this means
     * that calling this method twice or more times (or after any other
     * channel setting method has already been invoked) will return an
     * error condition (see the list of returned error codes).
     *
     */

    int  set_data_channel              (const char*  host,
                                        int          port,
                                        bool         server_mode);

    /**
     * \brief Write an l-record.
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
     * \param buffer_covariance Array holding the n_covariance covariance values
     *        to write.
     *
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The data channel is not open so it is not possible to write.
     *        - 2: I/O error detected while writing (either to disk or to
     *             a socket connection).
     *        - 3: NOT USED.
     *        - 4: Instrument identifier must be at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: Invalid number of values specified in n_tags (must be >= 0),
     *             n_expectations (must be > 0) or n_covariance_values (0,
     *             n_expectations or (n_expectations*(n_expectations+1))/2.
     *        - 6: NOT USED.
     *          7: NOT USED.
     *
     */
    int  write_l                       (bool         active,
                                        char*        identifier,
                                        int          instance_identifier,
                                        double       time,
                                        int          n_tags,
                                        double*      buffer_tags,
                                        int          n_expectations,
                                        double*      buffer_expectations,
                                        int          n_covariance_values,
                                        double*      buffer_covariance);


  protected:

    /// \brief The chunk files name generator.

    segmented_file_utils            chunk_names_generator_;
  
    /// \brief Flag. When true, indicates that the underlying instrument
    /// channel is open.

    bool                            data_channel_is_open_;

    /// \brief Flag. Used to indicate that the channel parameters have
    /// already been set.

    bool                            data_channel_is_set_;

    /// \brief Flag. Used to ascertain if the input channel is a socket.
    /// When true, we are using sockets, when false, files.

    bool                            data_channel_is_socket_;

    /// \brief Writer for instrument files in text format.

    instrument_file_writer_txt*     file_writer_;

    /// \brief Name of the output astrolabe header file.

    string                          header_file_name_;

    /// \brief Hostname (or IP address) of the server to "write" (send) data.

    string                          host_;

    /// \brief Name of the file containing actual instrument data.

    string                          instrument_file_name_;

    /// \brief The lineage information to write in the header of the
    /// ASTROLABE header file.

    lineage_data*                   lineage_info_;

    /// \brief Port number to use when writing through a sockets channel.

    int                             port_;

    /// \brief Flag stating if the socket has to work as a server (listener)
    /// or not (client).


    bool                            socket_server_mode_;

    /// \brief Socket writer in charge to write instrument data
    /// using a socket conection.

    instrument_socket_writer*       socket_writer_;
};

#endif // INSTRUMENT_WRITER_HPP
