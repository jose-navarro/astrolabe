/** \file observation_socket_writer.hpp
  \brief  observation socket writer.
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBSERVATION_SOCKET_WRITER_HPP
#define OBSERVATION_SOCKET_WRITER_HPP

#include "obs_e_based_socket_writer.hpp"

/**
 * @brief  Socket writer class. Writes observation-event data through a
 * socket connection.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class, observation_socket_writer is a direct descendant of obs_e_based_socket_writer.
 * See, therefore, class obs_e_based_socket_reader for a full description of the methods
 * available in this class.
 *
 * The parent class, obs_e_based_socket_writer, allows writing o-records (since it implements
 * the full blown observation-events specification). This one does as well, since observations
 * fully adhere to such specification.
 *
 */

class observation_socket_writer : public obs_e_based_socket_writer
{
  public:

    /**
     * \brief Default constructor.
     */
         observation_socket_writer (void);

     /**
      * \brief Destructor.
      */
        ~observation_socket_writer (void);
};

#endif // OBSERVATION_SOCKET_WRITER_HPP
