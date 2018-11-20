/** \file obs_residuals_file_writer_bin.hpp
  \brief Class defining the interface of binary observation residuals file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_RESIDUALS_FILE_WRITER_BIN_HPP
#define OBS_RESIDUALS_FILE_WRITER_BIN_HPP

#include "obs_e_based_file_writer_bin.hpp"

/**
 * @brief Class defining the interface of binary observation residuals file writers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements the interface defined in parent class
 * astrolabe_file_writer_bin, creating binary files as result.
 *
 */
class obs_residuals_file_writer_bin : public obs_e_based_file_writer_bin
{
  public:

    

    /**
     * @brief Default constructor.
     *
     */
                 obs_residuals_file_writer_bin (void);

    /**
     * @brief Destructor.
     */
    virtual     ~obs_residuals_file_writer_bin (void);


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
     *        - 0: NOT USED.
     *        - 1: NOT USED.
     *        - 2: NOT USED.
     *        - 3: An attempt to write an o-record has been detected.
     *        - 4: NOT USED.
     *        - 5: NOT USED.
     *        - 6: NOT USED.
     *        - 7: NOT USED.
     *
     * This method is provided just to comply with the generic interface that
     * all ASTROLABE writers must expose.
     *
     * However, ASTROLABE observation residuals files DO NOT INCLUDE o-records. Therefore,
     * it is not possible to write these.
     *
     * Thus, if this method is ever called, it will always return error code
     * 3 (which, in general, stands for "An attempt to write an o-record under some
     * circumnstances", whatever these be), meaning that it should have never been invoked.
     *
     */

    virtual int  write_o                       (bool         active,
                                                char*        identifier,
                                                double       time,
                                                int          n_parameter_iids,
                                                int*         the_parameter_iids,
                                                int          n_observation_iids,
                                                int*         the_observation_iids,
                                                int          n_instrument_iids,
                                                int*         the_instrument_iids);

};

#endif // OBS_RESIDUALS_FILE_WRITER_BIN_HPP
