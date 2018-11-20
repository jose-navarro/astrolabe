/** \file gp_astrolabe_header_file_data.hpp
  \brief Entity class to hold the data stored in an ASTROLABE header file.
  \addtogroup ASTROLABE_metadata Reading ASTROLABE metadata
*/

#ifndef _GP_ASTROLABE_HEADER_FILE_DATA_HPP_
#define _GP_ASTROLABE_HEADER_FILE_DATA_HPP_

#include "astrolabe_exceptions.hpp"

#include "lineage_data.hpp"

/// \brief Entity class to hold the data stored in an ASTROLABE header file.

class gp_astrolabe_header_file_data
{
  public:

    /// \brief Public constant: the device used to keep data is
    /// a binary file.

    const static int GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE;

    /// \brief Public constant: the device used to keep data is
    /// not set.

    const static int GP_ASTROLABE_HEADER_FORMAT_IS_NOT_SET;

    /// \brief Public constant: the device used to handle data is
    /// a socket.

    const static int GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET;

    /// \brief Public constant: the device used to keep data is
    /// a text file.

    const static int GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE;

  public:

    /// \brief Get the device file name used to store data.
    /**
      \pre ((get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE) |
            (get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE))
      \pre (is_set_device_file_name())
      \return The name of the file used to store data when the device
             used to do so is a file.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Note that a device file name is only meaningful when

      - a file is used as the storage device (that is,
        get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE or
        get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE and
      - the file name has been previously set.

      When any of these conditions are not met, an exception is thrown.
    */

    string        get_device_file_name           (void) const;

    /// \brief Get the format of the device used to store or handle data.
    /**
      \return The format of device used to store or handle data.

      This method will return either of the following values:

      - GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE - Data is stored in a
        binary file.
      - GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET - Data must be handled using
        a TCP/IP socket.
      - GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE - Data is stored in a
        text file.
      - GP_ASTROLABE_HEADER_FORMAT_IS_NOT_SET - This value has not been
        set yet.
    */

    int           get_device_format              (void) const;

    /// \brief Get the TCP/IP port used to handle data.
    /**
      \pre (get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET) 
      \pre (is_set_device_port())
      \return The port number of the socket used to handle data
             when the device used to do so is a socket.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Note that a device port is only meaningful when

      - a socket is used to handle data, that is,
        get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET and
      - the port has been previously set.

      When any of these conditions are not met, an exception is thrown.
    */

    int           get_device_port                (void) const;

    /// \brief Get the name or IP address of the server used to handle data.
    /**
      \pre (get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET)
      \pre (is_set_device_server())
      \return The name or IP address of the server used to handle data
             when the device used to do so is a socket.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Note that a device server is only meaningful when

      - a socket is used to handle data, that is,
        get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET_CLIENT and
      - the server name or IP has been previously set.

      When any of these conditions are not met, an exception is thrown.
    */

    string         get_device_server             (void) const;

    /// \brief Get the type of data stored or handled.
    /**
      \pre (is_set_device_type())
      \return The kind or type of data described by the ASTROLABE header file.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Note that the device type is only meaningful when

      - the type of data described by the ASTROLABE header file has already
        been set.

      When any of these conditions are not met, an exception is thrown.
    */

    string         get_device_type               (void) const;

    /// \brief Retrieve a pointer to the lineage_data object included
    /// in the gp_astrolabe_header_file_data object.
    /**
      This method provides with an easy way to access the methods of
      the lineage_data object, so the API of this class is not unnecessarily
      complicated.

      Note that the object referenced by the pointer thus retrieved MUST
      NEVER BE DELETED by the calling module. Doing so would corrupt the
      gp_astrolabe_header_file_data object and this could crash the application.
    */

    lineage_data* get_lineage_ptr                (void);

    /// \brief Default constructor.

                  gp_astrolabe_header_file_data  (void);

    /// \brief Destructor.

                  ~gp_astrolabe_header_file_data (void);

    /// \brief Check if the device file name has been set.
    /**
      \return True if the device file name has been set, false otherwise.

      This method is used mainly in preconditions.
    */

    bool          is_set_device_file_name        (void) const;

    /// \brief Check if the device format has been set.
    /**
      \return True if the device format has been set, false otherwise.

      This method is used mainly in preconditions.
    */

    bool          is_set_device_format           (void) const;

    /// \brief Check if the device port has been set.
    /**
      \return True if the device port has been set, false otherwise.

      This method is used mainly in preconditions.
    */

    bool          is_set_device_port             (void) const;

    /// \brief Check if the device server has been set.
    /**
      \return True if the device server has been set, false otherwise.

      This method is used mainly in preconditions.
    */

    bool          is_set_device_server           (void) const;

    /// \brief Check if the device type has been set.
    /**
      \return True if the device type has been set, false otherwise.

      This method is used mainly in preconditions.
    */

    bool          is_set_device_type             (void) const;

    /// \brief Set the device file name used to store data when
    /// files are used to do it.
    /**
      \pre ((get_device_format()  == GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE) |
            (get_device_format()  == GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE))
      \param device_file_name The name of the file used to store data when
             the device used to do so is a file.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Note that a device file name may be set only when

      - a file is used as the external storage device (that is,
        - get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE or
        - get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE) and
      - the file name has not been previously set.

      When any of these conditions are not met, an exception is thrown.
    */

    void          set_device_file_name           (string& device_file_name);

    /// \brief Set the device format used to store data.
    /**
      \pre ((device_type == GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE)   |
            (device_type == GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE)     |
            (device_type == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET))
      \pre (!is_set_device_format())
      \param device_format The format of the device used to store or handle data.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      This method is able to set the format of the device used to
      store or handle data, which may be one of the following values:

      - GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE - Data is stored in a
        binary file.
      - GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET_CLIENT - Data is handled using
        a TCP/IP socket.
      - GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE - Data is stored in a
        text file.
      - GP_ASTROLABE_HEADER_FORMAT_IS_NOT_SET - This value has not been
        set yet.

      Note that this method is a run-once one. It is not possible
      to change the device format if it has already been set, since
      there are other members in this class that depend on this value
      that might become meaningless after such a change.
    */

    void          set_device_format              (int device_format);

    /// \brief Set the TCP/IP port used to handle data.
    /**
      \pre (get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET)
      \param device_port The port number of the socket used to handle
             data when the device used to do so is a TCP / IP socket.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Note that a device port may be set only when

      - a socket is used to handle data, that is,
        get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET.

      When such condition is not met, an exception is thrown.
    */

    void          set_device_port                (int device_port);

    /// \brief Set the name or IP address of the server used to handle data.
    /**
      \pre (get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET)
      \param device_server The name or IP address of the server used to handle
             data when the device used to do so is a socket.
      \throw astrolabe_precondition_violation One or more preconditions
             have been violated.

      Note that a device server may be set only when

      - a socket is used to handle data, that is,
        get_device_format() == GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET.

      When such condition is not met, an exception is thrown.
    */

    void          set_device_server              (string& device_server);

    /// \brief Set the type of the data being handled.
    /**
      \param device_type The code representing the type of data being
             described by the ASTROLABE header file.
    */

    void          set_device_type                (string& device_type);

  protected:

    /// \brief Device format. Used to tell apart text / binary files
    /// or client / server socket connections.

    int          device_format_;

    /// \brief Flag that indicates whether the device format has been set.

    bool         device_format_is_set_;

    /// \brief Device type. Used to tell apart the different kinds of files
    /// that may be described by ASTROLABE header files.

    string       device_type_;

    /// \brief Flag that indicates whether the device type has been set.

    bool         device_type_is_set_;

    /// \brief Name of the file containing external data (when external
    /// data is used).

    string       file_name_;

    /// \brief Flag that indicates whether the file name has been set.

    bool         file_name_is_set_;

    /// \brief The lineage element.

    lineage_data lineage_;

    /// \brief Socket port, for TCP / IP socket devices.

    int          socket_port_;

    /// \brief Flag that indicates whether the socket port has been set.

    bool         socket_port_is_set_;

    /// \brief Socket server, for TCP / IP socket devices.

    string       socket_server_;

    /// \brief Flag that indicates whether the socket server has been set.

    bool         socket_server_is_set_;
  };

#endif // _GP_ASTROLABE_HEADER_FILE_DATA_HPP_
