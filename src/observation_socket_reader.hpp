/** \file observation_socket_reader.hpp
  \brief Observations socket reader
  \ingroup ASTROLABE_data_IO
*/

#ifndef OBSERVATION_SOCKET_READER_HPP
#define OBSERVATION_SOCKET_READER_HPP

#include "obs_e_based_socket_reader.hpp"

/**
 * @brief  Socket reader class. Read observation data through a
 * socket connection.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class, observation_socket_reader is a direct descendant of obs_e_based_socket_reader.
 * See, therefore, class obs_e_based_socket_reader for a full description of the methods
 * available in this class.
 *
 * The parent class, obs_e_based_socket_reader, allows reading o-records (since it implements
 * the full blown observation-events specification). This one, also allows reading such
 * records since observation data include these.
 *
 * Note that there may exists other descendants of obs_e_based_socket_reader that do not
 * allow the use of o-records (as in the case of parameters or instruments).
 *
 */

class observation_socket_reader : public obs_e_based_socket_reader
{
  public:

    /**
     * \brief Default constructor.
     */

         observation_socket_reader  (void);

    /**
     * \brief Destructor
     */

         ~observation_socket_reader (void);

};

#endif // OBSERVATION_SOCKET_READER_HPP