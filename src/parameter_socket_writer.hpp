/** \file parameter_socket_writer.hpp
  \brief  Parameter socket writer.
  \ingroup ASTROLABE_data_IO
*/

#ifndef PARAMETER_SOCKET_WRITER_HPP
#define PARAMETER_SOCKET_WRITER_HPP

#include "obs_e_based_socket_writer.hpp"

/**
 * @brief  Socket writer class. Writes parameter data through a
 * socket connection.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class, parameter_socket_writer is a direct descendant of obs_e_based_socket_writer.
 * See, therefore, class obs_e_based_socket_reader for a full description of the methods
 * available in this class.
 *
 * The parent class, obs_e_based_socket_writer, allows writing o-records (since it implements
 * the full blown observation-events specification). This one, on the contrary, does not
 * allow to do so, since parameter data includes no o-records.
 *
 */

class parameter_socket_writer : public obs_e_based_socket_writer
{
  public:

    /**
     * \brief Default constructor.
     */
         parameter_socket_writer (void);

     /**
      * \brief Destructor.
      */
        ~parameter_socket_writer (void);
};

#endif // PARAMETER_SOCKET_WRITER_HPP
