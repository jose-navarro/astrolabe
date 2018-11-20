/** \file astrolabe_contracts.hpp
\brief Definition of the macros to check pre- and postconditions.
\ingroup ASTROLABE_exceptions_group
*/

#ifndef ASTROLABE_CONTRACTS_HPP
#define ASTROLABE_CONTRACTS_HPP

#include <assert.h>
#include <iostream>

using namespace std;

//
// set_precondition and set_postcondition, only active in debug mode
//

#ifndef NDEBUG
  #define  set_precondition(ptr) \
           {if ((ptr)==0)	\
            {	\
              astrolabe_precondition_violation exc_g; \
              exc_g.set_contract(#ptr); \
              exc_g.set_line(__LINE__); \
              exc_g.set_file(__FILE__); \
              cout << exc_g; \
              throw exc_g; \
            }	\
           }

  #define  set_postcondition(ptr)  \
           {if ((ptr)==0)	\
            {	\
              astrolabe_postcondition_violation exc_g; \
              exc_g.set_contract(#ptr); \
              exc_g.set_line(__LINE__); \
              exc_g.set_file(__FILE__); \
              cout << exc_g; \
              throw exc_g; \
            }	\
           }
#else

  #define set_precondition(ptr)  assert(ptr)
  #define set_postcondition(ptr) assert(ptr)

#endif

//
// SET_PRECONDITION and SET_POSTCONDITION, all time active
//

#define  SET_PRECONDITION(ptr) \
         {if ((ptr)==0)	\
          {	\
            astrolabe_precondition_violation exc_g; \
            exc_g.set_contract(#ptr); \
            exc_g.set_line(__LINE__); \
            exc_g.set_file(__FILE__); \
            cout << exc_g; \
            throw exc_g; \
          }	\
         }

#define  SET_POSTCONDITION(ptr)  \
         {if ((ptr)==0)	\
          {	\
            astrolabe_postcondition_violation exc_g; \
            exc_g.set_contract(#ptr); \
            exc_g.set_line(__LINE__); \
            exc_g.set_file(__FILE__); \
            cout << exc_g; \
            throw exc_g; \
          }	\
         }

#endif // ASTROLABE_CONTRACTS_HPP
