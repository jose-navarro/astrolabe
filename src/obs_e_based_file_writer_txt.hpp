/** \file obs_e_based_file_writer_txt.hpp
  \brief Class defining the interface of text obs_e_based file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_E_BASED_FILE_WRITER_TXT_HPP
#define OBS_E_BASED_FILE_WRITER_TXT_HPP

#include "obs_e_based_file_writer.hpp"

//
// Define the type of newline character used depending on the operating
// system where this code is compiled. The first if covers all windows
// operating systems; the elif clause is targetted at Macintosh OS up to
// version 9.0. The last else covers all UNIX and Linux based systems (which
// includes Macintosh OS version 10 and up).
//

/// @brief Definition of the newline character sequence. The actual
/// value for this macro will depend on the operating system and compiler
/// used to build the code.

#ifdef _WIN32
    #define NEWLINE "\r\n"
#elif defined macintosh
    #define NEWLINE "\r"
#else
    #define NEWLINE "\n"
#endif

/**
 * @brief Class defining the interface of text obs_e_based file writers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * obs_e_based_file_writer, creating text files as result.
 *
 * The output is formatted according to ASTROLABE conventions.
 *
 *
 */
class obs_e_based_file_writer_txt : public obs_e_based_file_writer
{
  public:

    /**
     * @brief Closes the text file writer, disabling any other
     *        further action on it.
     *
     * Closes the text file writer, flushing any pending data
     * to the underlying file. Once close is executed, it is not possible
     * to perform any other actions on the writer besides destroying it.
     *
     * @return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the file.
     *        - 2: I/O error while writing data to disk.
     */
    virtual int  close                 (void);

    /**
     * @brief Default constructor.
     *
     */
                 obs_e_based_file_writer_txt (void);

    /**
     * @brief Destructor.
     *
     * Destroys the text writer. It also closes the underlying file
     * if no close() has been issued by the calling module, in order
     * to guarantee that all data is flushed to disk.
     */
    virtual     ~obs_e_based_file_writer_txt (void);

    /**
     * @brief Write an l-record.
     *
     * @param active The l-record is active (true) or removed (false).
     * @param identifier Identifier of the l-record. Must be at least 1 character long
     *             excluding the terminating NULL string.
     * @param instance_identifier Identifier of the l-record.
     * @param time Time tag of the l-record.
     * @param n_tags Number of tag values to write. May be zero if the
     *        l-record being written has no related tags.
     * @param buffer_tags Array holding the n_tags tag values to write.
     * @param n_expectations Dimension of the expectations array, that is,
     *        the number of expectation values to write.
     * @param buffer_expectations Array holding the n_expectations values
     *        to write.
     * @param n_covariance_values Number of covariance matrix elements to
     *        write. May be either 0 (no covariance matrix is written),
     *        n_expectations (that is, only the standard deviations are written)
     *        or (n_expectations * (n_expectations + 1))/2, (the full covariance
     *        matrix, not duplicating simmetric values).
     * @param buffer_covariance The covariance matrix elements to write.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: File is not open so it is not possible to write.
     *        - 2: I/O error detected while writing to disk.
     *        - 3: NOT USED.
     *        - 4: Parameter identifier must be at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: Invalid number of values specified in n_tags (must be >= 0),
     *             n_expectations (must be > 0) or n_covariance_values (0,
     *             n_expectations or (n_expectations*(n_expectations+1))/2.
     *
     * Once a text writer has been opened using open() it is possible to write
     * either l- or o-records. This method writes an l-record, with the
     * format defined by ASTROLABE.
     *
     * Note that the record type ('l' in this case) needs not to be specified.
     * This method writes this type automatically.
     *
     * Note also that the first record in a set of these containing the same
     * time tag must be always an l-record.
     *
     */
    virtual int  write_l                     (bool         active,
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
     * @brief Writes an o-record.
     *
     * @param active The o-record is active (true) or removed (false).
     * @param identifier Identifier of the o-record. Must be at least 1 character long
     *             excluding the terminating NULL string.
     * @param time Time tag of the o-record.
     * @param n_parameter_iids Total number of parameter instance identifiers
     *        to write. Must be greater than 0.
     * @param the_parameter_iids Buffer storing the parameter instance
     *        identifiers to write.
     * @param n_observation_iids Total number of observation instance identifiers
     *        to write. Must be greater than 0.
     * @param the_observation_iids Buffer storing the observation instance
     *        identifiers to write.
     * @param n_instrument_iids Total number of instrument instance identifiers
     *        to write. May be zero.
     * @param the_instrument_iids Buffer storing the instrument instance
     *        identifiers to write. 
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: File is not open so it is not possible to write.
     *        - 2: I/O error detected while writing to disk.
     *        - 3: An attempt to write an o-record as the first record in the
     *             group sharing the same time tag has been detected.
     *        - 4: Parameter identifier must be at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: NOT USED.
     *        - 6: NOT USED.
     *        - 7: Either the number of parameter or observation instance
     *              identifiers are equal to or less than zero. These values
     *              must be strictly positive. Also, this error code may
     *              be issued if the number of instrument instance identifiers
     *              requested is negative (but it may be zero).
     *
     * Once a text writer has been opened using open() it is possible to write
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
     * This method is implemented in desdendant classes only. See
     * the documentation of the specialized classes for details.
     *
     */

    virtual int  write_o                     (bool         active,
                                              char*        identifier,
                                              double       time,
                                              int          n_parameter_iids,
                                              int*         the_parameter_iids,
                                              int          n_observation_iids,
                                              int*         the_observation_iids,
                                              int          n_instrument_iids,
                                              int*         the_instrument_iids) = 0;

  protected:

    /// @brief Default format used to write double values.

    char* format_default_double_format_;

    /// @brief Default format used to write int values.

    char* format_default_int_format_;

    /// @brief Fixed part of the format used for l-records.

    char* format_fixed_l_;

    /// @brief Fixed part of the format used for o-records.

    char* format_fixed_o_;

    /// @brief Size in characters, once written, taken by any (either l or o) tag.
    /// Includes the preceding whitespace, the tag itself. The ending new line
    /// is also tallied; newline characters may be implemented in different
    /// ways (LF, CR, LF+CR, CR + LF) by different operating systems; the right
    /// newline character combination is selected in compilation time and the
    /// size of this combination is computed afterwards automatically.

    int sizeof_closing_tag_;

    /// @brief Size in characters, once written, taken by a single double value
    /// (expectation, standard deviation or auxiliar). Includes the whitespace
    /// preceding the double value itself.

    int sizeof_double_value_;

    /// @brief Size in characters, once written, of the fixed par of the l-record.
    /// Includes opening tag, 1 whitespace, the code (always 3 characters),
    /// 1 whitespace, identifier, 1 whitespace and time tag.

    int sizeof_fixed_l_;

    /// @brief Size in characters, once written, of the fixed part of the
    /// o-record. Includes opening tag, 1 whitespace, the code (always 3
    /// characters), 1 whitespace and time tag.

    int sizeof_fixed_o_;

    /// @brief Size in characters, once written, taken by a single integer value
    /// (identifiers). Includes the whitespace preceding the integer value itself.

    int sizeof_int_value_;
};

#endif // OBS_E_BASED_FILE_WRITER_TXT_HPP
