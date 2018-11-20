/** \file observation_file_reader_txt_backward.hpp
  \brief Class defining the interface of backwards observation text file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBSERVATION_FILE_READER_TXT_BACKWARD_HPP
#define OBSERVATION_FILE_READER_TXT_BACKWARD_HPP

#include "obs_e_based_file_reader_txt.hpp"

/**
 * @brief Class defining the interface of backwards observation text file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * obs_e_based_file_reader_txt, reading observation files stored in text format in
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

class observation_file_reader_txt_backward : public obs_e_based_file_reader_txt
{
  public:

    /**
     * @brief Default constructor.
     */

                 observation_file_reader_txt_backward  (void);

    /**
     * @brief Destructor.
     */
                 ~observation_file_reader_txt_backward (void);

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
     *        - 6: Malformed records found.
     *        - 7: Internal buffer too small.
     *
     * Once a reader has been instantiated (and, optionally, the
     * size of the internal buffer set using set_buffer_size()), it is
     * possible to open the underlying file to enable I/O operations.
     *
     * This method opens the file and instantiates the internal memory
     * buffer used to speed up I/O operations, enabling the different
     * read operations available in this class.
     *
     * Note that in the specific case of this reader, some additional operation
     * are performed. Once the file is open, this method reads the last
     * part of the file (as many bytes as the size of the internal buffer) and
     * tries to analyze its contents to prepare things for the rest of the
     * methods in this library.
     *
     * During this process, it may happen that malformed records be detected
     * (error code 6) or that the size of the internal buffer proves to be
     * too small to perform such analysis (error code 7).
     */

    virtual int  open                                  (const char*  file_name);

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
     *          -7: NOT USED.
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
     * Note that even when the set of identifiers has been correctly parsed,
     * still an additional error may arise: the remaining characters in the
     * record after these integer values may be invalid. After these values,
     * only whitespace and a legal o-record closing tag may be found. If this
     * is not the case, an error condition is arised via return code 5.
     *
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
     * to read_l_id() has been made; on the contrary, if an o-record
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

    virtual int  read_time                             (double& time);

    /**
     * @brief Read the event record type.
     *
     * @param record_type The type of the record being read.
     * @return Error code. Error code values:
     *         - 0: Successful completion.
     *         - 1: End-of-file (no more data).
     *         - 2: I/O error while reading (might happen if the file
     *              has not been opened).
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: Malformed record found.
     *         - 5: NOT USED.
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
     * @brief Locates the position where the first complete epoch starts. The
     *        search is made in the internal data buffer.
     * @param start The position in the internal data buffer where the search
     *        should start.
     * @param position If the first epoch is found, this contains the absolute
     *        position from the beginning of the internal buffer where such
     *        epoch is located.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Buffer exhausted, first epoch not found.
     *        - 2: Malformed record found.
     *        - 3: Buffer is empty, it contains whitespace only.
     *
     * This method scans the internal data buffer in order to locate the
     * position where the first COMPLETE epoch is located.
     *
     * An epoch is the set of l- and o-records that have the same time tag.
     * Epochs always start with an l-record, which helps to locate their
     * beginning.
     *
     * When the first epoch is found, its starting position in the data buffer
     * is copied to parameter position. Note (1) that this position is absolute,
     * that is, referred to the beginning of the internal buffer and (2) that
     * the beginning of an epoch is the first opening '<' found in such
     * epoch.
     *
     * If it is not possible to find a complete epoch in the buffer, an error
     * code is returned.
     */

    int          find_first_epoch                      (int start, int& position);

    /**
     * @brief Computes the starting and ending positions of the next
     *        record to be read.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: End of file (no more data).
     *        - 2: Malformed record found.
     *        - 3: I/O error detected while reading the file.
     *        - 4: Buffer too small.
     *
     * Computes the starting and ending positions of the next record within
     * the current epoch.
     *
     * If the current epoch is exhausted, it makes find_prev_epoch() to look
     * for the previous one inside the internal data buffer and, once the
     * new epoch is identified, the limits of its first record are computed.
     *
     * Note that calling find_prev_epoch() may trigger an I/O operation if
     * the internal data buffer is already exhausted. This, also, may lead
     * to an end-of-file condition as well as other errors related to the
     * identification of the first epoch in the chunk of data read.
     *
     * Record limits are set using members cur_record_start_, cur_record_end_
     * and cur_record_cur_;
     */

    int          find_next_record_limits               (void);

    /**
     * @brief Computes the pointers that define the area where the previous
     *        unprocessed epoch is stored. Computes also the number of full
     *        records included in this epoch. The internal data buffer is
     *        searched BACKWARDS.
     * @param start Absolute position in the internal data buffer where the
     *        search for the previous epoch starts.
     * @param epoch_first Absolute position in the internal data buffer where
     *        the previous epoch starts.
     * @param epoch_last Absolute position in the internal data buffer where
     *        the previous epoch ends.
     * @param record_first Absolute position in the internal data buffer
     *        where the first record in the epoch starts.
     * @param record_last Absolute position in the internal data buffer where
     *        the first record in the epoch ends.
     * @param total_records Number of records included in the previous epoch.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Malformed records found (invalid file).
     */
    int          find_prev_epoch                       (int  start,
                                                        int& epoch_first , int& epoch_last,
                                                        int& record_first, int& record_last,
                                                        int& total_records);

    /**
     * @brief Locates a record boundary, that is a sequence given by an
     *        opening <, plus whitespace only and then an opening <.
     * @param from Absolute position in the internal data buffer where to start
     *        the search from.
     * @param next_opening The position in the internal data buffer where
     *        the first opening < is found.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Buffer exhausted while looking for the record.
     */
    int          find_record_boundary                  (int from, int& next_opening);

    /**
     * @brief Looks for the absolute limits of a complete event record, either
     *        l- or o- and its type. The search is performed BACKWARDS.
     * @param from Absolute position in the internal data buffer where to start
     *        the search from.
     * @param start Absolute position of the beginning of the record.
     * @param end Absolute position of the end of the record.
     * @param type Type of the record found.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Buffer exhausted while looking for the record.
     *        - 2: Malformed record found.
     *
     * The search finishes either successfully when a complete record is found.
     * An unsuccessful search implies a bad record found in the buffer.
     */
    int          find_record_bwd                       (int from,  int& start, int& end, char& type);

    /**
     * @brief Looks for the absolute limits of a complete event record, either
     *        l- or o- and its type. The search is performed FORWARDS.
     * @param from Absolute position in the internal data buffer where to start
     *        the search from.
     * @param start Absolute position of the beginning of the record.
     * @param end Absolute position of the end of the record.
     * @param type Type of the record found.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Buffer exhausted while looking for the record.
     *        - 2: Malformed record found.
     *
     * The search finishes either successfully when a complete record is found
     * or unsuccessfully if the end of the internal data buffer is reached.
     */
    int          find_record_fwd                       (int from,  int& start, int& end, char& type);

    /**
     * @brief Locates the time tag in a record (either l- or o-) and parses
     *        its time tag.
     * @param record_type Type of the record to be sought.
     * @param record_start Position in the data buffer where the record to be
     *        sought starts.
     * @param record_end Position in the data buffer where the record to be
     *        sought ends.
     * @param timetag Value of the time tag.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Time tag not found (invalid record).
     *
     * Given the first and last positions (in the internal data buffer) of a
     * complete record (either l- or o-), this method looks for the time tag
     * (in text format) and converts its to its corresponding double
     * representation.
     */

    int          find_timetag                          (char    record_type,
                                                        int     record_start, int record_end,
                                                        double& timetag);

    /**
     * @brief Read as many bytes as possible from the input file to fill the
     *        data buffer. The read process is performed in backwards direction.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: Beginning of file (no more data).
     *        - 2: I/O error while reading (might happen if the file
     *             has not been opened).
     *        - 3: Malformed record found.
     *        - 4: Buffer too small to hold required data; unable to find
     *             the first epoch in the data chunk read because of this.
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
     * So, going back to the previous example, if start amounts to 15, the
     * new fseek position is computed as (1000 - (300-15)) = 715, and the
     * actual amount of bytes read is 300 - 15 = 285.
     *
     * Once the actual read process finishes, the new fseek position becomes
     * the current one, thus being ready for a new call to this method.
     *
     * Remember: the data already stored in positions 0..start_at of the buffer
     * is left undisturbed.
     */

    int          read_prev_chunk                       (void);

  protected:


    /// @brief Position in the data buffer where actual data starts (due to
    /// the backwards nature of the read process, the absolute beginning of
    /// the data buffer may not store actual data but just garbage).

    int          chunk_start_index_;

    /// @brief Pointer to the first non-processed position (data not yet
    /// "read") in the data buffer.

    int          data_buffer_current_;

    /// @brief Position in the data buffer where the epoch being processed
    /// ends.

    int          epoch_end_;


    /// @brief Number of records remaining in the epoch being processed.

    int          epoch_remaining_records_;

    /// @brief Position in the data buffer where the epoch being processed
    /// starts.

    int          epoch_start_;

    /// @brief Number of bytes in the input file that have not been read yet.

    int          file_bytes_unread_;
};

#endif // OBSERVATION_FILE_READER_TXT_BACKWARD_HPP
