/** \file adp_stringx.hpp
  \brief XMLCh to local code page transcoding.
  \ingroup adp_DOMparser_group
*/

#ifndef __ADP_STRINGX_HPP__
#define __ADP_STRINGX_HPP__

#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <fstream>

XERCES_CPP_NAMESPACE_USE

/// \brief XMLCh to local code page transcoding.
/**
  This is a simple class that lets us do easy (though not terribly efficient)
  trancoding of XMLCh data to local code page for display.
 */

class adp_stringx
{
public :

    /// \brief Constructor
    /**
       \param toTranscode String to transcode to local code page.
     */

    adp_stringx(const XMLCh* const toTranscode)
    {
      {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
      }
    }

    /// \brief Destructor.
    ///
    ~adp_stringx (void)
    {
      {
        XMLString::release(&fLocalForm);
      }
    }


    /// \brief Retrieve the local code page version (local form) of the string.
    /**
       \return The local code page (local form) of the string used to instantiate
               the object.
     */

    const char* localForm() const
    {
      {
        return fLocalForm;
      }
    }

private :

    /// \brief Local code page form of a string.

    char*   fLocalForm;
};

/// \brief Print somewhere.
/**
   \param target The channel to which the string will be sent.
   \param toDump The string to send / print / write.
   \return Reference to the ostream used (the target, once it has received
          the string to send / print / write.
 */

inline ostream& operator<<(ostream& target, const adp_stringx& toDump)
{
  {
    target << toDump.localForm();
    return target;
  }
}

#endif // __STRINGX_HPP__
