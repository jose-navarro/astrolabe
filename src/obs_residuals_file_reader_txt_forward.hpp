/** \file obs_residuals_file_reader_txt_forward.hpp
  \brief Class defining the interface of forward text observation residuals file readers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_RESIDUALS_FILE_READER_TXT_FORWARD_HPP
#define OBS_RESIDUALS_FILE_READER_TXT_FORWARD_HPP

#include "obs_e_based_file_reader_txt_forward.hpp"

/**
 * @brief Class defining the interface of forward text observation residuals file readers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * astrolabe_file_reader_txt_forward, reading observation residuals files stored in text format in
 * forward direction.
 *
 */

class obs_residuals_file_reader_txt_forward : public obs_e_based_file_reader_txt_forward
{
  public:

    /**
     * @brief Default constructor.
     */

                 obs_residuals_file_reader_txt_forward  (void);

    /**
     * @brief Destructor.
     */
                 ~obs_residuals_file_reader_txt_forward (void);

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
     *         - 0: NOT USED.
     *         - 1: NOT USED.
     *         - 2: NOT USED.
     *         - 3: Invalid call sequence (see description below about
     *              the proper call sequence).
     *         - 4: NOT USED.
     *         - 5: NOT USED.
     *         - 6: NOT USED.
     *          -7: NOT USED.
     *
     * This method is provided just to comply with the generic interface that
     * all ASTROLABE readers must expose.
     *
     * However, ASTROLABE observation residuals files DO NOT INCLUDE o-records. Therefore,
     * it is not possible to read these.
     *
     * Thus, if this method is ever called, it will always return error code
     * 3 (which, in general, stands for "invalid call sequence"), meaning that
     * it should have never been invoked.
     *
     */

    virtual int  read_o_data                        (int   n_parameter_iids,
                                                     int*& the_parameter_iids,
                                                     int   n_observation_iids,
                                                     int*& the_observation_iids,
                                                     int   n_instrument_iids,
                                                     int*& the_instrument_iids);

    /**
     * @brief Read the time tag.
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
     * This method may be used when reading either l-records, since
     * there exist no o-records in observation residuals files.
     *
     * It must be called right after a call to read_l_id() has been made.
     * Using this method in a different moment will arise an error
     * condition.
     *
     * Once run, time will contain the value of the time tag for the
     * l-record being read.
     *
     */

    virtual int  read_time                          (double& time);

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
     *         - 4: Malformed record found (including o tags, invalid in
     *              observation residuals files).
     *         - 5: Unexpected end of file. Invalid file. This error code
     *              differs from code 1 in the sense that for code 1,
     *              absolutely no data was found in the file when trying
     *              to read a new type tag; this means that the end
     *              of the file has been legally reached. On the contrary,
     *              code 5 means that some data could be read before all
     *              the needed information could be retrieved; this points
     *              to a corrupted file.
     *         - 6: NOT USED.
     *
     * read_type() is the method that starts the process of reading a
     * full observation residuals l-record.
     *
     * It may be invoked right after open() or read_l_data() have been
     * called only. Using this method at any other moment will produce
     * an error condition.
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
     * event_file_reader for an example on how to check and treat this situation.
     *
     * After a successful completion, the record_type parameter will contain
     * either an 'l' character (always lowercase) denoting the
     * type of record being read (l-records).
     */
    virtual int  read_type                          (char& record_type);

};

#endif // OBS_RESIDUALS_FILE_READER_TXT_FORWARD_HPP
