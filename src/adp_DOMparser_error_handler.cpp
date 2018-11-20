/** \file adp_DOMparser_error_handler.cpp
  \brief Implementation file for adp_DOMparser_error_handler.hpp
  \ingroup adp_DOMparser_group
*/

#include "adp_DOMparser_error_handler.hpp"

adp_DOMparser_error_handler::
adp_DOMparser_error_handler
(adp_parsing_errors* errors) : fSawErrors(false)
{
  {
    error_list_ = errors;
  }
}


adp_DOMparser_error_handler::
~adp_DOMparser_error_handler
(void)
{
  {
    // Intentionally left blank.
  }
}

char*
adp_DOMparser_error_handler::
getErrorText
(void)
const
{
  {
    const char* noErrors = "No errors detected";

    if (!fSawErrors) return strdup(noErrors);
    else             return (char*)errorString;
  }
}

bool
adp_DOMparser_error_handler::
getSawErrors
(void)
const
{
  {
    return fSawErrors;
  }
}

bool
adp_DOMparser_error_handler::
handleError
(const DOMError& domError)
{
  {
    XMLFileLoc column;
    XMLFileLoc line;
    char*      location;
    char*      msg;

    std::string full_error_message;

    fSawErrors = true;

    msg      = xercesc::XMLString::transcode(domError.getMessage());
    location = xercesc::XMLString::transcode(domError.getLocation()->getURI());
    line     = domError.getLocation()->getLineNumber();
    column   = domError.getLocation()->getColumnNumber();

    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
    {
      full_error_message = "WARNING";
    }
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
    {
      full_error_message = "ERROR";
    }
    else
    {
      full_error_message = "FATAL ERROR";
    }

    full_error_message += " at file ";
    full_error_message += location;
    #ifdef __GNUC__
      sprintf(errorString,"%lu",line);
    #else
      sprintf (errorString, "%llu", line);
    #endif

    full_error_message += " line ";
    full_error_message += errorString;

    #ifdef __GNUC__
      sprintf(errorString,"%lu",column);
    #else
      sprintf (errorString, "%llu", column);
    #endif

    full_error_message += " char ";
    full_error_message += errorString;

    full_error_message += ": ";
    full_error_message += msg;

    error_list_->add_error(full_error_message);

    xercesc::XMLString::release(&msg);
    xercesc::XMLString::release(&location);

    return true;
  }
}

void
adp_DOMparser_error_handler::
resetErrors
(void)
{
  {
    fSawErrors = false;
  }
}
