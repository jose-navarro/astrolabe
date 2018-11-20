/** \file astrolabe_string_utilities.hpp
  \brief Utilities to work with basic char* variables
  \ingroup astrolabe_string_utilities_group

  This file provides with a series of functions to manipulate
  basic C char* variables.

  The functions provided perform tasks as:

  - Concatenation.
  - Duplication.
  - Substring extraction and replacement.
  - Whitespace removal and trimming.
  - Lowercase to uppercase transformation and vice versa.
  - Conversion to several basic types (bool, long, double),
    assuming that the source string contains a valid representation of
    one of these values.
*/

#ifndef __ASTROLABE_STRING_UTILITIES_HPP__
#define __ASTROLABE_STRING_UTILITIES_HPP__

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <set>
using namespace std;

/// \brief Concatenate two strings, including an optional separator
/**
  \pre first  != NULL
  \pre second != NULL
  \param first First ("left") string to concatenate
  \param second Second ("right") string to concatenate
  \param separator Optional separator to insert between the first
    and second strings. It may be NULL; if this is the case, no
    separator is inserted between first and second.
  \return The concatenation of first and second, with separator
    inserted between these if it was not NULL. A NULL is returned
    if any of the preconditions is violated.
  */

char* astr_concat             (const char* first, const char* second,
                               const char* separator);

/// \brief Duplicate a string.
/**
  \pre source != NULL
  \param source String to duplicate.

  \return NULL if any of the preconditions is violated. Otherwise, a copy of
  the source string.
  */

char*  astr_dup               (const char* source);

/// \brief Extract a substring
/**
  \pre source != NULL
  \pre start <= end;
  \pre start >= 0;
  \pre end < strlen(source)
  \param source String to extract the substring from.
  \param start First character (numbered from 0) to extract.
  \param end Last character (numbered from 0) to extract.
  \return The substring going from characters start to end, both
   included, or NULL if any of the preconditions is violated.
*/

char* astr_extract            (const char* source, int start, int end);

/// \brief Remove all whitespace (blank, tab and newline characters) from a string.
/**

  \pre source != NULL
  \param source String to manipulate
  \return NULL if the precondition is violated or the resulting string would
               be empty after the whitespace removal. Otherwise, a new string
               where all the whitespace (blank, tab and newline characters)
               have been removed.
*/

char*  astr_remove_whitespace (const char* source);

/// \brief Replace all the occurrences of a substring by another substring.
/**

  \pre source != NULL
  \pre old_text != NULL
  \pre new_text != NULL

  \param source String to manipulate.
  \param old_text String whose occurrences within source have to be replaced
                  by new_string.
  \param new_text String that will replace all the occurrences of old_text in
                  the source string.

  \return NULL if any of the preconditions is violated. Otherwise, a new string
               where all the occurrences of old_string have been replaced by
               new_string.

  */

char*  astr_replace           (const char* source, const char* old_text, const char* new_text);

/// \brief String to boolean conversion
/**

  \pre string_to_convert != NULL
  \pre string_to_convert contains either "yes" or "no", either in upper, lower or
                         mixed case.

  \param string_to_convert String to convert.
  \param error_code        Will be set to zero if the conversion is successful,
                           or to 1 otherwise.

  \return True if string_to_convert contains "yes" and false if the contents of
          such parameter is "no". The return value is valid only if no errors
          are detected.

  */

int    astr_to_bool            (const char* string_to_convert, int* error_code);

/// \brief Converts a string containing an ASCII representation of a double number
/// to its binary form.
/**

  \pre string_to_convert != NULL
  \pre string_to_convert contains a valid ASCII representation of a double number.

  \param string_to_convert String to convert.
  \param error_code        Will be set to zero if the conversion is successful,
                           or to 1 otherwise.

  \return The binary representation of the double number stored in string_to_convert.
          The return value is valid only if no errors are detected.

  */

double astr_to_double         (const char* string_to_convert, int* error_code);

/// \brief Converts a string containing an ASCII representation of a long number
/// to its binary form.
/**

  \pre string_to_convert != NULL
  \pre string_to_convert contains a valid ASCII representation of a "long" number.

  \param string_to_convert String to convert.
  \param error_code        Will be set to zero if the conversion is successful,
                           or to 1 otherwise.

  \return The binary representation of the "long" number stored in string_to_convert.
          The return value is valid only if no errors are detected.

  */

long   astr_to_long           (const char* string_to_convert, int* error_code);

/// \brief Change the case to lowercase.
/**

  \pre string_to_convert != NULL

  \param string_to_convert String to convert.

  \return NULL if str_to_lowercase was also NULL. Otherwise, the input
          string converted to lowercase.


  */

char*  astr_to_lowercase      (const char* string_to_convert);

/// \brief Change the case to UPPERCASE.
/**

  \pre string_to_convert != NULL

  \param string_to_convert String to convert.

  \return NULL if str_to_lowercase was also NULL. Otherwise, the input
          string converted to UPPERCASE.


  */

char*  astr_to_uppercase      (const char* string_to_convert);

/// \brief Trim the whitespace surrounding a string
/**

  \pre string_to_convert != NULL

  \param string_to_convert String to convert.

  \return NULL if string_to_convert was also NULL. Otherwise, the input
          string with the surrounding whitespace removed.


  */

char*  astr_trim_whitespace   (const char* string_to_convert);

/**
\brief String tokenizer. Replaces the standard strtok function in the C library.

This class provides with a tokenizer for standard C character strings.

The idea behind this class is to overcome the problems suffered by
the default strtok standard function.

The following is an example showing how to use this class.

int
main
(void)
{
  {
    //
    // The next one is a string containing delimiters and "real" tokens.
    // It is the string that we want to analyze.
    //
    // Note that the words we are looking for are: "Keep", "calm"
    // "and", "carry", "on" (quotes excluded).
    //

    char* string_to_parse = "Keep   \t\t\n calm  \n\t\n  and carry\n\t  \n\t\n  on\t\t\t\n";

    //
    // We must select the set of characters that work as delimiters, that
    // is, these characters that are used to separate tokens (words) and
    // that, therefore, are not part of these.
    //
    // In the example below, we say that the newline, tab and white
    // characters are delimiters.
    //

    char* delimiters = "\n\t ";

    // Declare a string tokenizer to do the work.

    astr_tokenizer* tokenizer;

    //
    // We need a char* to retrieve the different tokens or words
    // returned by the string tokenizer.
    //

    char* single_token;

    //
    // Create the string tokenizer. Note how we pass the string that
    // we want to analyze as well as the set of characters that are
    // to be considered as delimiters.
    //

    tokenizer = new astr_tokenizer (string_to_parse, delimiters);

    //
    // To retrieve all the tokens in the string to parse, we must
    // iterate until method next_token() returns NULL. The
    // way to proceed is to ask for the first token in the string
    // and then work while its result is non-NULL.
    //

    // So we get the first token...

    single_token = tokenizer->next_token ();

    // ... and then iterate.

    while (single_token != NULL)
    {
      //
      // single_token contains a "word". Do whatever you need to do with it.
      // In this example, we just print it.
      //

      printf ("%s\n", single_token);

      //
      // Get rid of the token. Do it always or memory leaks will
      // infect your code!
      //

      free (single_token);

      // Try to get the next token. This might return NULL.

      single_token = tokenizer->next_token ();
    }

    //
    // At this point there are no more tokens available. We may
    // destroy the string tokenizer.
    //

    delete tokenizer;
  }
  return 0;
}
\endcode

*/

class astr_tokenizer
{
  public:

  /// \brief Retrieve the next token in the source string.
  /**
    \return The next token in the source string or NULL if
            no more tokens exist.

    The result is allocated internally by this method. It is
    the responsibility of the calling module to destroy it,
    using "free" (not "delete").
  */

    char* next_token        (void);

  /// \brief Default constructor
  /**
    \param to_parse The source string whose tokens must be found.
    \param delimiters The set of characters that must be considered
           as delimiters or separators.

    The calling module must set two input parameters: the char*
    containing the string to split into tokens as well as the
    set of characters that are defined as delimiters.

    These delimiters (for instance, tabs, new lines, white characters)
    will never be considered as a part of a token.

    The delimiters must be joined in a single input parameter, namely
    delimiters, as for instance, " \t\n".

    Both input parameters must be NULL-terminated.
  */

          astr_tokenizer    (const char* to_parse, const char* delimiters);

  /// \brief Destructor

          ~astr_tokenizer   (void);

  protected:

  ///
  /// \brief Check if the character in position "position" of the local
  /// copy of the string to parse is a delimiter (as defined by the the
  /// calling module).
  ///
  /**
    \param position The position, in the local copy of the string to parse,
           that must be checked.
    \return True if the character stored in position "position" of the
           local copy of the string to parse is a delimiter (as defined by
           the calling module), false otherwise.
  */

    bool  is_delimiter (int position) const;

  protected:

  /// \brief Position of the character being analyzed.

    int       current_char_;

  ///
  /// \brief Local set containing the delimiters set by the user. Used
  /// to speed up the search process.
  ///

    set<char> delimiters_;

  /// \brief Local copy of the input source string.

    char*     local_source_;

  ///
  /// \brief Length of the string being analyzed. Note that it does not
  /// include the ending NULL character.
  ///

    int       local_source_length_;

  /// Number of character in the separators_ array.

    int       total_delimiters_;
};


#endif // __ASTROLABE_STRING_UTILITIES_HPP__
