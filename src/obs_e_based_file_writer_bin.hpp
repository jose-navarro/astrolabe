/** \file obs_e_based_file_writer_bin.hpp
  \brief Class defining the interface of binary obs_e_based file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_E_BASED_FILE_WRITER_BIN_HPP
#define OBS_E_BASED_FILE_WRITER_BIN_HPP

#include "obs_e_based_file_writer.hpp"

/**
 * @brief Class defining the interface of binary obs_e_based file writers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * obs_e_based_file_writer, creating binary files as result.
 *
 *
 */
class obs_e_based_file_writer_bin : public obs_e_based_file_writer
{
  public:

    /**
     * @brief Closes the binary file writer, disabling any other
     *        further action on it.
     *
     * Closes the binary file writer, flushing any pending data
     * to the underlying file. Once close is executed, it is not possible
     * to perform any other actions on the writer besides destroying it.
     *
     * @return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the file.
     *        - 2: I/O error while writing data to disk.
     */
    virtual int  close                       (void);

    /**
     * @brief Default constructor.
     *
     */
                 obs_e_based_file_writer_bin (void);

    /**
     * @brief Destructor.
     *
     * Destroys the binary writer. It also closes the underlying file
     * if no close() has been issued by the calling module, in order
     * to guarantee that all data is flushed to disk.
     */
    virtual     ~obs_e_based_file_writer_bin (void);

    /**
     * @brief Write an l-record.
     *
     * @param active The l-record is active (true) or removed (false).
     * @param identifier Identifier of the l-record.
     * @param instance_identifier Instance identifier of the l-record.
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
     * Once a binary writer has been opened using open() it is possible to write
     * either l- or o-records. This method writes an l-record.
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
     * @param identifier Identifier of the o-record.
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
     *        - 4: Parameter code must at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: NOT USED.
     *        - 6: NOT USED.
     *        - 7: Either the number of parameter or observation instance
     *              identifiers are equal to or less than zero. These values
     *              must be strictly positive. Also, this error code may
     *              be issued if the number of instrument instance identifiers
     *              requested is negative (but it may be zero).
     *
     * Once a binary writer has been opened using open() it is possible to write
     * either l- or o-records. This method writes an o-record.
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

    /// @brief Number of bytes accumulated in the current epoch (set of
    /// l- and o- records with the same time tag) never including the
    /// backtracking information

    int    bytes_in_current_block_;

};

#endif // OBS_E_BASED_FILE_WRITER_BIN_HPP
