/** \file gp_ref_code.hpp
\brief Entity class to store coordinate system (CS),
       reference frame (RF) or coordinate reference frame (CRF)
       codes.
\ingroup ASTROLABE_metadata
*/

#ifndef _GP_REF_CODE_HPP_
#define _GP_REF_CODE_HPP_

#include <vector>

#include "gp_ref_code_element.hpp"

using namespace std;

/// \brief Entity class to store coordinate system (CS),
/// reference frame (RF) or coordinate reference frame (CRF)
/// codes.

/**
  Entity class to store a coordinate system (CS), reference frame (RF)
  or coordinate reference frame (CRF) codes.

  CS, RF or CRF codes are made of a series of elements separated
  by plus signs (+). For instance, the
  following could be an example of a full CS / RF / CRF code:

  a(b,c,d,e)+f(g)+h

  The whole string would be the CS / RF / CRF code.  This class models
  these codes.

  Codes are built using refernce code elements (modelled by class
  gp_ref_code_element). An element is each of the components in
  the ref code that is delimited by plus signs (+).

  Note that a CS, RF or CRF code may be composed of a single element,
  so no plus signs would exist, as in the following example:

  id(par1,par2,par3)

  Such elements, as those shown above, are made of a single identifier
  and an optional list of parameters enclosed in parenthesis and separated by commas,
  like, for instance,

  - the\_crf\_code
  - another\_crf\_code\_with\_parameters(par1, par2,..., parN)

  Note that since parameters are optional, an element may consist of a
  single identifier only.
*/

class gp_ref_code
{
  public:

    /// \brief Add a new element to the CRF.

    void                 add_element          (const gp_ref_code_element& element);

    /// \brief Retrieve the number of elements in the code.
    /**
    \return The number of elements integrating the code (that is,
    the number of components separated by plus signs).
    */

    int                  get_n_elements       (void) const;

    /// \brief Retrieve one of the elements of the CRF by position.
    /**
       \param pos The position of the element to retrieve, which
              ranges between 0 and get_n_elements() - 1.
       \param error An output error code. 0 will stand for successful
              completion, while 1 means that parameter pos points to
              an invalid element position.
       \return The requested element if pos points to a valid
               element or an empty element otherwise.
    */

    gp_ref_code_element& get_ref_code_element (int pos, int& error);

    /// \brief Default constructor.

                         gp_ref_code          (void);

    /// \brief Destructor.

                         ~gp_ref_code         (void);

    /// \brief Build a string representation of the ref code.
    /**
      \return A string representation of the ref code adhering
              to the ASTROLABE syntax.
    */

    string&              to_string            (void);

  protected:

    /// \brief The code, including all its components (the elements
    /// that are separated by plus signs).

    vector<gp_ref_code_element> the_code_;
};

#endif // _GP_REF_CODE_HPP_