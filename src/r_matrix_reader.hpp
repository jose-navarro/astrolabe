/** \file r_matrix_reader.hpp
  \brief  Class able to read r_matrix data from all kinds of
          ASTROLABE supported data channels.
*/

#ifndef R_MATRIX_READER_HPP
#define R_MATRIX_READER_HPP

#include "r_matrix_file_reader_bin_backward.hpp"
#include "r_matrix_file_reader_bin_forward.hpp"
#include "r_matrix_file_reader_txt_backward.hpp"
#include "r_matrix_file_reader_txt_forward.hpp"

#include "r_matrix_socket_reader.hpp"

#include "gp_utils.hpp"
#include "gp_astrolabe_header_file_parser.hpp"

#include "segmented_file_utils.hpp"


#include <list>
#include <string>

using namespace std;

/**
   \brief  Class able to read r_matrix data from all kinds of
   ASTROLABE supported data channels.

   \ingroup ASTROLABE_data_IO

   This class is able to read r_matrix data from all the different types of
   data channels defined by the ASTROLABE I/O interface.

   This includes either binary or text files, in forward or backward directions as
   well as TCP / IP sockets (only in forward direction).

   THEREFORE, THIS IS THE CLASS TO USE WHEN READING ASTROLABE R_MATRIX DATA. All other
   classes in this library implement the different readers needed to
   deal with the different kind of ASTROLABE data channels and should never
   be invoked directly.

   The way to use this reader is pretty straightforward:

   - Instantiate the r_matrix_reader object (either automatically or using explicitly
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
     data channel (r_matrix data).
   - Then, a loop must be implemented until the "end-of-file" (or end-of-data)
     condition is detected. Each iteration must read a complete record in
     the following way:
     - Read the type (will always be 'r') of the record to come using read_type().
       This method is the one able to detect an end of data condition, so after
       calling it, this situation must be checked in order to break the loop if
       necessary.
     - Read the active flag using read_active_flag(). Note that this method
       may report that the record being read is not active. No matter what's
       the value of the active flag, the rest of the record must be FULLY
       read; otherwise, errors will arise.
     - Read the time tag for the record. This is done calling read_time().
     - Call read_r_data() to obtain the correlation values included in the
       r-record.
   - Once out of the read loop, call close() to close the reader.
   - Destroy the reader (this is not necessary, of course, if it was
     instantiated automatically).

   Note that all methods in this class are completely independent of the kind of
   output channel (files either text of binary...) being used.

   Note that this class relies on the XTL or eXternalization Template Library
   (http://xtl.sourceforge.net/) which is a portable implementation of the
   XDR standard (http://en.wikipedia.org/wiki/External_Data_Representation).

   This library is used by the r_matrix_reader class to decode the data when
   received via a socket channel, to avoid the problems that arise when different
   computer architectures are involved in the process of sending / receiving
   information. Such problems are endianness and the specific representation
   format choosen for binary data as integer or double values.

   The XDR standard is NOT used when reading or writing from / to file channes;
   only sockets are affected by this technique.

   This reader, therefore, use XTL to decode the information sent by r_matrix_writer
   when the underlying data channel is a socket connection.
*/

class r_matrix_reader
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
    * @brief Checks if the data channel being read is a file.
    * @return True if the data channel in use is a file, false
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
     * @brief Check if the data in the r_matrix channel has been completely read.
     *
     * @return True if all data in the r_matrix channel has already been read,
     *        false otherwise.
     *
     * Use this method to check if the contents of the r_matrix channel has
     * been exhausted (completely read) or not. In other words, check if
     * the "end of the file" has been reached.
     *
     * Strictly speaking, backwards file readers will never reach the end
     * of the file but its beginning, since the read process traverses
     * the file from bottom to top.
     *
     * However, the concept of data exhaustion is valid in all cases
     * (forward and reverse file readers) and this is the method to
     * use in all cases to check if there is anything left to read. That is
     * why an unique method has been provided for this purpose (instead of
     * adding an hypothetical is_bof() or "is beginning of file" to be used
     * only with reverse file readers. In this way, the interface for any
     * reader is kept uniform.
     */

    bool                 is_eof                    (void) const;

    /**
     * @brief Check if the underlying data channel has already been opened.
     *
     * @return True if the data channel has already been opened, false otherwise.
     */

    bool                 is_open                   (void) const;

    /// \brief Constructor.

                         r_matrix_reader           (void);

    /// \brief Destructor

                         ~r_matrix_reader          (void);

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
     *             file because of I/O problems.
     *        - 3: Not enough memory to allocate internal buffers.
     *        - 4: End of file condition detected - the data channel is empty.
     *        - 5: I/O error detected while reading the channel.
     *        - 6: Malformed records found.
     *        - 7: Internal buffer too small to read data.
     *
     * Once a reader has been instantiated, and the metadata included in
     * the input ASTROLABE header file loaded, it is possible to open
     * the underlying data channel to enable actual read operations.
     *
     * This method opens the channel.
     *
     * It is possible to specify that such data channel be read in reverse mode
     * (see parameter reverse_mode).
     *
     */

    int                  open                      (bool reverse_mode=false);

    /**
     * @brief Reads the active flag.
     *
     * @param active Value of the active flag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the r_matrix
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
     * @brief Read correlation matrix values in an r-record.
     *
     * @param n_correlation_values_found The number of correlation values
     *        read from the r-record.
     * @param the_correlation_values Array that will store the correlation
     *        values. The number of actual values read will be stored in parameter
     *        n_correlation_values_found.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the r_matrix data
     *              channel has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. At least one of the correlation values
     *              is not a valid double value.
     *         - 5: Invalid r-record. Several conditions may arise that are
     *              represented by this error code; for instance, an invalid
     *              or missing closing tag might produce this error.
     *
     * This method must be called right after read_time() has been invoked.
     * Calling this method at any other moment will arise an error condition.
     *
     * A successful execution will provide the values of the correlation values
     * stored in the the_correlation_values output parameter.
     *
     */

    virtual int   read_r_data                      (int&     n_correlation_values_found,
                                                    double*& the_correlation_values);


    /**
     * @brief Read the time tag.
     *
     * @param time The time tag.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End of file (no more data).
     *         - 2: I/O error while reading (might happen if the r_matrix
     *              data channel has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Illegal value. The value corresponding to the
     *              time read from the input file is not a valid
     *              double.
     *
     * It must be called right after a call to read_active_flag() has
     * been made.  Using this method in a different moment will arise an
     * error condition.
     *
     * Once run, time will contain the value of the time tag for the
     * record being read.
     *
     */

    int                  read_time                 (double& time);

    /**
     * @brief Read the event record type.
     *
     * @param record_type The type of the record being read, which must
	   *        be always an "r".
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End-of-file (no more data).
     *         - 2: I/O error while reading (might happen if the r_matrix
     *              data channel has not been opened).
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
     *
     */

    int                  read_type                 (char& record_type);

     /**
     * @brief Set the parameters controlling the behaviour of socket
     *        readers, when these are used.
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
     *        - 2: The name of the input ASTROLABE header file has not been set.
     *        - 3: Warnings detected when retrieving the metadata.
     *
     * Once a reader has been instantiated, it is necessary to retrieve the metadata
     * defining the underlying data channel. This must
     * be done before any open() or read operation is started.
     *
     * The ASTROLABE header file may contain warnings or errors. When this happens, an
     * specific error code is returned (see the list of error codes above).
     *
     * Warnings may be ignored and the process of reading the r_matrix data
     * file may also continue. However, it is recommended that these warnings are
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

    r_matrix_based_file_reader*     file_reader_;

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

    /// \brief Name of the XML schema file used to validate the syntax
    /// of the ASTROLABE header file to read. When empty, no schema
    /// is available.

    string                          schema_file_;

    /// \brief The socket reader to use when the underlying channel is
    /// a socket connection.

    r_matrix_socket_reader*         socket_reader_;

    /// \brief When using underlying socket readers, this flag states how
    /// these must behave when connecting: as a server or client socket.

    bool                            socket_works_in_server_mode_;

};

#endif // R_MATRIX_READER_HPP
