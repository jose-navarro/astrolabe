/** \file gp_ref_code_element.hpp
\brief Entity class to store a coordinate system (CS),
       reference frame (RF) or coordinate reference
       frame (CRF) codes' single elements.
\ingroup ASTROLABE_metadata
*/

#ifndef _GP_REF_CODE_ELEMENT_HPP_
#define _GP_REF_CODE_ELEMENT_HPP_

#include <string>
#include <vector>

using namespace std;

/**
  Entity class to store a coordinate system (CS), reference frame (RF)
  or coordinate reference frame (CRF) code's single element

  CS, RF or CRF codes are made of a series of elements separated
  by plus signs. For instance, the
  following could be an example of a full CS / RF / CRF code:

  a(b,c,d,e)+f(g)+h

  The whole string would be the CS / RF / CRF code. Each of the
  components separated by plus signs are the elements this
  class models.

  Note that a CS, RF or CRF code may be composed of a single element,
  so no plus signs would exist, as in the following example:

  id(par1,par2,par3)

  Such elements, as those shown above, are made of a single identifier
  and an optional list of parameters enclosed in parenthesis and separated by commas,
  like, for instance,
   
      - the_crf_code
      - another_crf_code_with_parameters(par1, par2,..., parN)

  Note that since parameters are optional, an element may consist of a
  single identifier only

*/

class gp_ref_code_element
{
  public:

    /// \brief Add an extra parameters to the current list of parameters.
    /**
      \param parameter The parameter to add to the current list of parameters.
             It should not be the empty string. If so, it is simply ignored.

      This methods appends one more parameter to the current list of CRF parameters.
    */

    void            add_parameters       (const string& parameter);

    /// \brief Retrieve the code of the CRF element.
    /**
      \return The code of the CRF element, which may be empty if it was
              never set.
    */
    string&         get_code             (void);

    /// \brief Retrieve the number of parameters attached to
    /// the CRF element.
    /**
      \return The number of parameters attached to the CRF component.
              It may be zero.
    */

    int             get_n_of_parameters  (void) const;

    /// \brief Retrieve the parameter at position i.
    /**
      \param  pos The position of the parameter requested. It must
              be in the range 0 .. get_n_of_parameters() - 1
      \return Parameter at position pos, or the empty string if
              the position specified is out of range.
    */
    string&         get_parameter        (int pos);

    /// \brief Default constructor.

                    gp_ref_code_element  (void);

    /// \brief Destructor.

                    ~gp_ref_code_element (void);

    /// \brief Set the CRF code.
    /**
      \param code The code to be set, identifying the CRF element.

      Note that this method will overwrite any previously set code.
    */

    void           set_code              (const string& code);

    /// \brief Build a string representation of the CRF element.
    /**
      \return A string representation of the CRF component adhering
              to the ASTROLABE syntax.
    */

    string&        to_string             (void);

  protected:

    /// \brief The code defining the CRF element.

    string         code_;

    /// \brief The list of parameters characterizing the CRF code.

    vector<string> parameters_;
};

#endif // _GP_REF_CODE_ELEMENT_HPP_