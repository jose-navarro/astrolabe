/** \file obs_e_based_file_writer.hpp
  \brief  Abstract class to define the interface of obs-e based file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBS_E_BASED_FILE_WRITER_HPP
#define OBS_E_BASED_FILE_WRITER_HPP

#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;

/// @brief Default buffer size: 5 Mbyte (5 * 1024 * 1024 bytes)

#define _ASTROLABE_FILE_WRITER_DEFAULT_BUFFER_SIZE 5242880

/**
 * @brief  Abstract class to define the interface of event file writers.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * All writer classes derived from this abstract class MUST implement
 * a buffered I/O model, since the goal of this library is to reduce
 * the amount of disk operations
 *
 * Additionally, all descendant writer classes must adhere to the same
 * interface described in this abstract class, no matter how specialized these
 * might be.
 *
 * The overall usage philosophy is described below:
 *
 * - A writer object must be instantiated using the appropriate, specialised
 *   constructor.
 * - Optionally, the size of the internal buffer used to reduce disk I/O
 *   may be set using method set_buffer_size(). Note that all descendant
 *   classes must provide with a default buffer size that would be used
 *   if a calling module makes no decisions on this subject.
 * - The writer must be associated to a physical output file using
 *   method open(). This action enables actual I/O operations.
 * - A series of either l- or o-records may be written using, respectively,
 *   methods write_l() or write_o(). More on this below (*)
 * - The writer must close() the associated output file, thus guaranteeing
 *   that no pending write operations are left behind. Note that close()
 *   must flush the contents of the internal buffer to disk, when non empty.
 *   Closing the associated file will disable any further I/O write operations.
 * - The writer object may be destroyed. Destructors must guarantee that
 *   close() (see previous point) has been called. If not, the destructor
 *   will call close() by itself, in an automatic, transparent way.
 *
 * (*) Note that l- and o- records have an attribute named time tag. Successive
 * l- and o-records sharing the same value for the time tag will be written
 * together. This set of records (either l- or o-) sharing the same value
 * for its time tag are called an "epoch". The first record in an epoch must
 * always be an l-record. write_l() and write_o() must complain (by means of
 * an appropriate error code) about any violation of this policy.
 *
 */
class obs_e_based_file_writer
{
  public:

    /**
     * @brief Returns the current size of the buffer internally
     *        used by the writer.
     * @return Current size in bytes of the internal writer's buffer.
     *
     * All writers use an internal buffer to reduce the amount
     * of I/O operations. The size of this buffer may be, optionally,
     * set using method set_buffer_size().
     *
     * This method return the size of the buffer at the moment it is called;
     * if no calls to set_buffer_size() are made, it will be equivalent
     * to _default_buffer_size(). On the contrary, the result will vary
     * if set_buffer_size() is invoked. In this case, the size set by
     * the user will be returned.
     */

    virtual int _buffer_size               (void) const;

    /**
     * @brief Returns a copy of the name of the file the writers are
     *        dealing with.
     * @return The name of the file when the file has been opened, a
     *        NULL value otherwise.
     */

    virtual char* _file_name               (void) const;

    /**
     * @brief Closes the event file writer, disabling any other
     *        further action on it.
     * @return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the file.
     *        - 2: I/O error while writing data to disk.
     *
     * This method is implemented in desdendant classes only. See
     * the documentation of the specialized classes for details.
     */

    virtual int  close                     (void)                      = 0;

    /**
     * @brief Returns the default size of the buffer internally
     *        used by the writer.
     * @return Default size in bytes of the internal writer's buffer (either
     * the default one or the one set by the calling module using
     * set_buffer_size())
     *
     * All writers use an internal buffer to reduce the amount
     * of I/O operations. The size of this buffer may be, optionally,
     * set using method set_buffer_size().
     *
     * This method return this size in bytes.
     *
     * Note that when no size is set, writers use a default buffer size.
     *
     */

    virtual int _default_buffer_size       (void) const;

    /**
     * @brief Default constructor.
     *
     * Since this is an abstract class, it can not be instantiated.
     * Use the specific constructors and destructors of descendant classes
     * to instantiate or destroy the corresponding specialized writers.
     */

                 obs_e_based_file_writer   (void);

     /**
      * @brief Destructor.
      *
      * Since this is an abstract class, it can not be instantiated.
      * Use the specific constructors and destructors of descendant classes
      * to instantiate or destroy the corresponding specialized writers.
      */

    virtual     ~obs_e_based_file_writer   (void);

    /**
     * @brief Check if the underlying data file has already been opened.
     *
     * @return True if the file has already been opened, false otherwise.
     */

    virtual bool is_open                   (void) const;

    /**
     * @brief Opens the writer, enabling I/O operations.
     *
     * @param file_name Name of the file to write.
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: The file was already open.
     *        - 2: Unable to open the selected file.
     *        - 3: Not enough memory to allocate the internal buffer.
     *
     * Once a writer has been instantiated (and, optionally, the
     * size of the internal buffer set using set_buffer_size()), it is
     * possible to open the underlying file to enable I/O operations.
     *
     * This method opens the file and instantiates the internal memory
     * buffer used to speed up I/O operations, enabling the different
     * write operations available in this class (see write_l() and
     * write_o(), as well as close()).
     */

    virtual int  open                      (const char*  file_name);

    /**
     * @brief Set the size of the internal buffer used by the writer.
     *
     * @param buffer_size Size in bytes of the internal buffer.
     * @return Error code. Error code values are:
     *         - 0: Successful completion.
     *         - 1: Writer already open. It is not allowed to change
     *              the buffer size at this moment.
     *         - 2: Invalid buffer size. It must be greater than zero.
     *
     * All writers use an internal buffer to reduce the amount
     * of I/O operations. The size of this buffer may be, optionally,
     * set using method set_buffer_size().
     *
     * This method may be called only after once the writer has been
     * instantiated but before the open() method has been invoked.
     *
     * If this method, which is optional, is not used, a default buffer
     * size will be used by the writer. See _default_buffer_size() for
     * details.
     */

    virtual int  set_buffer_size           (int          buffer_size);

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
     *
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
     * This method is implemented in desdendant classes only. See
     * the documentation of the specialized classes for details.
     */

    virtual int  write_l                   (bool         active,
                                            char*        identifier,
                                            int          instance_identifier,
                                            double       time,
                                            int          n_tags,
                                            double*      buffer_tags,
                                            int          n_expectations,
                                            double*      buffer_expectations,
                                            int          n_covariance_values,
                                            double*      buffer_covariance) = 0;

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
     *
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: File is not open so it is not possible to write.
     *        - 2: I/O error detected while writing to disk.
     *        - 3: An attempt to write an o-record as the first record in the
     *             group sharing the same time tag has been detected.
     *        - 4: Parameter identifier must at least 1 character long (excluding
     *             the terminating NULL string).
     *        - 5: NOT USED.
     *        - 6: NOT USED.
     *        - 7: Either the number of parameter or observation instance
     *              identifiers are equal to or less than zero. These values
     *              must be strictly positive. Also, this error code may
     *              be issued if the number of instrument instance identifiers
     *              requested is negative (but it may be zero).
     *
     * The number of elements in the different data arrays must be coherent
     * with the number of elements stated in the different dimensioning
     * parameters (n_parameter_iids, n_observation_iids and n_instrument_iids).
     *
     * This method is implemented in desdendant classes only. See
     * the documentation of the specialized classes for details.
     *
     */

    virtual int  write_o                   (bool         active,
                                            char*        identifier,
                                            double       time,
                                            int          n_parameter_iids,
                                            int*         the_parameter_iids,
                                            int          n_observation_iids,
                                            int*         the_observation_iids,
                                            int          n_instrument_iids,
                                            int*         the_instrument_iids) = 0;

  protected:

    /**
     * @brief Writes the contents of the internal buffer to disk.
     *
     * @return Error code. Error code values:
     *        - 0: successful completion.
     *        - 1: Error detected while writting the buffer to disk.
     *
     * This protected method writes the contents of the internal buffer to
     * disk, thus reducing the total amount of I/O operations needed and
     * (hopefully) improving the performance.
     */

    virtual int  flush                     (void);

  protected:

    /// @brief The data_buffer_ used to store the data to write.

    char*  data_buffer_;

    /// @brief First free position in data_buffer_. Used to track where
    /// to put new information when write_l() and write_o() try to write
    /// to disk (using data_buffer_ as the intermediate buffer).

    int    data_buffer_cur_;

    /// @brief Actual size in bytes of data_buffer_.

    int    data_buffer_size_;

    /// @brief The file handled by the writer.

    FILE*  file_file_;

    /// @brief The name of the underlying file being dealt with.

    char*  file_name_;

    /// @brief. Flag. When true, the underlying file has been opened.

    bool   file_is_open_;

    /// @brief Flag used to check if an l-record has already been written
    /// in the current epoch (set of l- and o- records with the same time
    /// tag).

    bool   first_l_written_;

    /// @brief Flag used to indicate that no writes operations have been
    /// executed yet (either write_l() or write_o()) and that therefore
    /// there exist no "previous" time tag to detect an epoch change.

    bool   first_time_writing_;

    /// @brief Value of the current time tag. Used to detect new epochs
    /// (set of l- and o-records with the same time tag).

    double last_time_tag_;
};

#endif // OBS_E_BASED_FILE_WRITER_HPP
