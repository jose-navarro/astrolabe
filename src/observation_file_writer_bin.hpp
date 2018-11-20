/** \file observation_file_writer_bin.hpp
  \brief Class defining the interface of observation binary file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBSERVATION_FILE_WRITER_BIN_HPP
#define OBSERVATION_FILE_WRITER_BIN_HPP

#include "obs_e_based_file_writer_bin.hpp"

/**
 * @brief Class defining the interface of observation binary file writers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * astrolabe_file_writer_bin, creating binary files as result.
 *
 */
class observation_file_writer_bin : public obs_e_based_file_writer_bin
{
  public:

    

    /**
     * @brief Default constructor.
     *
     */
                 observation_file_writer_bin (void);

    /**
     * @brief Destructor.
     *
     * Destroys the binary writer. It also closes the underlying file
     * if no close() has been issued by the calling module, in order
     * to guarantee that all data is flushed to disk.
     */
    virtual     ~observation_file_writer_bin (void);


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
     */

    virtual int  write_o                     (bool         active,
                                              char*        identifier,
                                              double       time,
                                              int          n_parameter_iids,
                                              int*         the_parameter_iids,
                                              int          n_observation_iids,
                                              int*         the_observation_iids,
                                              int          n_instrument_iids,
                                              int*         the_instrument_iids);

};

#endif // OBSERVATION_FILE_WRITER_BIN_HPP
