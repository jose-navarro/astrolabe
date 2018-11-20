/** \file instrument_socket_reader.hpp
  \brief Instruments socket reader
  \ingroup ASTROLABE_data_IO
*/

#ifndef INSTRUMENT_SOCKET_READER_HPP
#define INSTRUMENT_SOCKET_READER_HPP

#include "obs_e_based_socket_reader.hpp"

/**
 * @brief  Socket reader class. Read instrument data through a
 * socket connection.
 *
 * \ingroup ASTROLABE_data_IO
 *
 * This class, instrument_socket_reader is a direct descendant of obs_e_based_socket_reader
 * where reading o-records IS NOT allowed.
 *
 * See, therefore, class obs_e_based_socket_reader for a full description of the methods
 * available in this class.
 *
 * Note that there may exists other descendants of obs_e_based_socket_reader that do not
 * allow the use of o-records (as in the case of parameters) or, on the contrary, that
 * allows such kind or records, like observations.
 *
 */

class instrument_socket_reader : public obs_e_based_socket_reader
{
  public:

    /**
     * \brief Default constructor.
     */

         instrument_socket_reader  (void);

    /**
     * \brief Destructor
     */

         ~instrument_socket_reader (void);

};

#endif // INSTRUMENT_SOCKET_READER_HPP