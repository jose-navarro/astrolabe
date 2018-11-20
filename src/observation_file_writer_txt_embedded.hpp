/** \file observation_file_writer_txt_embedded.hpp
  \brief Class defining the interface of embedded observation text file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBSERVATION_FILE_WRITER_TXT_EMBEDDED_HPP
#define OBSERVATION_FILE_WRITER_TXT_EMBEDDED_HPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * \brief Class defining the interface of embedded observation text file writers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class implements embedded text writers, that is, those that
 * write observation-event data in the same file than the heeader
 * describing these data.
 *
 * Ths class does not inherit from the abstract class observation_file_writer, since
 * the interface it implements is slightly different to the rest of writers.
 *
 * In fact, the file to which data is written is never open by objects
 * of this class but by the calling module responsible for writing the
 * header data of the observation-events file.
 *
 * Therefore, the logical sequence to follow when writing embedded text files
 * is:
 *
 * - The calling module actually opens the observation-event file using
 *   a FILE* variable.
 * - Then it writes the header (up to the line where the kind of device
 *   used to define how actual data is stored).
 * - Afterwards, the calling module instantiates an observation\_file\_writer\_txt\_embedded
 *   object.
 * - To link the actual FILE* opened by the calling module to the writer object,
 *   it must use method attach() to pass that file. Then, the writer knows
 *   where to write data. attach() is the equivalent of method open() in other
 *   writer classes.
 * - Methods write_l() and write_o() may then be used by the calling module to write
 *   l- and o- records, as it is done by other writer classes in this library.
 * - Once the writing is over, the calling module must detach() the file (this is
 *   the equivalent of close() in other writer classes).
 * - Then, the calling module must take care of closing the header and the FILE*
 *   itself.
 *
 * Note that all the restrictions concerning how data is written apply to this
 * writer too. For instance, epochs must start with l-records.
 *
 * The output is formatted according to ASTROLABE conventions.
 *
 */

class observation_file_writer_txt_embedded
{
  public:

     /**
     * \brief Attach the embedded observation text file writer to the actual file
     * to which data will be actually written, enabling I/O operations.
     *
     * \param file_to_attach Pointer to an writable open text file where data
     *        is going to be written. May not be NULL.
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The pointer to the file to attach may not be null.
     *        - 2: File is already attached.
     *
     * Attaches the embedded observation text file writer to the actual file
     * to which data will be written. Once attach() is invoked, it is possible
     * to execute the I/O methods offered by this class.
     *
     */
     int  attach                                (FILE* file_to_attach);

    /**
     * \brief Detach the embedded observation text file writer, disabling any other
     *        further action on it.
     *
     * Detaches the embedded observation text file writer from the actual file
     * to which data is written. Once detach is executed, it is not possible
     * to perform any other actions on the writer besides destroying it.
     *
     */
     void detach                                (void);

    /**
     * \brief Default constructor.
     *
     */
           observation_file_writer_txt_embedded (void);

    /**
     * \brief Destructor.
     *
     * Destroys the text writer. It also detaches the underlying file
     * if no detach() has been issued by the calling module.
     *
     */
          ~observation_file_writer_txt_embedded (void);

    /**
     * \brief Write an l-record.
     *
     * \param active The l-record is active (true) or removed (false).
     * \param identifier Identifier of the l-record. Must be at least 1 character long
     *             excluding the terminating NULL string.
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
     * \param buffer_covariance Array holding the n_covariance values
     *        to write.
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: File is not attached so it is not possible to write.
     *        - 2: NOT USED.
     *        - 3: NOT USED.
     *        - 4: Parameter identifier must be at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: Invalid number of values specified in n_tags (must be >= 0),
     *             n_expectations (must be > 0) or n_covariance_values (0,
     *             n_expectations or (n_expectations*(n_expectations+1))/2.
     *
     * Once that a file has been attached using attach() it is possible to write
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

     int  write_l                               (bool         active,
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
     * \return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: File is not attached, so it is not possible to write.
     *        - 2: NOT USED.
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
     *
     * Once a file has been attached using attach() it is possible to write
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

     int  write_o                               (bool         active,
                                                 char*        identifier,
                                                 double       time,
                                                 int          n_parameter_iids,
                                                 int*         the_parameter_iids,
                                                 int          n_observation_iids,
                                                 int*         the_observation_iids,
                                                 int          n_instrument_iids,
                                                 int*         the_instrument_iids);

  protected:

    /// \brief Flag that indicates that a FILE* has been attached
    /// to this writer.

    bool   file_is_attached_;

    /// \brief Flag used to indicate that no write operations have been
    /// executed yet (either write_l() or write_o()) and that therefore
    /// there exist no "previous" time tag to detect an epoch change.

    bool   first_time_writing_;

    /// \brief Pointer to the file where data is to be written.

    FILE*  file_to_write_;

    /// \brief Flag used to check if an l-record has already been written
    /// in the current epoch (set of l- and o- records with the same time
    /// tag).

    bool   first_l_written_;

    /// \brief Default format used to write double values.

    char*  format_default_double_format_;

    /// \brief Default format used to write int values.

    char*  format_default_int_format_;

    /// \brief Fixed part of the format used for l-records.

    char*  format_fixed_l_;

    /// \brief Fixed part of the format used for o-records.

    char*  format_fixed_o_;

    /// \brief Value of the current time tag. Used to detect new epochs
    /// (set of l- and o-records with the same time tag).

    double last_time_tag_;

};

#endif // OBSERVATION_FILE_WRITER_TXT_EMBEDDED_HPP
