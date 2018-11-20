/** \file adp_DOMparser.cpp
  \brief Implementation file for adp_DOMparser.hpp
  \ingroup adp_DOMparser_group
*/

#include "adp_DOMparser.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>

//
// Constructor
//

adp_DOMparser::
adp_DOMparser
(const char*  xml_file_name)
{
  {

    XMLCh               tmp_string[100];
    DOMImplementation  *impl;

    xml_file_       = astr_dup(xml_file_name);

    current_label_  = NULL;
    doc_            = NULL;
    error_handler_  = NULL;
    error_occurred_ = false;
    path_to_schema_ = NULL;
    parser_         = NULL;
    parsed_         = false;
    valid_          = true;

    //
    // Initialize the XML4C system
    //

    try
    {
      XMLPlatformUtils::Initialize();
    }

    catch (const XMLException& toCatch)
    {
      error_occurred_ = true;
      sprintf(error_text_,"File %s: fatal error during parser initialization!\n%s",
        xml_file_, adp_stringx(toCatch.getMessage()).localForm());
      error_list_.add_error(error_text_);
      valid_ = false;
      return;
    }


    //
    // Instantiate the DOM parser.
    //

    XMLString::transcode("LS", tmp_string, 99);

    impl    = DOMImplementationRegistry::getDOMImplementation(tmp_string);
    parser_ = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    //
    // Create and install the error handler; Reset its errors.
    //

    try
    {
      error_handler_ = new adp_DOMparser_error_handler(&error_list_);
    }
    catch(...)
    {
      valid_         = false;
      error_handler_ = NULL;

      throw astrolabe_exception();
    }

    parser_->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, error_handler_);
    error_handler_->resetErrors();

    if (error_occurred_) valid_ = false;

  }
}

//
// Destructor.
//

adp_DOMparser::
~adp_DOMparser
(void)
{
  {

    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //

    parser_->release();

    //
    // And call the termination method
    //

    XMLPlatformUtils::Terminate();

    //
    // Delete the other members.
    //

                              free(xml_file_);
    if (error_handler_ != 0)  delete error_handler_;
    if (path_to_schema_ != 0) delete path_to_schema_;
  }
}

// Navigate back to the last "previous" current label.

void
adp_DOMparser::
go_back(void)
{
  set_precondition (is_parsing_done());
  set_precondition (is_backtracking_possible());
  {
    current_label_path_.pop();
    current_label_ = current_label_path_.top();
  }
}

void
adp_DOMparser::
goto_label
(const char* label_path,
 int         label_index)
{
  set_precondition (is_parsing_done());
  set_precondition (label_path != NULL);
  set_precondition (label_index > 0);
  {
    DOMNode* new_current;

    new_current = navigate_to_label(NULL, label_path, label_index);
    if (new_current == NULL)
    {
      throw astrolabe_exception();
    }

    current_label_ = new_current;

    while (!current_label_path_.empty()) current_label_path_.pop();
    current_label_path_.push(current_label_);
  }
  set_postcondition (is_current_label_set());
}

void
adp_DOMparser::
goto_label_relative
(const char* label_path,
 int         label_index)
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());
  set_precondition (label_path != NULL);
  set_precondition (label_index > 0);

  {
    DOMNode* new_current;

    new_current = navigate_to_label(current_label_, label_path, label_index);
    if (new_current == NULL)
    {
      throw astrolabe_exception();
    }

    current_label_ = new_current;
    current_label_path_.push(current_label_);
  }

  set_postcondition (is_current_label_set());
}

// Check if it is possible to navigate back.

bool
adp_DOMparser::
is_backtracking_possible
(void)
const
{
  bool result;
  {
    result = !(current_label_path_.empty());
  }
  return result;
}

//
// Check if the current label is set.
//

bool
adp_DOMparser::
is_current_label_set
(void)
const
{
  bool result;
  {
    result = (current_label_ != NULL);
  }
  return result;
}

//
// Check if the parsing process is over and OK.
//
bool
adp_DOMparser::
is_parsing_done
(void)
const
{
  {
    return parsed_;
  }
}

//
// Check if the parser has found errors in the input file.
//
bool
adp_DOMparser::
is_parsing_erroneous
(void)
const
{
  {
    return error_occurred_;
  }
}

//
// Check if the parser is valid (that is, no errors have been
// detected during instantiation, processing...)
//

bool
adp_DOMparser::
is_parser_valid
(void)
const
{
  {
    return valid_;
  }
}


// Retrieve the value of a named attribute of the current label as a boolean.

bool
adp_DOMparser::
_label_attribute_bool
(const char* attr_name)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());
  set_precondition (attr_name != NULL);

  bool result;

  {
    char* text;
    int   error;
    int   intboolean;

    text = _label_attribute_string(attr_name);

    intboolean = astr_to_bool(text, &error);

    result = (intboolean == 1);

    free(text);
    text = NULL;

    if (error != 0) throw astrolabe_parse_error();
  }

  return result;
}

// Retrieve the value of a named attribute of the current label as a double.

double
adp_DOMparser::
_label_attribute_double
(const char* attr_name)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());
  set_precondition (attr_name != NULL);

  double result;

  {
    char* text;
    int   error;

    text = _label_attribute_string(attr_name);

    result = astr_to_double(text, &error);

    free(text);
    text = NULL;

    if (error != 0) throw astrolabe_parse_error();
  }

  return result;
}

// Retrieve the value of a named attribute of the current label as a float.

float
adp_DOMparser::
_label_attribute_float
(const char* attr_name)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());
  set_precondition (attr_name != NULL);

  float result;

  {
    result = (float)_label_attribute_double(attr_name);
  }

  return result;
}

// Retrieve the value of a named attribute of the current label as an int(eger).

int
adp_DOMparser::
_label_attribute_int
(const char* attr_name)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());
  set_precondition (attr_name != NULL);

  int result;

  {
    result = (int)_label_attribute_long(attr_name);
  }

  return result;
}

// Retrieve the value of a named attribute of the current label as a long.

long
adp_DOMparser::
_label_attribute_long
(const char* attr_name)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());
  set_precondition (attr_name != NULL);

  long result;

  {
    char* text;
    int   error;

    text = _label_attribute_string(attr_name);

    result = astr_to_long(text, &error);

    free(text);
    text = NULL;

    if (error != 0) throw astrolabe_parse_error();
  }

  return result;
}

// Retrieve the value of a named attribute of the current label as a string.

char*
adp_DOMparser::
_label_attribute_string
(const char* attr_name)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());
  set_precondition (attr_name != NULL);

  char* result;

  {
    char* text;

    text = astr_dup(attr_name);

    result = du_.get_string_attribute(current_label_, text);

    free(text);
    text = NULL;

    if (result == NULL) throw astrolabe_exception ();
  }

  return result;
}

//
// Retrieve the text value of a given label.
//

char*
adp_DOMparser::
_label_value
(const DOMNode* label)
const
{
  char* result;

  if (label == NULL) return NULL;

  {
    result = du_.trim_element_text_contents(label);
  }
  return result;
}

// Retrieve the value of the current label as a boolean.

bool
adp_DOMparser::
_label_value_bool
(void)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());

  bool result;

  {
    char* text;
    int   error;
    int   intboolean;

    text   = _label_value(current_label_);

    intboolean = astr_to_bool(text,&error);

    result = (intboolean == 1);

    free(text);
    text = NULL;

    if (error != 0) throw astrolabe_parse_error();
  }

  return result;
}

// Retrieve the value of the current label as a double.

double
adp_DOMparser::
_label_value_double
(void)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());

  double result;

  {
    char* text;
    int   error;

    text   = _label_value(current_label_);
    result = astr_to_double(text,&error);

    free(text);
    text = NULL;

    if (error != 0) throw astrolabe_parse_error();
  }

  return result;
}

// Retrieve the value of the current label as an float.

float
adp_DOMparser::
_label_value_float
(void)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());

  float result;

  {
    result = (float) _label_value_double();

  }

  return result;
}

// Retrieve the value of the current label as an int(eger).

int
adp_DOMparser::
_label_value_int
(void)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());

  int result;

  {
    result = (int) _label_value_long();

  }

  return result;
}

// Retrieve the value of the current label as a long.

long
adp_DOMparser::
_label_value_long
(void)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());

  long result;

  {
    char* text;
    int   error;

    text   = _label_value(current_label_);
    result = astr_to_long(text,&error);

    free(text);
    text = NULL;

    if (error != 0) throw astrolabe_parse_error();
  }

  return result;
}

// Retrieve the value of the current label as a string.

char*
adp_DOMparser::
_label_value_string
(void)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());

  char* result;

  {
    result = _label_value(current_label_);
  }

  return result;
}

// Navigate to a given label.

DOMNode*
adp_DOMparser::
navigate_to_label
(DOMNode*       start,
 const char*    label_path,
 int            label_index)
const
{
  DOMNode* result;
  {

    astr_tokenizer* st;
    char*           label;
    const char*     separators = "/\\"; // Labels are separated by slashes
                                        // (either forward or backward).

    DOMNode*        current;
    DOMNode*        begin;
    char*           node_name;
    char*           last_label;

    int             status;
    int             i;

    result = NULL;

    if (label_path == NULL) return result;
    if (label_index <= 0)   return result;

    last_label = NULL;

    try
    {
      st = new astr_tokenizer(label_path, separators);
    }
    catch(...)
    {
      st = NULL;
      throw astrolabe_exception ();
    }

    begin = start;

    if (begin == NULL)
    {
      //
      // Absolute navigation. We must start from the root element.
      // Moreover, the first label in the label path MUST be
      // the root element!
      //

      // Get the first label in the path.

      label = st->next_token();
      if (label == NULL)
      {
        delete st;
        st = NULL;

        return result;
      }

      // Get the root node.

      begin = (DOMNode*)doc_->getDocumentElement();

      // Make sure that the root node equals the first label in our path.

      node_name = du_.get_element_name(begin);
      status = strcmp(label, node_name);

      delete node_name;
      node_name = NULL;

      if (status != 0)
      {
        // The root element is not our first label in the path!

        free(label);
        label = NULL;

        delete st;
        st = NULL;

        return result;
      }

      // The root node and the first label match. Prepare data to keep going.

      last_label = strdup(label);

      free(label);
      label = NULL;

    }

    //
    // From this point, we are always performing "relative" navigation. If
    // we had no starting point, we have fixed it in the previous if...
    //


    label = st->next_token();

    while (label != NULL)
    {
      current = du_.get_node_unique_son_by_name(begin, label);

      if (last_label != NULL)
      {
        free(last_label);
        last_label = NULL;
      }

      last_label = strdup(label);

      free(label);
      label = NULL;

      if (current == NULL)
      {
        free(last_label);
        last_label = NULL;

        delete st;
        st = NULL;

        return result;
      }

      begin = current;
      label = st->next_token();
    }

    //
    // We have found the label sought. We must check now if we have been
    // requested a particular index...
    //

    if (label_index > 1)
    {

      for (i = 2; i <= label_index; i++)
      {
        current = du_.get_next_sibling_element(begin);
        node_name = NULL;

        while (current != NULL)
        {
          begin = current;

          node_name = du_.get_element_name(begin);
          status = strcmp(node_name,last_label);

          delete node_name;
          node_name = NULL;

          if (status == 0) break; // We've found the label.

          current = du_.get_next_sibling_element(begin);
        }

        if (current == NULL)
        {
          // There is not "label_index" repeated labels...

          if (last_label != NULL)
          {
            free(last_label);
            last_label = NULL;

            delete st;
            st = NULL;

            return result;
          }
        }
      }
    }

    // Well, we've got it!

    if (last_label != NULL)
    {
      free(last_label);
      last_label = NULL;
    }

    delete st;
    st = NULL;

    result = begin;

  }
  return result;
}

//
// Number of repeated sublabels of the current label.
//

int
adp_DOMparser::
_n_of_repeated_sublabels
(const char* sublabel)
const
{
  set_precondition (is_parsing_done());
  set_precondition (is_current_label_set());
  set_precondition (sublabel != NULL);

  int result;
  {
    int error;

    result = du_.get_number_of_repeated_sons_by_name(current_label_,
                                                     sublabel,
                                                     &error);
    if (error != 0) result = 0;
  }
  return result;
}

//
// Parse the xml file
//

void
adp_DOMparser::
parse
(void)
{
  set_precondition (is_parser_valid());
  set_precondition (!is_parsing_done());
  {
    error_occurred_ = false;

    //
    // Kick off the parse of the indicated file. Catch any exceptions
    // that might propagate out of it. Reset error count first
    //

    try
    {
      //
      // Reset document pool
      //

      parser_->resetDocumentPool();

      //
      // Get the DOM document via XML parsing.
      //

      doc_ = parser_->parseURI(xml_file_);
    }

    catch (const XMLException& toCatch)
    {
      error_occurred_ = true;
      sprintf(error_text_,"File %s: %s",
        xml_file_, adp_stringx(toCatch.getMessage()).localForm());
      error_list_.add_error(error_text_);
    }
    catch (const DOMException& toCatch)
    {
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];

      error_occurred_ = true;

      sprintf(error_text_,
        "File %s: DOM Error during parsing\n    DOMException code: %d",
        xml_file_,toCatch.code);

      if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
      {
        strcat(error_text_,"\n    ");
        strcat(error_text_, adp_stringx(errText).localForm());
      }

      error_list_.add_error(error_text_);
    }
    catch (...)
    {
      error_occurred_ = true;
      sprintf(error_text_,"File %s: unexpected exception during parsing!",xml_file_);
      error_list_.add_error(error_text_);
    }

    if (error_handler_->getSawErrors())
    {
      error_occurred_ = true;
      sprintf(error_text_,"File %s: %s",
        xml_file_, error_handler_->getErrorText());
      error_list_.add_error(error_text_);
    }

    valid_  = !error_occurred_;

    if (!valid_)
    {
      throw astrolabe_exception ();
    }

    parsed_ = true;

    set_postcondition (is_parsing_done());

    return;
  }
}

//
// Retrieve the list of parsing errors
//

adp_parsing_errors*
adp_DOMparser::
_parsing_errors
(void)
{
  adp_parsing_errors* result;
  {
    try
    {
      result = new adp_parsing_errors(error_list_);
    }
    catch(...)
    {
      result = NULL;
      throw astrolabe_exception ();
    }
  }
  return result;
}


// Set the path to a validating schema.

void
adp_DOMparser::
set_schema
(const char* full_path_to_schema)
{
  set_precondition (is_parser_valid());
  set_precondition (!is_parsing_done());
  set_precondition (full_path_to_schema != NULL);

  {

    char  *schema_location;
    char  *normalized_schema_location;
    XMLCh *xerces_schema_path;

    if (path_to_schema_ != NULL) delete path_to_schema_;
    path_to_schema_ = strdup(full_path_to_schema);


    schema_location = astr_concat("file:///",path_to_schema_,NULL);
    normalized_schema_location = astr_replace(schema_location, "\\", "/");

    delete schema_location;
    schema_location = NULL;

    xerces_schema_path = XMLString::transcode(normalized_schema_location);


    delete normalized_schema_location;
    normalized_schema_location = NULL;


    if (parser_->getDomConfig()->canSetParameter(XMLUni::fgDOMNamespaces, true))
        parser_->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);

    if (parser_->getDomConfig()->canSetParameter(XMLUni::fgDOMDatatypeNormalization, true))
        parser_->getDomConfig()->setParameter(XMLUni::fgDOMDatatypeNormalization, true);

    if (parser_->getDomConfig()->canSetParameter(XMLUni::fgXercesSchema, true))
        parser_->getDomConfig()->setParameter(XMLUni::fgXercesSchema, true);

    if (parser_->getDomConfig()->canSetParameter(XMLUni::fgXercesHandleMultipleImports, true))
        parser_->getDomConfig()->setParameter(XMLUni::fgXercesHandleMultipleImports, true);

    if (parser_->getDomConfig()->canSetParameter(XMLUni::fgXercesSchemaFullChecking, true))
        parser_->getDomConfig()->setParameter(XMLUni::fgXercesSchemaFullChecking, true);

    if (parser_->getDomConfig()->canSetParameter(XMLUni::fgDOMValidate, true))
        parser_->getDomConfig()->setParameter(XMLUni::fgDOMValidate, true);
    
    if (parser_->getDomConfig()->canSetParameter(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, xerces_schema_path))
        parser_->getDomConfig()->setParameter(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, xerces_schema_path);
    
  }
}
