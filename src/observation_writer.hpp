/** \file observation_writer.hpp
  \brief  Class able to write observation-event data to all kinds of
          ASTROLABE supported data channels.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBSERVATION_WRITER_HPP
#define OBSERVATION_WRITER_HPP

#include "observation_socket_writer.hpp"
#include "observation_file_writer_bin.hpp"
#include "observation_file_writer_txt.hpp"

#include "astrolabe_header_file_writer.hpp"

#include "segmented_file_utils.hpp"
#include "lineage_data.hpp"


/**
   \brief  Class able to write observation-event data to all kinds of
   ASTROLABE supported data channels.

   \ingroup ASTROLABE_data_IO

   This class is able to write to all the different types of data channels
   defined by the ASTROLABE I/O interface.

   This includes both sockets and files; files may be either binary or
   text.

   THEREFORE, THIS IS THE CLASS TO USE WHEN WRITING ASTROLABE DATA. All other
   classes in this library implement the different writers needed to
   deal with the different kind of ASTROLABE data channels and should never
   be invoked directly.

   The way to use this writer is pretty straightforward:

   - Set the kind of output channel depending on its type (file or socket). See
     the different set_data_channel() methods available to do so.
   - Open the channel using method open().
   - Write all the needed information to the channel using methods write_l()
     and write_o() repeatedly until data is exhausted and then,
     close the channel using method close().

   Note that all methods but set_data_channel() are completely independent
   of the kind of output channel being used.

   This class also implements the restrictions that apply on how l- and o- records
   must be written. For instance, new epochs must start with an l-record. Violating
   such requisite will be reported by the proper error code.

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

class observation_writer
{
  public:

    /**
     * \brief Closes the observation writer, disabling any other
     *        further action on it.
     * \return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the data channel.
     *        - 2: I/O error while writing data to disk.
     */

    int  close                         (void);

    /**
     * \brief Check if the underlying data channel has already been opened.
     *
     * \return True if the data channe has already been opened, false otherwise.
     */

    bool is_open                       (void) const;

    /**
     * \brief Default constructor.
     */
    observation_writer                  (void);

    /**
     * \brief Destructor.
     */
    ~observation_writer                  (void);

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
     * write operations available in this class (see write_l() and
     * write_o(), as well as close()).
     *
     * Note that open() may be called only once and just after either
     * of the methods set_data_channel has been invoked.
     *
     */

    int  open                          (void);

    /**
     * \brief Set the observation channel to work with files
     *
     * \param astrolabe_header_file_name Name (may include the full path) of the
     *        astrolabe header file to write that will describe how data is
     *        organized.
     * \param external_base_file_name Base name from which will be derived the
     *        names of the different file segments or chunks where data will
     *        be actually stored.
     * \param binary When true, it indicates that the file used to write data
     *        will be a binary one. When false, this file will be a text file.
     * \param max_epochs_to_split Maximum number of epochs to write in any of
     *        the external files containing actual data. Must be greater than zero.
     * \param file_suffix_width Number of digits to use in the suffix part
     *        of the names of the different external files into which data is
     *        written. Range: [1..9].
     * \param lineage_info The lineage of the astrolabe header file, where information
     *        about (for instance) authors, project, task, etc. is written.
     *
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The data channel is already set.
     *        - 2: The name of output astrolabe header file may not be NULL.
     *        - 3: The base file name for external file segments is NULL.
     *        - 4: The maximum number of epochs to write in the external files
     *             must be greater than zero.
     *        - 5: The width of the numeric suffix used to name external (splitted)
     *             files must be greater than 0 and less than or equal to 9.
     *
     * After an observation writer has been instantiated, the parameters defining the
     * type of data channel used must be specified.
     *
     * This methods sets the output channel as a file , either text or binary.
     *
     * Parameter binary, when set to true, is used to indicate that output data
     * must be written in binary form. When set to false, data is written in
     * text form instead.
     *
     * Data is written to one or more external files. These files are limited
     * in size according to the value of parameter max\_epochs\_to\_split.
     *
     * When a writer starts writing records, it keeps track of how many
     * epochs have been stored in the output file. When this number reaches
     * the maximum number of epochs stated in max\_epochs\_to\_split, the current
     * external file is closed and a new one is created; is in this new file where
     * data will be stored now. This process is repeated until no more data
     * needs to be written.
     *
     * These files need to be given a name automatically. To do it, the
     * file name given in parameter external\_base\_file\_name is used as the base
     * to derive the names of the actual files containing actual data.
     *
     * To tell apart the different sub-files or "chunks", a numeric suffix
     * is added (is ascending order, starting at 1) to that base name.
     * Parameter file\_suffix\_width\ indicates the number of digits to be
     * used to build such suffix. For instance, if this parameter is
     * set to 2, the numeric suffixes appended to the base name will
     * be "_01", "_02", ... , "_99".
     *
     * The numeric suffix is appended right before the dot used to
     * specify the extension of the file (for instance, if the base
     * name specified in external\_base\_file\_name is "myfile.dat", a valid
     * suffixed file name would be "myfile_03.dat"). When no extension
     * is provided, the suffix is just appended at the end of the
     * base file name.
     *
     * Note that the number of digits allowed in parameter
     * file\_suffix\_width imposes a limit to the maximum number of
     * "chunk" files that may be generated. For instance, a width
     * of 2 digits implies that no more than 99 (1..99) chunks may
     * be created. If such width were 3, then up to 999 chunks may
     * be given a name (1...999).
     *
     * Therefore, it is highly recommended to use as many digits as
     * possible for the numeric suffix width. Using, for instance,
     * the maximum allowed file suffix width (9) will allow up to
     * 999999999 files to be created (1 .. 999999999).
     *
     * Note that once a channel is set, it may no be set again; this means
     * that calling this method twice or more times (or after any other
     * channel setting method has already been invoked) will return an
     * error condition (see the list of returned error codes).
     *
     */

    int  set_data_channel              (const char*  astrolabe_header_file_name,
                                        const char*  external_base_file_name,
                                        bool         binary,
                                        int          max_epochs_to_split,
                                        int          file_suffix_width,
                                        const lineage_data*
                                                     lineage_info);

    /**
     * \brief Set the observation channel to work with sockets.
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
     * After an observation writer has been instantiated, the parameters defining the
     * type of data channel used must be specified.
     *
     * This methods sets the channel as a TCP / IP socket.
     *
     * To define a TCP / IP socket channel both the host (or its IP address)
     * and the port used to write data must be specified when working in
     * CLIENT mode (see the description about parameter server_mode below).
     * On the contrary, the socket is going to work in SERVER
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
     *        - 4: Parameter identifier must be at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: Invalid number of values specified in n_tags (must be >= 0),
     *             n_expectations (must be > 0) or n_covariance_values (0,
     *             n_expectations or (n_expectations*(n_expectations+1))/2.
     *        - 6: Error writing: a new chunk file had to be created to
     *             store the new l-record, but the width of the numeric
     *             suffix is not big enough to allow for more chunk
     *             file names.
     *          7: I/O error: unable to create a new file chunk.
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

    /**
     * \brief Writes an o-record.
     *
     * \param active The o-record is active (true) or removed (false).
     * \param identifier Identifier of the o-record. Must be at least 1 character long
     *             excluding the terminating NULL string.
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
     *        - 1: The data channel is not open so it is not possible to write.
     *        - 2: I/O error detected while writing (either to to disk or
     *             to a socket connection).
     *        - 3: An attempt to write an o-record as the first record in the
     *             group sharing the same time tag has been detected.
     *        - 4: Parameter identifier must at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: Error writing: a new chunk file had to be created to
     *             store the new o-record, but the width of the numeric
     *             suffix is not big enough to allow for more chunk
     *             file names.
     *          6: I/O error: unable to create a new file chunk.
     *
     * Once a writer has been opened using open() it is possible to write
     * either l- or o-records. This method writes an o-record, with the format
     * defined by ASTROLABE.
     *
     * Note that the record type ('o' in this case) needs not to be specified.
     * This method writes this type automatically.
     *
     * Note also that the first record in a set of these containing the same
     * time tag must be always an l-record. Otherwise, an error condition will
     * be raised (see list of error codes).
     *
     * The number of elements in the different data arrays must be coherent
     * with the number of elements stated in the different dimensioning
     * parameters (n_parameter_iids, n_observation_iids and n_instrument_iids).
     *
     */

    int  write_o                       (bool         active,
                                        char*        identifier,
                                        double       time,
                                        int          n_parameter_iids,
                                        int*         the_parameter_iids,
                                        int          n_observation_iids,
                                        int*         the_observation_iids,
                                        int          n_instrument_iids,
                                        int*         the_instrument_iids);

  protected:

    /// \brief The chunk files name generator.

    segmented_file_utils            chunk_names_generator_;

    /// \brief The number of epochs already written to the current
    /// chunk.

    int                             already_written_epochs_;

    /// \brief Flag. When true, indicates that the underlying data
    /// channel is open.

    bool                            data_channel_is_open_;

    /// \brief Flag. Used to indicate that the channel parameters have
    /// already been set.

    bool                            data_channel_is_set_;

    /// \brief Flag. Used to ascertain if the input channel is a socket.
    /// When true, we are using sockets, when false, files.

    bool                            data_channel_is_socket_;

    /// \brief Base name of the external files written when working in
    /// external mode.

    string                          external_base_file_;

    /// \brief Flag that indicates that a file data channel must be
    /// written in binary mode.

    bool                            file_mode_is_binary_;

    /// \brief The polymorphic file writer that will be in charge of
    /// write data to files whatever its format is.

    obs_e_based_file_writer*        file_writer_;

    /// \brief Flag used to indicate that no writes operations have been
    /// executed yet (either write_l() or write_o()) and that therefore
    /// there exist no "previous" time tag to detect an epoch change.

    bool                            first_time_writing_;

    /// \brief Name of the output astrolabe header file.

    string                          header_file_name_;

    /// \brief Hostname (or IP address) of the server to "write" (send) data.

    string                          host_;

    /// @brief Value of the current time tag. Used to detect new epochs
    /// (set of l- and o-records with the same time tag).

    double                          last_time_tag_;

    /// \brief The lineage information to write in the header of the
    /// ASTROLABE header file.

    lineage_data*                   lineage_info_;

    /// \brief Maximum number of epochs to write in a single chunk file.

    int                             max_epochs_per_chunk_;

    /// \brief Width in characters of the numeric suffix used to identify
    /// the different chunk file names.

    int                             numeric_suffix_width_;

    /// \brief Port number to use when writing through a sockets channel.

    int                             port_;

    /// \brief Flag stating if the socket has to work as a server (listener)
    /// or not (client).


    bool                            socket_server_mode_;

    /// \brief Socket writer in charge to write observation data
    /// using a socket conection.

    observation_socket_writer*      socket_writer_;
};

#endif // OBSERVATION_WRITER_HPP
