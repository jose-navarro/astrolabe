#ifndef RECORDSTRUCTURE_HPP
#define RECORDSTRUCTURE_HPP

#include <string>
#include <map>

using namespace std;

/**
 * @brief Class simulating the structural information of
 *        a few obs-e based l-records.
 *
 * This class define the structural properties of a few
 * obs-e based l-records
 *
 * Such structural properties are the following:
 *
 * - the value of the identifier,
 * - the number of tag values,
 * - the dimension of the observation array (number of expectation
 *   values). This, indirectly, also sets the dimension of the
 *   covariance matrix related to the observation.
 *
 */


class RecordStructure
{
  public:

    /// @brief Default constructor.

    RecordStructure (void);

    /**
     * @brief Return the structure of an obs-e based l-record given
     * its identifier.
     *
     * @param id The identifier of the obs-e l-record whose structural
     *        information is to be sought.
     * @param n_tags Number of tags for this obs-e record.
     * @param n_expectation Number of expectations for this obs-e record.
     *
     * @return True if the identifier id corresponds to an existing
     *         obs-e record, false otherwise.
     *
     * The output values of n_tags and n_expectations only make sense
     * when this method returns true (that is, the identifier sought
     * do exist).
     *
     */

    bool l_record_data (const char* id, int& n_tags, int& n_expectations);

    /**
     * @brief Return the structure of an obs-e based l-record given
     * its internal index.
     *
     * @param index The internal index of the obs-e based l-record whose 
     *        structural information is to be sought.
     * @param id The identifier related to this obs-e based l-record.
     * @param n_tags Number of tags for this obs-e based l-record.
     * @param n_expectation Number of expectations for this obs-e based l-record.
     *
     * @return True if the identifier id corresponds to an existing
     *         obs-e based l-record, false otherwise.
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
     * @brief Get the number of different obs-e based l-record types simulated
     *        by this class.
     * @return The number of different l-record types simulated by
     *        this class.
     */

    int  n_l_record_types (void);

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
};

#endif // RECORDSTRUCTURE_HPP
