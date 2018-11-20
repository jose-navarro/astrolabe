/** \file parameter_socket_reader.hpp
  \brief Parameters socket reader
  \ingroup ASTROLABE_data_IO
*/

#ifndef PARAMETER_SOCKET_READER_HPP
#define PARAMETER_SOCKET_READER_HPP

#include "obs_e_based_socket_reader.hpp"

/**
 * @brief  Socket reader class. Read parameter data through a
 * socket connection.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class, parameter_socket_reader is a direct descendant of obs_e_based_socket_reader
 * where reading o-records IS NOT allowed.
 *
 * See, therefore, class obs_e_based_socket_reader for a full description of the methods
 * available in this class.
 *
 * Note that there may exists other descendants of obs_e_based_socket_reader that do not
 * allow the use of o-records (as in the case of instruments) or, on the contrary, that
 * allows such kind or records, like observations.
 *
 */

class parameter_socket_reader : public obs_e_based_socket_reader
{
  public:

    /**
     * \brief Default constructor.
     */

         parameter_socket_reader  (void);

    /**
     * \brief Destructor
     */

         ~parameter_socket_reader (void);

};

#endif // PARAMETER_SOCKET_READER_HPP