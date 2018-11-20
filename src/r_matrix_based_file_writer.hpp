/** \file r_matrix_based_file_writer.hpp
  \brief  Abstract class to define the interface of r_matrix based file writers.
  \ingroup ASTROLABE_data_IO
*/

#ifndef R_MATRIX_BASED_FILE_WRITER_HPP
#define R_MATRIX_BASED_FILE_WRITER_HPP

#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;

/// @brief Default buffer size: 5 Mbyte (5 * 1024 * 1024 bytes)

#define _ASTROLABE_R_MATRIX_FILE_WRITER_DEFAULT_BUFFER_SIZE 5242880

/**
 * @brief  Abstract class to define the interface of r_matrix based file writers.
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
 * - A series of r-records may be written using method write_r().
 * - The writer must close() the associated output file, thus guaranteeing
 *   that no pending write operations are left behind. Note that close()
 *   must flush the contents of the internal buffer to disk, when non empty.
 *   Closing the associated file will disable any further I/O write operations.
 * - The writer object may be destroyed. Destructors must guarantee that
 *   close() (see previous point) has been called. If not, the destructor
 *   will call close() by itself, in an automatic, transparent way.
 *
 */
class r_matrix_based_file_writer
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

    virtual int _buffer_size                (void) const;

    /**
     * @brief Returns a copy of the name of the file the writers are
     *        dealing with.
     * @return The name of the file when the file has been opened, a
     *        NULL value otherwise.
     */

    virtual char* _file_name                (void) const;

    /**
     * @brief Closes the r_matrix file writer, disabling any other
     *        further action on it.
     * @return The error code. Possible error code values are:
     *        - 0: Successful completion.
     *        - 1: Error closing the file.
     *        - 2: I/O error while writing data to disk.
     *
     * This method is implemented in desdendant classes only. See
     * the documentation of the specialized classes for details.
     */

    virtual int  close                      (void) = 0;

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

    virtual int _default_buffer_size        (void) const;

    /**
     * @brief Check if the underlying data file has already been opened.
     *
     * @return True if the file has already been opened, false otherwise.
     */

    virtual bool is_open                    (void) const;

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
     * write operations available in this class (see write_r()
     * as well as close()).
     */

    virtual int  open                       (const char*  file_name);

    /**
     * @brief Default constructor.
     *
     * Since this is an abstract class, it can not be instantiated.
     * Use the specific constructors and destructors of descendant classes
     * to instantiate or destroy the corresponding specialized writers.
     */

                 r_matrix_based_file_writer (void);

     /**
      * @brief Destructor.
      *
      * Since this is an abstract class, it can not be instantiated.
      * Use the specific constructors and destructors of descendant classes
      * to instantiate or destroy the corresponding specialized writers.
      */

    virtual     ~r_matrix_based_file_writer (void);

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

    virtual int  set_buffer_size            (int          buffer_size);

    /**
     * @brief Write an r-record.
     *
     * @param active The r-record is active (true) or removed (false).
     * @param time Time tag of the r-record.
     * @param n_correlation_values Number of correlation values to
     *        write. Must be > 0.
     * @param the_correlation_values The correlation valus to write.
     *
     * @return Error code. Error code values:
     *        - 0: Successful completion.
     *        - 1: File is not open so it is not possible to write.
     *        - 2: I/O error detected while writing to disk.
     *        - 3: Invalid number of correlation values. It must be > 0.
     *
     * This method is implemented in desdendant classes only. See
     * the documentation of the specialized classes for details.
     */

    virtual int  write_r                    (bool         active,
                                             double       time,
                                             int          n_correlation_values,
                                             double*      the_correlation_values) = 0;

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
    /// to put new information when write_r() tries to write
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

};

#endif // R_MATRIX_BASED_FILE_WRITER_HPP
