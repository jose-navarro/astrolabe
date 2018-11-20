/** \file adp_DOMparser.hpp
  \brief Generic, easy to use XML parser using DOM.
  \ingroup adp_DOMparser_group
*/

#ifndef __ADP_DOMPARSER_HPP__
#define __ADP_DOMPARSER_HPP__

#include "astrolabe_exceptions.hpp"
#include "astrolabe_string_utilities.hpp"

#include "adp_parsing_errors.hpp"

#include "adp_DOMparser_error_handler.hpp"
#include "adp_domtree_utilities.hpp"
#include "adp_stringx.hpp"

#include <stack>
using namespace std;

/// Generic, easy to use XML parser using DOM.
/**
    Use this class to parse an XML file using a pretty simplified interface.

    adp_DOMPARSER relies on the following libraries:

      - str_util,
      - str_util_cpp and
      - xerces-c 3.
  */

class adp_DOMparser
{
public:

 /// \brief Basic constructor
 /**
    \param xml_file_name Name of the XML file to parse.
    \throw astrolabe_exception Not enough memory to complete the request.

    Builds an DOM XML simple parser.

    After instantiating a new parser, the calling module should check
    whether the resulting object (parser) is valid. Several reasons,
    all of them related to the availability of the base parsing system,
    the xerces library, may arise. Please use the method is_parser_valid()
    to check this.

    If the parser object is valid after all, the parse() method may be
    then used to start the actual parsing process.

    \see is_parser_valid()

 */

                adp_DOMparser               (const char*  xml_file_name);

  /// \brief Destructor

                ~adp_DOMparser              (void);

  /// \brief Set the current label to the previous relative level.
  /**
    \pre (! is_parsing_done())
    \pre (is_backtracking_possible())
    \throw e_bs_precondition_violation One or more preconditions have been violated.

     This method must be used in combination with goto_label_relative(). This last
     method creates a "path" of current labels visited. Using go_back() is possible
     to go back one step in this "current label path" chain.

     For instance, assuming the next succession of goto* calls:

     \code

     int i;
     char* my_option;
     adp_DOMparser* dp;

     dp = new adp_DOMparser("my_xml_file.xml");
     ...

     index = 1;

     dp->goto_label("label1/label2",index);
     my_option = dp->_label_value_string(); // Requesting label2's value.
     ...

     dp->goto_label_relative("label3/label4/label5", index);
     my_option = dp->_label_value_string(); // Requesting label5's value.
     ...


     dp->goto_label_relative("label6/label7/label8/label9",index);
     my_option = dp->_label_value_string(); // Requesting label9's value.
     ...

     dp->go_back(); // Now the default label is label5.
     ...

     dp->go_back(); // And now, label2.
     ...

     dp->go_back(); // And now, we get an exception! We've gone too far...
     ...

     \endcode

     the current label after the goto_label() call would be "label2", after
     the first call to goto_label_relative(), "label5" and after the second
     call to the same method, it would be "label9". In fact, "label9" is the
     current label at this point. (the full current path is
     "label1/label2/label3/.../label8/label9").

     Invoking go_back() once would change the current label to "label5". A second
     call to this method would change it to "label2". One more would throw
     an exception, since no previous current label is stored.

     The combination of goto* + this method creates a convenient way to make
     backwards navigation easier. It is specially useful in looops when
     repeated (i.e., using label_index(es) greater than 1) have to be visited, since
     it avoids the necessity of re-issuing certain goto_label() or goto_label_relative()
     commands again.

     \see goto_label()
     \see goto_label_relative().

  */

  void             go_back                  (void);

  /// \brief Navigate to a given label, absolute mode.
  /**
    \pre (is_parsing_done())
    \pre (label_path != NULL)
    \pre (label_index > 0)
    \post (is_current_label_set())
    \param label_path Absolute (that is, starting from the root label) path of
                      labels to follow in the XML file. The label_index_th
                      occurrence of the last one will be set as default.
    \param label_index Used when more than one occurrence of the last label in the
                      label path exists. If only one is available, set this
                      parameter to 1. If two or more repeated labels exist,
                      indicate here the index (starting from 1) of the label's
                      occurrence of interest.
                      This parameter may be omitted. If so, it takes the default
                      value "1".
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw e_bs_postcondition_violation One or more postcondition have not been fullfilled.
    \throw astrolabe_exception The label_path + label_index combination does
                               not exist.

    Sets the current label using an absolute mode, that is, specifying a full
    path starting from the root tag in the XML file.

    Any previous path of current labels is reset. After invoking this method,
    it is possible to build a new path of current labels using goto_label_relative(),
    but the beginning of the path will be the label set by goto_label().

    It is not possible to use this method if any label in the path is multiple
    (that is, a set of repeated labels exists under a given parent label).
    Therefore, the longest path that may be specified to goto_label ends with
    the first repeated label.

    Paths must be specified using slashes (either forward or backward).

    \see go_back()
    \see goto_label_relative()


  */

  void             goto_label               (const char* label_path, int label_index=1);

  /// \brief Navigate to a given label, relative mode.
  /**
    \pre  (is_parsing_done())
    \pre  (is_current_label_set())
    \pre  (label_path != NULL)
    \pre  (label_index > 0)
    \post (is_current_label_set())
    \param label_path Relative (that is, starting from the current label) path of
                      labels to follow in the XML file. The label_index_th
                      occurrence of the last one will be set as default.
    \param label_index Used when more than one occurrence of the last label in the
                      label path exists. If only one is available, set this
                      parameter to 1. If two or more repeated labels exist,
                      indicate here the index (starting from 1) of the label's
                      occurrence of interest.
                      This parameter may be omitted. If so, it takes the default
                      value "1".
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw e_bs_postcondition_violation One or more postcondition have not been fullfilled.
    \throw astrolabe_exception The label_path + label_index combination does
                               not exist.

    Sets the current label using a relative mode, that is, specifying a path
    in the XML file starting from and previously set current label.

    It is possible to invoke this method as many times as required to navigate
    the XML file. However, the very first call to goto_label_relative()
    must have been preceded by at least one call to goto_label().

    The reason why is that since this method requires a preexisting current label,
    it may not be invoked if such current label has not been set. The only
    way to set a current label from scratch is goto_label().

    Each time that this method is invoked, the new current label is stored
    in a "current label path" keeping track of the relative navigation history. Using
    restore_current_label_relative() it is possible to navigate back to the
    previous relative label set. This is very useful in loops where a label
    has several repeated sublabels and each of these has to be processed. Setting
    as a relative current label the one that has repeated sons is very convenient,
    since it is possible to navigate back to this point with a very simple call.

    Note that as soon as goto_label() is used, the "current label
    path" is reset, so navigation history is lost. In fact, that's why
    that method (goto_label()) is "absolute".

    Paths must be specified using slashes (either forward or backward).

    \see is_current_label_set()
    \see go_back()
    \see goto_label()

  */

  void             goto_label_relative      (const char* label_path, int label_index=1);

  /// \brief Check if it is possible to navigate back using go_back()
  /**
    \return True if it is possible to use go_back().
  */

  bool             is_backtracking_possible (void) const;

  /// \brief Check if the current label is set.
  /**
    \return True if the current label has been set by any of the
    goto_label* methods in this class.

    This method is used mainly in other method's pre and postconditions.
  */

  bool             is_current_label_set     (void) const;

  /// \brief Check if the parsing process is over and finished correctly.
  /**

    \return True if the parsing process (invoked using parse()) has finished
    correctly.

    This method is used mainly as a precondition by other methods in this class.

    \see _parsing_errors()
    \see is_parsing_erroneous()
    \see parse()

  */

  bool             is_parsing_done          (void) const;

  /// \brief Check if the parsing process produced any errors.
  /**

    \return True if the parser has detected any parsing errors up to the moment,
    false otherwise.

    WARNING. This is a redundant method. All public methods in this class' API
    throw exceptions to signal that problems have been found. However, this
    method may be used at any time to recheck.

    Parsing is a non error-free process. The input file may contain incorrections,
    either syntactic or semantic.

    These errors may arise when the parse() method is invoked.

    To check whether any of these methods produced parsing errors, just invoke
    this method. If the answer is positive, _parsing_errors() may be used to
    retrieve the FULL list of errors detected up to the moment.

    \see _parsing_errors()
    \see is_parsing_done
    \see parse()

  */

  bool             is_parsing_erroneous     (void) const;

  /// \brief Check if a newly instantiated parser is valid.
  /**
    \return True if the parser is valid and ready to parse, false otherwise.

    When a new parser is created using the constructor (adp_DOMparser())
    it may become invalid, essentially by the lack of [parts of] the base
    Xerces system.

    Use this method to check if the parser is ready to be used right after
    its instantiation.

    \see adp_DOMparser()

  */

  bool             is_parser_valid          (void) const;

  /// \brief Retrieve the value of a named attribute of the current label as
  /// a boolean.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())
    \pre (attr_name != NULL)
    \param attr_name The name of the attribute whose string value is sought.

    \return The boolean value of the current label named attribute.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw e_algorithm_object_not_found The named attribute does not exist.
    \throw astrolabe_parse_error The text stored in the named attribute cannot be
           interpreted as a boolean value.

    Retrieve the value of a named attribute of the current label as a boolean.
    
    The valid equivalences between text and boolean values are fixed by the routine
    astr_to_bool() in the astrolabe_string_utilies package.

    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  bool          _label_attribute_bool       (const char* attr_name) const;

  /// \brief Retrieve the value of a named attribute of the current label as
  /// a double.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())
    \pre (attr_name != NULL)
    \param attr_name The name of the attribute whose string value is sought.

    \return The double value of the current label named attribute.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw e_algorithm_object_not_found The named attribute does not exist.
    \throw astrolabe_parse_error The text stored in the named attribute cannot be
           interpreted as a double value.

    Retrieve the value of a named attribute of the current label as a double. 

    \see _label_attribute_bool()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  double        _label_attribute_double     (const char* attr_name) const;

  /// \brief Retrieve the value of a named attribute of the current label as
  /// a float.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())
    \pre (attr_name != NULL)
    \param attr_name The name of the attribute whose string value is sought.

    \return The float value of the current label named attribute.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw e_algorithm_object_not_found The named attribute does not exist.
    \throw astrolabe_parse_error The text stored in the named attribute cannot be
           interpreted as a float value.

    Retrieve the value of a named attribute of the current label as a float. 

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  float         _label_attribute_float      (const char* attr_name) const;

  /// \brief Retrieve the value of a named attribute of the current label as
  /// an int(eger).
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())
    \pre (attr_name != NULL)
    \param attr_name The name of the attribute whose string value is sought.

    \return The int(eger) value of the current label named attribute.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw e_algorithm_object_not_found The named attribute does not exist.
    \throw astrolabe_parse_error The text stored in the named attribute cannot be
           interpreted as an int(eger) value.

    Retrieve the value of a named attribute of the current label as an int(eger). 

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  int           _label_attribute_int        (const char* attr_name) const;

  /// \brief Retrieve the value of a named attribute of the current label as
  /// a long.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())
    \pre (attr_name != NULL)
    \param attr_name The name of the attribute whose string value is sought.

    \return The long value of the current label named attribute.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw e_algorithm_object_not_found The named attribute does not exist.
    \throw astrolabe_parse_error The text stored in the named attribute cannot be
           interpreted as a long value.

    Retrieve the value of a named attribute of the current label as a long. 

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  long          _label_attribute_long       (const char* attr_name) const;

  /// \brief Retrieve the value of a named attribute of the current label as
  /// a string.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())
    \pre (attr_name != NULL)
    \param attr_name The name of the attribute whose string value is sought.

    \return The string value of the current label named attribute.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw astrolabe_exception The named attribute does not exist.

    Retrieve the value of a named attribute of the current label as a string.

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  char*         _label_attribute_string     (const char* attr_name) const;

  /// \brief Retrieve the value stored in the current label as
  /// a boolean.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())

    \return The boolean value associated to the current label text.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw astrolabe_parse_error The text stored in the label cannot be interpreted
                           as a boolean value.

    Retrieve the boolean equivalence of the text stored in the current label text.

    The valid equivalences between text and boolean values are fixed by the routine
    astr_to_bool() in the astrolabe_string_utilies package.

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  bool          _label_value_bool           (void) const;

  /// \brief Retrieve the value stored in the current label as
  /// a double
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())

    \return The double value associated to the current label text.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw astrolabe_parse_error The text stored in the label cannot be interpreted
                           as a double value.

    Retrieve the double equivalence of the text stored in the current label text.

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  double        _label_value_double         (void) const;

  /// \brief Retrieve the value stored in the current label as
  /// a float
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())

    \return The float value associated to the current label text.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw astrolabe_parse_error The text stored in the label cannot be interpreted
                           as a float value.

    Retrieve the float equivalence of the text stored in the current label text.

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  float         _label_value_float          (void) const;

  /// \brief Retrieve the value stored in the current label as
  /// an int(eger) value.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())

    \return The int(eger) value associated to the current label text.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw astrolabe_parse_error The text stored in the label cannot be interpreted
                           as an int(eger) value.

    Retrieve the int(eger) equivalence of the text stored in the current label text.

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  int           _label_value_int            (void) const;

  /// \brief Retrieve the value stored in the current label as
  /// a long value.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())

    \return The long value associated to the current label text.
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw astrolabe_parse_error The text stored in the label cannot be interpreted
                           as a long value.

    Retrieve the long equivalence of the text stored in the current label text.

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */

  long          _label_value_long           (void) const;

  /// \brief Retrieve the value stored in the current label as
  /// a string.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())

    \return The string value associated to the current label.
    \throw e_bs_precondition_violation One or more preconditions have been violated.

    Retrieve the string value associated to the current label. 

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see goto_label()
    \see goto_label_relative()

  */

  char*         _label_value_string         (void) const;
	
  /// \brief Number of identical sublabels for the current label.
  /**
    \pre (is_parsing_done())
    \pre (is_current_label_set())
    \pre (sublabel != NULL)
    \par sublabel The sublabel whose number of occurrences is sought.
    \throw e_bs_precondition_violation One or more preconditions have been
                                       violated.
    \return The number of identical sublabels of the current label. Zero
            is returned if no such sublabel is found.

    When a label has repeated, identical sublabels, it is necessary to know how
    many of these are there to be able to retrieve their values using, for
    instance, a loop with a sublabel index ranging from 1 to the number of
    repeated sublabels.

    This method assumes a current label, which can be set using any of the
    goto_label* methods in this class.

    \see goto_label()
    \see goto_label_relative()
  */

  int          _n_of_repeated_sublabels     (const char* sublabel) const;

  /// \brief Start the syntactic parsing of the o_set_file.
  /**
    \pre (is_parser_valid())
    \pre (! is_parsing_done())
    \throw e_bs_precondition_violation One or more preconditions have been violated.
    \throw e_bs_postcondition_violation One or more postconditions have not been
           fullfilled!
    \throw astrolabe_exception This exception is thrown whenever an
           error during the parsing of the input file is detected. To
           obtain the actual errors, use _parsing_errors().

    Once that a parser object has been built using the constructor
    adp_DOMparser() and it has proven valid (is_parser_valid()),
    it is possible to start the syntactic parsing process using
    this method, parse().

    This method may produce errors (because of mistakes in the input file).
    If this happens, the e_algorithm_error_detected exception is thrown.

    \see is_parser_valid()
    \see adp_DOMparser()

  */

  void            parse                     (void);

  /// Retrieve the list of parsing errors.
  /**

    \throws astrolabe_exception Not enough memory to complete the request.
    \return A pointer to a parsing_errors object containing the
            list of errors found by the parser during the
            parsing process.

    Unfortunatelly, parsing is a process that not always
    works, since the input file may contain syntactic or
    semantical errors.

    If errors have been detected during the parsing process
    (this may be checked using is_parsing_erroneous()), this
    method must be used to retrieve the list of problems found.

    \see is_parsing_erroneous()
  */

  adp_parsing_errors* _parsing_errors       (void);


  /// \brief Set the path to a validating schema
  /**
    \pre (is_parser_valid())
    \pre (!is_parsing_done())
    \pre (full_path_to_schema != NULL)
    \throw e_bs_precondition_violatin One or more preconditions have been violated.
    \param full_path_to_schema The path and filename of the file containing
      the schema to be used to validate the input file.

    The XML file parser has been conceived both as a development and
    production tool.

    While developing an XML file, it may be rather annoying to use
    a schema to validate, from the syntactical standpoint, the input
    XML file. Therefore, it is much more confortable to avoid
    the use of validating schemes.

    On the contrary, once that the structure of the XML file is
    finished, it is highly recommended that a schema be used in order
    to minimize the risk of syntactical errors.

    Using this method it is possible to specify where to find the validating
    schema. Once set, the parser will use it. This means that if no validation
    is required, set_schema() must not be invoked at all.

    Note that this method, if required, must be invoked just after
    the instantiation of the new adp_DOMparser but before the
    method parse() is used.

    Note also that parameter full_path_to_schema must always contain an absolute
    full path, even when the schema is located in the so-called "working
    directory". This means that full_path_to_schema may NOT contain
    only a filename as, for instance, "my_schema.xsd". If this rule is
    violated, the schema file will not be found by the parser and the
    parsing process will fail, therefore returning errors.

  */

  void            set_schema                (const char* full_path_to_schema);

private:

  /// \brief Retrieve the value stored in the current label, not reinterpreting
  /// it. This method is the base for all the _label_value_*() methods included
  /// in this class.

  /**
    \return The string value associated to the current label.

    Retrieve the string value associated to the current label. No reintepretation
    of such string is made.

    \see _label_attribute_bool()
    \see _label_attribute_double()
    \see _label_attribute_float()
    \see _label_attribute_int()
    \see _label_attribute_long()
    \see _label_attribute_string()
    \see _label_value_bool()
    \see _label_value_double()
    \see _label_value_float()
    \see _label_value_int()
    \see _label_value_long()
    \see _label_value_string()
    \see goto_label()
    \see goto_label_relative()

  */
  char*           _label_value              (const DOMNode* label) const;

  /**
      \throw astrolabe_exception Not enough memory to complete the request.
   */
  DOMNode*        navigate_to_label         (DOMNode* start, const char* label_path, int label_index) const;

private:

  /// \brief Current base label.

  DOMNode                                    *current_label_;

  /// \brief Current base label path.

  stack<DOMNode*>                            current_label_path_;

  /// \brief Document root node; directly related to Xerces and DOM structures
  /// for XML parsing.

  XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc_;

  /// \brief DOM tree utilities object, used to perform several utilitary functions
  /// related to DOM processing.

  adp_domtree_utilities                      du_;

  /// \brief Error handler used by the basic Xerces parser.

  adp_DOMparser_error_handler                *error_handler_;

  /// \brief List of errors detected during the parsing process.

  adp_parsing_errors                         error_list_;

  /// \brief Flag used to signal that error(s) has(ve) been detected and that
  /// error_list_ should be checked.

  bool                                       error_occurred_;

  /// \brief Auxiliary buffer used to format error messages.

  char                                       error_text_[2000];

  /// \brief The Xerces parser itself.

  DOMLSParser                                *parser_;

  /// \brief Flag used to signal whether the parsing process is over and OK.

  bool                                       parsed_;

  /// \brief Path to the schema to use, when requested.

  char*                                      path_to_schema_;


  /// \brief Flag used to signal whether the parser_ object is valid or not.

  bool                                       valid_;

  /// \brief The path and file name of the file to parse.

  char                                       *xml_file_;
};

#endif // __ADP_DOMPARSER_HPP__
