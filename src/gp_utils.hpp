/** \file gp_utils.hpp
  \brief Utility class providing several useful tools to help the parsing process.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_UTILS_HPP_
#define _GP_UTILS_HPP_

#include "adp_DOMparser.hpp"
#include "gp_clist.hpp"
#include "gp_covariance.hpp"
#include "gp_ispec.hpp"
#include "gp_lspec.hpp"
#include "gp_mspec.hpp"
#include "gp_pspec.hpp"
#include "gp_ref.hpp"
#include "gp_ref_code.hpp"
#include "gp_ref_code_element.hpp"
#include "gp_scale.hpp"
#include "gp_tspec.hpp"
#include "gp_timespec.hpp"
#include "gp_unit.hpp"
#include "lineage_data.hpp"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

// We declare this class here instead of including the corresponding
// header to avoid a name clash produced by one of the headers
// used by such class, that of UDUNITS2 package. We'll include
// the header in the cpp file.

class gp_units_manager;

/// \brief Utility class providing several useful tools to help the parsing process.

class gp_utils
{
  public:

    /// \brief Utility function to format error messages.
    /**
      \param error_level Warning (value = 1) or error (value = 2);
      \param offending_file File name of the file where the error has
             been detected.
      \param tag Name of the tag or tags where an warning or error has
             been detected.
      \param description Textual description of the warning or error.
      \return The formatted warning or error message

      Use this protected method to format warning and error messages
      in an uniform way.

      Note that parameter description is a vector of strings. This is
      useful to format multi-line error messages. Each line of the
      message must be stored in one of the components of the vector, first
      line in component 0, last line in component n-1, where n is the
      total number of lines in the message.
    */

    string build_message                           (int             error_level,
                                                    const string&   offending_file,
                                                    string&         tag,
                                                    vector<string>& description);

    /// \brief Utility function to format error messages.
    /**
      \param error_level Warning (value = 1) or error (value = 2);
      \param offending_files File names of the files where the error has
             been detected.
      \param tag Name of the tag or tags where an warning or error has
             been detected.
      \param description Textual description of the warning or error.
      \return The formatted warning or error message

      Use this protected method to format warning and error messages
      in an uniform way.

      This version takes a series of file names as the origin of the problem
      instead just a single one. This is so because in some situations
      an error is detected when a series of files are processed together,
      so the error may not be assigned to a single file.

      Note that parameter description is a vector of strings. This is
      useful to format multi-line error messages. Each line of the
      message must be stored in one of the components of the vector, first
      line in component 0, last line in component n-1, where n is the
      total number of lines in the message.
    */

    string build_message                           (int                   error_level,
                                                    const vector<string>& offending_files,
                                                    string&               tag,
                                                    vector<string>&       description);

    /// \brief Constructor.

                    gp_utils                       (void);

    /// \brief Destructor.

                    ~gp_utils                      (void);

    /// \brief Check if a given character is considered as whitespace.
    /**
      \param c The character to check.
      \return True if c is considered whitespace, false otherwise.

      Note that the characters considered as whitespace are:

      - ' '
      - '\\t'
      - '\\r'
      - '\\n'
      - '\\v'
      - '\\f'
    */

    bool            is_whitespace                  (const char c);

    /// \brief Parses a c_list (\<c_list\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (units_manager != NULL)
      \pre (units_manager->is_valid())
      \pre (!lineage_id.empty())
      \pre (clist != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param units_manager The gp_units_manager object used to validate the
             syntax of the units included in the  \<c_list\> element. It
             must be valid (that is, it must have been conveniently initialized).
      \param lineage_id Identifier of the \<lineage\> element in which the
             \<c_list\> element to parse is included.
      \param clist The gp_clist object to be set once the parsing of the
             c_list element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<c_list\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */
    int             parse_clist                    (const string&     filename,
                                                    adp_DOMparser*    dp,
                                                    gp_units_manager* units_manager,
                                                    string&           lineage_id,
                                                    gp_clist*         clist,
                                                    vector<string>&   error_messages);

    /// \brief Parses a covariance (\<c\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (!tag_parsed.empty())
      \pre (!lineage_id.empty())
      \pre (cov != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param tag_parsed The tag being parsed. This is used to build properly
             any error messages that may arise.
      \param lineage_id The value of the identifier present in the \<lineage\>
             element related to the entity (l_spec, p_spec, i_spec) containing
             the covariance tag being parsed. Used to build properly any
             arror messages that may arise.
      \param cov The covariance object to be set once the parsing of the
             covariance element is done.
      \param error_message If a parsing error is detected, this parameter will
             contain the text describing it; otherwise, it will be empty. Note
             that its contents is always modified!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<c\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_covariance               (const string&  filename,
                                                    adp_DOMparser* dp,
                                                    const string&  tag_parsed,
                                                    const string&  lineage_id,
                                                    gp_covariance* cov,
                                                    string&        error_message);


    /// \brief Parses a i_list (\<i_list\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (!lineage_id.empty())
      \pre (ilist != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param lineage_id Identifier of the \<lineage\> element in which the
             \<i_list\> element to parse is included.
      \param ilist The gp_ilist object to be set once the parsing of the
             i_list element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<i_list\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_ilist                    (const string&   filename,
                                                    adp_DOMparser*  dp,
                                                    string&         lineage_id,
                                                    gp_ilist*       ilist,
                                                    vector<string>& error_messages);

    /// \brief Parses the specification of the instrument (\<i_spec\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (units_manager != NULL)
      \pre (units_manager->is_valid())
      \pre (ispec != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param units_manager The gp_units_manager object used to validate the
             syntax of the \<units\> elements being validated. It must be valid
             (that is, it must have been conveniently initialized).
      \param ispec The instrument specification object to be set once the
             parsing of the i_spec element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<i_spec\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_ispec                    (const string&     filename,
                                                    adp_DOMparser*    dp,
                                                    gp_units_manager* units_manager,
                                                    gp_ispec*         ispec,
                                                    vector<string>&   error_messages);

    /// \brief Parses a \<lineage\> element.
    /**
      \pre (dp != NULL)
      \pre (lineage != NULL)
      \param dp The adp_DOMparser object used to parse the input file.
      \param lineage The lineage object to be set once the parsing of the
             lineage element is done.
      \return True if the parsing finished correctly, false otherwise (either
             because one or more preconditions are violated or because of other
             errors in the input file itself).

      Note that the adp_DOMparser object must have been positioned just
      on the \<lineage\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    bool            parse_lineage                  (adp_DOMparser* dp,
                                                    lineage_data*  lineage);

    /// \brief Parses a l_list (\<i_list\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (!lineage_id.empty())
      \pre (llist != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param lineage_id Identifier of the \<lineage\> element in which the
             \<l_list\> element to parse is included.
      \param llist The gp_llist object to be set once the parsing of the
             l_list element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<l_list\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_llist                    (const string&   filename,
                                                    adp_DOMparser*  dp,
                                                    string&         lineage_id,
                                                    gp_llist*       llist,
                                                    vector<string>& error_messages);

    /// \brief Parses the specification of the observation (\<l_spec\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (units_manager != NULL)
      \pre (units_manager->is_valid())
      \pre (lspec != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param units_manager The gp_units_manager object used to validate the
             syntax of the \<units\> elements being validated. It must be valid
             (that is, it must have been conveniently initialized).
      \param lspec The observation specification object to be set once the
             parsing of the l_spec element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<l_spec\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_lspec                    (const string&     filename,
                                                    adp_DOMparser*    dp,
                                                    gp_units_manager* units_manager,
                                                    gp_lspec*         lspec,
                                                    vector<string>&   error_messages);

    /// \brief Parses the specification of the model (\<m_spec\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (mspec != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param mspec The model specification object to be set once the
             parsing of the m_spec element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<m_spec\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_mspec                    (const string&   filename,
                                                    adp_DOMparser*  dp,
                                                    gp_mspec*       mspec,
                                                    vector<string>& error_messages);

    /// \brief Parses a p_list (\<p_list\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (!lineage_id.empty())
      \pre (plist != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param lineage_id Identifier of the \<lineage\> element in which the
             \<p_list\> element to parse is included.
      \param plist The gp_plist object to be set once the parsing of the
             p_list element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<p_list\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_plist                    (const string&   filename,
                                                    adp_DOMparser*  dp,
                                                    string&         lineage_id,
                                                    gp_plist*       plist,
                                                    vector<string>& error_messages);

    /// \brief Parses the specification of the parameter / state (\<p_spec\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (units_manager != NULL)
      \pre (units_manager->is_valid())
      \pre (pspec != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param units_manager The gp_units_manager object used to validate the
             syntax of the \<units\> elements being validated. It must be valid
             (that is, it must have been conveniently initialized).
      \param pspec The parameter / state specification object to be set once the
             parsing of the p_spec element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<p_spec\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_pspec                    (const string&     filename,
                                                    adp_DOMparser*    dp,
                                                    gp_units_manager* units_manager,
                                                    gp_pspec*         pspec,
                                                    vector<string>&   error_messages);

    /// \brief Parses a \<ref\> element.
    /**
      \pre (dp != NULL)
      \pre (ref != NULL)
      \param dp The adp_DOMparser object used to parse the input file.
      \param ref The gp_ref object to be set once the parsing of the
             ref element is done.
      \return True if the parsing finished correctly, false otherwise (either
             because one or more preconditions are violated or because the
             list of codes for coordinate systems, reference frames or
             coordinate reference systems are erroneous, that is, do
             not adhere to the proper syntax defined in the ICD).

      Note that the adp_DOMparser object must have been positioned just
      on the \<ref\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    bool            parse_ref                      (adp_DOMparser* dp,
                                                    gp_ref*        ref);

    /// \brief Parse a string containing a list of coordinate systems (CS),
    /// Reference Frames (RF) or Coordinate Reference Frame (CRF).
    /**
    \param code_list_text A string containing a comma separated list of Coordinate System (CS),
    Reference Frame (RF) or Coordinate Reference Frame (CRF) codes. Note that there may
    be a single code, thus not been necessary the commas to separate it.
    \param the_codes An array containing the above CS, RF or CRF codes in
    gp_ref_code format.
    \return An error code. The possible values are:
    - 0 - Successful completion
    - 1 - No codes found in input string.
    - 2 - At least one code is empty (but may include a single '+')
    - 3 - The parentheses found in at least one code do not match.
    - 4 - Empty parameter list in code element.
    - 5 - No code found, only a list of parameters.
    - 6 - Extra commas found in the input string.
    - 7 - Extra plus signs found in the input string.

    Use this method to parse a list of either Coordinate System, Reference Frame
    or Coordinate Reference Frame codes found in \<ref\> tags.

    The input is a comma separated list of these codes. The codes themselves
    must adhere to the syntax described in the ASTROLABE ICD.

    Note that the contents of the output parameter the_codes is undefined
    if an error condition (see the description of the return value) is
    detected. Thus, the_codes must not be used when the error code returned
    is not 0.
    */

    int             parse_ref_code_list            (const string&  code_list_text,
                                                    vector<gp_ref_code>& the_codes);

    /// \brief Parses a scale (\<s\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (!tag_parsed.empty())
      \pre (!lineage_id.empty())
      \pre (s != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param tag_parsed The tag being parsed. This is used to build properly
             any error messages that may arise.
      \param lineage_id The value of the identifier present in the \<lineage\>
             element related to the entity (l_spec, p_spec, i_spec) containing
             the covariance tag being parsed. Used to build properly any
             arror messages that may arise.
      \param scale The scale object to be set once the parsing of the
             scale element is done.
      \param error_message If a parsing error is detected, this parameter will
             contain the text describing it; otherwise, it will be empty. Note
             that its contents is always modified!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<s\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_scale                    (const string&  filename,
                                                    adp_DOMparser* dp,
                                                    const string&  tag_parsed,
                                                    const string&  lineage_id,
                                                    gp_scale*      scale,
                                                    string&        error_message);

    /// \brief Parses a sub-m_list (\<sub-m_list\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (!lineage_id.empty())
      \pre (smlist != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param lineage_id Identifier of the \<lineage\> element in which the
             \<sub-m_list\> element to parse is included.
      \param smlist The gp_sub_m_list object to be set once the parsing of the
             \<sub-m_list\> element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<sub-m_list\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_sub_m_list               (const string&   filename,
                                                    adp_DOMparser*  dp,
                                                    string&         lineage_id,
                                                    gp_sub_m_list*  smlist,
                                                    vector<string>& error_messages);

    /// \brief Parses the specification of the time (\<t_spec\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (units_manager != NULL)
      \pre (units_manager->is_valid())
      \pre (!tag_parsed.empty())
      \pre (!lineage_id.empty())
      \pre (tspec != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param units_manager The gp_units_manager object used to validate the
             syntax of the \<units\> elements being validated. It must be valid
             (that is, it must have been conveniently initialized).
      \param tag_parsed The XML tag being parsed. This is used to build properly
             any error messages that may arise.
      \param lineage_id The value of the identifier present in the \<lineage\>
             element related to the entity (l_spec, p_spec) containing the time_spec
             tag being parsed. Used to build properly any error messages that may arise.
      \param tspec The time specification object to be set once the
             parsing of the t_spec element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<t_spec\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_timespec                 (const string&     filename,
                                                    adp_DOMparser*    dp,
                                                    gp_units_manager* units_manager,
                                                    const string&     tag_parsed,
                                                    const string&     lineage_id,
                                                    gp_timespec*      tspec,
                                                    vector<string>&   error_messages);

    /// \brief Parses the specification of tag values (\<t_spec\>) element.
    /**
      \pre (!filename.empty())
      \pre (dp != NULL)
      \pre (units_manager != NULL)
      \pre (units_manager->is_valid())
      \pre (!tag_parsed.empty())
      \pre (!lineage_id.empty())
      \pre (tspec != NULL)
      \param filename The file name of the file being parsed.
      \param dp The adp_DOMparser object used to parse the input file.
      \param units_manager The gp_units_manager object used to validate the
             syntax of the \<units\> elements being validated. It must be valid
             (that is, it must have been conveniently initialized).
      \param tag_parsed The XML tag being parsed. This is used to build properly
             any error messages that may arise.
      \param lineage_id The value of the identifier present in the \<lineage\>
             element related to the entity (l_spec, p_spec, i_spec) containing
             the t_spec tag being parsed. Used to build properly any
             error messages that may arise.
      \param tspec The tag values specification object to be set once the
             parsing of the t_spec element is done.
      \param error_messages If parsing errors are detected, this vector will
             contain the text describing them; otherwise, it will be empty. Note
             that its contents is cleared when this method is called!
      \return 0 if the parsing finished correctly; 1 if any of the preconditions
             are violated; 2 if errors are detected while parsing the element.

      Note that the adp_DOMparser object must have been positioned just
      on the \<t_tag\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_tspec                    (const string&     filename,
                                                    adp_DOMparser*    dp,
                                                    gp_units_manager* units_manager,
                                                    const string&     tag_parsed,
                                                    const string&     lineage_id,
                                                    gp_tspec*         tspec,
                                                    vector<string>&   error_messages);


    /// \brief Parses a units (\<units\>) element.
    /**
      \pre (dp != NULL)
      \pre (units_manager != NULL)
      \pre (units_manager->is_valid())
      \pre (dimension > 0)
      \pre (units != NULL)
      \param dp The adp_DOMparser object used to parse the input file.
      \param units_manager The gp_units_manager object used to validate the
             syntax of the unit being validated. It must be valid (that is,
             it must have been conveniently initialized).
      \param dimension The amount of units that should be parsed.
      \param units The units object to be set once the parsing of the
             units element is done.
      \return An error code. 0 means successful completion. 1 means that
             one or more preconditions have been violated. 2 stands for
             an invalid number of units found in the \<units\> element
             - that is, such number does not match with the value of the
             dimension parameter - and, finally, 3 means that one or more
             of the units found in the \<units\> element are syntactically
             invalid.

      Note that the adp_DOMparser object must have been positioned just
      on the \<units\> tag to be able to work. This must be done using
      any of the goto methods available in the adp_DOMparser class.
    */

    int             parse_units                    (adp_DOMparser*    dp,
                                                    gp_units_manager* units_manager,
                                                    int               dimension,
                                                    gp_unit*          units);

    /// \brief Converts a string containing a double value in text form to double.
    /**
      \pre (!numberAsString.empty())
      \param numberAsString The double in text (string) format to convert.
      \param result The double value of the input string.
      \return True if the conversion is successful, false otherwise.
    */
    bool            string_convert_to_double       (string& numberAsString, double& result);

    /// \brief Converts a string containing an int value in text form to int.
    /**
      \pre (!numberAsString.empty())
      \param numberAsString The int in text (string) format to convert.
      \param result The int value of the input string.
      \return True if the conversion is successful, false otherwise.
    */
    bool            string_convert_to_int          (string& numberAsString, int& result);

    /// \brief Splits a string into several tokens or elements using a
    /// user-defined separator.
    /**
      \param source The string to split.
      \param separator The character to be considered as separator and
             determine how to split the source string into separate tokens.
      \return A vector of strings; each component contains the components
             the source string has been split into.

      This method does NOT remove any whitespace.

      For instance, if the source string is:

      '   token1   ,token2  ,     token3'

      and the separator character is a comma (','),

      elements 0 to 2 in the output vector of strings would be:

      - '   token1   '
      - 'token2  '
      - '     token3'

    */

    vector<string>& string_tokenize                (string& source, const char separator);

    /// \brief Removes all whitespace in the source string.
    /**
      \param source The string whose whitespace character have to be removed.
      \return A string with no whitespace characters at all.

      Note that 'whitespace' stands for any character recongnized as whitespace by
      method is_whitespace().

      Example:

      If the source string contains:

      '   \\t\\r   This is\\t\\n a text \\f \\v full of whitespace characters\\r\\n   '

      the output of this method would be

      'Thisisatextfullofwhitespacecharacters'
    */

    string&         string_whitespace_remove_all   (string& source);

    /// \brief "Simplifies" the whitespace included in a source string.
    /**

      "Simplifying" a string means:

      - Removing any leading or trailing whitespace characters,
      - Substituting any sequence of whitespace characters by a single blank character.

      where the set of "whitespace characters" contains those defined by
      method is_whitespace().

      Example:

      If the source string contains:

      '   \\t\\r   This is\\t\\n a text \\f \\v full of whitespace characters\\r\\n   '

      the output of this method would be

      'This is a text full of whitespace characters'
    */

    string&         string_whitespace_simplify     (string& source);

    /// \brief Validate the syntax and semantics or the \<item\> elements
    /// included in the \<i_spec\>\<c_list\> tag.
    /**
      \param the_item The string containing the value of the \<item\>
             to validate.
      \param is_scalar Flag that, on output, indicates whether the
             item validated is scalar or not (thus, a matrix).
      \param dimensions Valid only when is_scalar becomes false. List
             of dimensions of the matrix item.
      \return True if the value of the item is syntactically and
             semantically correct, false otherwise.
    */

    bool            validate_ispec_clist_item_type (string&      the_item,
                                                    bool&        is_scalar,
                                                    vector<int>& dimensions);

    /// \brief Validate the semantics of a \<ref\> element.
    /**
      \param ref The \<ref\> element to validate.
      \return True if the \<ref\> element is correctly built, false otherwise.

      Check that the coordinate reference frame codes or, equivalently,
      coordinate systems plus reference frames codes match the specification.

      There is nothing to check when coordinate reference systems are used,
      because in this case the syntax of the codes has been validated
      previously.

      On the contrary, when using a combination of coordinate systems plus reference frames,
      the number of elements in both lists must always match. Note that method
      parse_rf takes care of building a list of "null" ("-") coordinate system
      codes when these are not specified in the input file (these are optional).

      So, in short, this method will return false when using reference frames
      plus coordinate system lists and the lengths of these do not match.
      True will be returned in all the remaining situations.
    */

    bool            validate_ref                   (gp_ref* ref);

};

#endif // _GP_UTILS_HPP_
