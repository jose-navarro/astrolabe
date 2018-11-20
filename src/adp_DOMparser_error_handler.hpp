/** \file adp_DOMparser_error_handler.hpp
  \brief adp_DOMparser class' error handler.
  \ingroup adp_DOMparser_group
*/

#ifndef __ADP_DOMPARSER_ERROR_HANDLER_HPP__
#define __ADP_DOMPARSER_ERROR_HANDLER_HPP__

#include "adp_parsing_errors.hpp"
#include "adp_stringx.hpp"

#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

///  \brief adp_DOMparser class' error handler.
/**

  Each Xerces DOM parser must have an associated error handler (derived
  from DOMErrorHandler).

  This is adp_DOMparser's own error handler. Obviously, the
  set of methods included in this class must derive from those
  in DOMErrorHandler.

  */


class adp_DOMparser_error_handler : public DOMErrorHandler
{
public:

    /// \brief Basic constructor
    /**
      \param errors List of errors. All the problems detected by
             the parsing process will be stored here.
    */

          adp_DOMparser_error_handler  (adp_parsing_errors* errors);

    /// \brief Destructor

          ~adp_DOMparser_error_handler (void);

    /// Get the last error's text.
    /**
      \return The text associated to the last error detected, if any.

      getSawErrors() should be used before calling this method.
    */

    char* getErrorText                 (void) const;

    /// \brief Check if errors have been detected.
    /**
      \return True if errors have been detected up to the moment, false
              otherwise.
    */

    bool  getSawErrors                 (void) const;


    /// \brief Stores the errors detected in the list of errors.
    /**

      \param domError The error reported by the parser.

      Whenever the parsing process detects an error, this method
      is invoked in order to deal with the error as convenient.

      Here, the error text is stored in error_list_, so the
      different problems detected can be preserved and checked
      when the calling module thinks it's convenient.

      Overrides of the default handleError method in parent class.

    */

    bool  handleError                  (const DOMError& domError);

    /// \brief Reset the "errors detected" state.
    /**

      This method resets the errors detected state to false. It does not
      alters the contents of error_list_, the actual list of detected
      errors.

     */

    void  resetErrors                  (void);


private :

    /// \brief Flag used to signal that errors have been detected.

    bool            fSawErrors;

    /// \brief Buffer to keep the last error text, if any (fSawErrors must be true).

    char            errorString[2000];

    /// \brief Pointer to the global list of errors. Initialized by the constructor.

    adp_parsing_errors* error_list_;
};

#endif // __ADP_DOMPARSER_ERROR_HANDLER_HPP__
