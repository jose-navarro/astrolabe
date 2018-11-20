#ifndef RECORDSTRUCTURE_HPP
#define RECORDSTRUCTURE_HPP

#include <string>
#include <map>

using namespace std;

/**
 * @brief Class simulating the structural information of
 *        a few observations (l-records) and observation
 *        equations (o-records).
 *
 * This class define the structural properties of a few
 * observations and observation equations.
 *
 * Such structural properties are the following:
 *
 * For observations (l-records),
 *
 * - the value of the identifier,
 * - the number of tag values,
 * - the dimension of the observation array (number of expectation
 *   values). This, indirectly, also sets the dimension of the
 *   covariance matrix related to the observation.
 *
 * For observation equations (o-records),
 *
 * - the value of the identifier,
 * - the number of parameter instance identifiers it needs,
 * - the same for observation instance identifiers and
 * - once more, such number but for instrument identifiers.
 */


class RecordStructure
{
  public:

    /// @brief Default constructor.

    RecordStructure (void);

    /**
     * @brief Return the structure of an observation (l-record) given
     * its identifier.
     *
     * @param id The identifier of the observation whose structural
     *        information is to be sought.
     * @param n_tags Number of tags for this observation.
     * @param n_expectation Number of expectations for this observation.
     *
     * @return True if the identifier id corresponds to an existing
     *         observation, false otherwise.
     *
     * The output values of n_tags and n_expectations only make sense
     * when this method returns true (that is, the identifier sought
     * do exist).
     *
     */

    bool l_record_data (const char* id, int& n_tags, int& n_expectations);

    /**
     * @brief Return the structure of an observation (l-record) given
     * its internal index.
     *
     * @param index The internal index of the observation whose structural
     *        information is to be sought.
     * @param id The identifier related to this observation.
     * @param n_tags Number of tags for this observation.
     * @param n_expectation Number of expectations for this observation.
     *
     * @return True if the identifier id corresponds to an existing
     *         observation, false otherwise.
     *
     * The output values of n_tags and n_expectations only make sense
     * when this method returns true (that is, the index sought
     * do exist).
     *
     * The internal index is just a number between 0 and
     * number_of_l_record_types()-1. These indexes are used to refer to the
     * observations internally.
     *
     */

    bool l_record_data (int index, string& id, int& n_tags, int& n_expectations);

    /**
     * @brief Get the number of different l record types simulated
     *        by this class.
     * @return The number of different l-record types simulated by
     *        this class.
     */

    int  n_l_record_types (void);

    /**
     * @brief Get the number of different o record types simulated
     *        by this class.
     * @return The number of different o-record types simulated by
     *        this class.
     */

    int  n_o_record_types (void);

    /**
     * @brief Return the structure of an observation equation (o-record) given
     * its identifier.
     *
     * @param id The identifier of the observation whose structural
     *        information is to be sought.
     * @param n_par_iids Number of parameter instance identifiers in this
     *        observation equation.
     * @param n_obs_iids Number of observation instance identifiers in this
     *        observation equation.
     * @param n_ins_iids Number of instrument instance identifiers in this
     *        observation equation.
     *
     * @return True if the identifier id corresponds to an existing
     *         observation, false otherwise.
     *
     * The output values of n_par_iids, n_obs_iids and n_ins_iids only
     * make sense when this method returns true (that is, the identifier
     * sought do exist).
     *
     */

    bool o_record_data (const char* id,
                        int& n_par_iids, int& n_obs_iids, int& n_ins_iids);

    /**
     * @brief Return the structure of an observation equation (o-record) given
     * its identifier.
     *
     * @param index The internal index of the observation whose structural
     *        information is to be sought.
     * @param id The identifier related to the observation equation.
     * @param n_par_iids Number of parameter instance identifiers in this
     *        observation equation.
     * @param n_obs_iids Number of observation instance identifiers in this
     *        observation equation.
     * @param n_ins_iids Number of instrument instance identifiers in this
     *        observation equation.
     *
     * @return True if the identifier id corresponds to an existing
     *         observation, false otherwise.
     *
     * The output values of n_par_iids, n_obs_iids and n_ins_iids only
     * make sense when this method returns true (that is, the identifier
     * sought do exist).
     *
     * The internal index is just a number between 0 and
     * number_of_o_record_types()-1. These indexes are used to refer to the
     * observation equations internally.
     *
     */

    bool o_record_data (int index, string& id,
                        int& n_par_iids, int& n_obs_iids, int& n_ins_iids);

  protected:

    /// \brief Observations: number of observations modeled.

    int               l_records_available_;

    /// \brief Observations: identifiers.

    string            l_identifiers_[4];

    /// \brief Observations: number of tags.

    int               l_n_tags_[4];

    ///
    /// \brief Observations: dimension of the observation array, or
    /// number of expectation values.
    ///

    int               l_n_expectations_[4];

    ///
    /// \brief Observations: map to go from an observation identifier
    /// to the index in the arrays that defines it.
    ///

    map<string, int>  lid_to_index;

    /// \brief Observation equations: number of observation equations modeled.

    int               o_records_available_;

    /// \brief Observation equations: identifiers.

    string            o_identifiers_[2];

    /// \brief Observation equations: number of parameter instance ids.

    int               o_n_par_iids_[2];

    /// \brief Observation equations: number of observation instance ids.

    int               o_n_obs_iids_[2];

    /// \brief Observation equations: number of instrument instance ids.

    int               o_n_ins_iids_[2];

    ///
    /// \brief Observation equations: map to go from an observation equation
    /// identifier to the index in the arrays that defines it.
    ///

    map<string, int>  oid_to_index;
};

#endif // RECORDSTRUCTURE_HPP
