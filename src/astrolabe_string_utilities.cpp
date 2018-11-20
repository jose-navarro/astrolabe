/** \file astrolabe_string_utilities.cpp
  \brief Implementation file for astrolabe_string_utilities.hpp
  \ingroup astrolabe_string_utilities_group
*/

#include "astrolabe_string_utilities.hpp"
#include <stdio.h>

char*
astr_concat
(const char* first,
 const char* second,
 const char* separator)
{
  char* result;

  if (first  == NULL) return NULL;
  if (second == NULL) return NULL;
  {
    int l1, l2, lsep;
    int ltotal;

    l1   = (int)strlen(first);
    l2   = (int)strlen(second);
    lsep = 0;

    if (separator != NULL) lsep = (int)strlen(separator);

    ltotal = l1 + l2 + lsep + 1;

    result = (char*)malloc(ltotal);

    strcpy(result, first);
    if (lsep != 0) strcat(result, separator);
    strcat(result, second);
  }
  return result;
}

char*
astr_dup
(const char* source)
{
  char* result;

  if (source == NULL) return NULL;
  {
    int lon;
    lon = (int)strlen(source) + 1;
    result = (char*)malloc(lon);
    strcpy(result,source);
  }
  return result;
}

char*
astr_extract
(const char* source,
 int start,
 int end)
{
  char* result;

  if (source == NULL)             return NULL;
  if (start < 0)                  return NULL;
  if (start > end)                return NULL;
  if (end >= (int)strlen(source)) return NULL;

  {
    int i,j;
    int lon;

    lon = end - start + 2;
    result = (char*)malloc(lon);

    j = 0;
    for (i=start; i<=end; i++)
    {
      result[j] = source[i];
      j++;
    }
    result[j] = '\0';
  }
  return result;
}

char*
astr_remove_whitespace
(const char* source)
{
  char* result;
  {
    int input_len, new_len;
    int total_whitespace;
    int i,j;
    
    input_len = (int)strlen(source);
    
    total_whitespace = 0;
    for (i = 0; i < input_len; i++)
    {
      if (source[i] == ' ' ) total_whitespace++;
      if (source[i] == '\t') total_whitespace++;
      if (source[i] == '\n') total_whitespace++;
    }
   
    new_len = input_len - total_whitespace;
    
    if (new_len == 0) return NULL;
    
    result = (char*)malloc( (new_len+1) * sizeof(char));
    
    j = 0;
    for (i = 0; i < input_len; i++)
    {
      if ( (source[i] != ' ' ) &&
           (source[i] != '\t') &&
           (source[i] != '\n') )
      {
        result[j++] = source[i];
      }
   }
    result[new_len] = '\0';
  }
  
  return result;
}

char*
astr_replace
(const char* source, const char* old_text, const char* new_text)
{
 char* result;
 
 if (source == NULL) return NULL;
 if (old_text == NULL) return NULL;
 if (new_text == NULL) return NULL;
 {
  int l_source;
  int l_old;
  int l_new;
  int l_inc;
  int l_result;
  int pos;
  int pos2;
  int total_found;
  int i;
  
  int found;
  
  l_source = (int)strlen(source);
  l_old    = (int)strlen(old_text);
  l_new    = (int)strlen(new_text);
  l_inc    = l_new - l_old;
  
  total_found = 0;
  found = 0;
  
  for (pos = 0; pos < l_source-(l_old-1); pos++)
  {
   if (source[pos] == old_text[0])
   {
    found = 1;
    for (i=1; i<l_old; i++)
    {
      if (source[pos+i] != old_text[i])
      {
       found = 0;
       break;
      }
    }
    if (found==1)
    {
     total_found++;
     found = 0;
    }
   }
  }
  
  if (total_found == 0)
  {
   result = strdup(source);
   return result;
  }
  
  l_result = l_source + (total_found * l_inc);
  result = (char*) malloc(l_result+1);
  result[l_result] = '\0';
  
  found = 0;
  pos   = 0;
  pos2  = 0;
  
  while (pos < l_source)
  {
   if (source[pos] != old_text[0])
   {
    result[pos2] = source[pos];
    pos++;
    pos2++;
   }
   else
   {
    found = 1;
    for (i=1; i<l_old; i++)
    {
     if ((pos+i) < l_source)
     {
      if (source[pos+i] != old_text[i])
      {
       found = 0;
       break;
      }
     }
    }
    if (found==1)
    {
     for (i = 0; i < l_new; i++)
     {
      result[pos2+i] = new_text[i];
     }
     pos  += l_old;
     pos2 += l_new;
     found = 0;
    }
    else
    {
     result[pos2] = source[pos];
     pos++;
     pos2++;
    }
   }
  }
  return result;
 }
}

int
astr_to_bool
(const char* string_to_convert, int* error_code)
{
  int result;

  if (string_to_convert == NULL)
  {
    *error_code = 1;
    result = 0;
    return result;
  }

  {
    char* lowercase;

    *error_code = 0;

    lowercase = astr_to_lowercase(string_to_convert);

    if (strcmp(lowercase,"yes") == 0)
    {
      result = 1;
    }
    else if (strcmp(lowercase, "no") == 0)
    {
      result = 0;
    }
    else
    {
      result = 0;
      *error_code = 1;
    }

    free(lowercase);
    lowercase = NULL;

  }
  return result;
}

double
astr_to_double
(const char* string_to_convert, int* error_code)
{
  double result;

  if (string_to_convert == NULL)
  {
    *error_code = 1;
    result = 0.0;
    return result;
  }

  {
    char* token;
    char* stopScan;

    token = strdup(string_to_convert);

    stopScan = NULL;
    result = strtod(token, &stopScan);
    if (token != stopScan)
    {
      *error_code = 0;
    }
    else
    {
      result = 0.0;
      *error_code = 1;
    }

    free(token);
    token = NULL;

  }
  return result;
}

long
astr_to_long
(const char* string_to_convert, int* error_code)
{
  long result;

  if (string_to_convert == NULL)
  {
    *error_code = 1;
    result = 0;
    return result;
  }

  {
    char* token;
    char* stopScan;

    token = strdup(string_to_convert);

    stopScan = NULL;
    result = strtol(token, &stopScan, 10);
    if (token != stopScan)
    {
      *error_code = 0;
    }
    else
    {
      result = 0;
      *error_code = 1;
    }

    free(token);
    token = NULL;
  }
  return result;
}

//
// Change the letters in a string from upper to lowercase.
//

char*
astr_to_lowercase
(const char* string_to_convert)
{
  char* result;

  if (string_to_convert == NULL) return NULL; // EXCEPTION !!!

  {
    int len;
    int i;

    len = (int)strlen(string_to_convert);
    result = (char*)malloc(len+1);

    for (i=0; i<len; i++) result[i] = tolower(string_to_convert[i]);

    result[len] = '\0';
  }
  return result;
}

//
// Change the letters in a string from upper to lowercase.
//

char*
astr_to_uppercase
(const char* string_to_convert)
{
  char* result;

  if (string_to_convert == NULL) return NULL; // EXCEPTION !!!

  {
    int len;
    int i;

    len = (int)strlen(string_to_convert);
    result = (char*)malloc(len+1);

    for (i=0; i<len; i++) result[i] = toupper(string_to_convert[i]);

    result[len] = '\0';
  }
  return result;
}

//
// Trim the whitespace surrounding a string
//

char*
astr_trim_whitespace
(const char* charToTrim)
{
  char* trimmedChar;

  if (charToTrim==NULL) return NULL; // EXCEPTION!!!

  {
    int   lon, lon2;
    int   i,j,k;
    char  test;

    lon = (int)strlen(charToTrim);

    for (i=0; i<lon; i++)
    {
      test = charToTrim[i];
      if ( (test != ' ') &&
           (test != '\t') ) break;
    }

    for (j=lon-1; j>=0; j--)
    {
      test = charToTrim[j];
      if ( (test != ' ') &&
           (test != '\t') ) break;
    }

    if (i>j)
    {
      trimmedChar = NULL;
    }

    else
    {
      lon2 = j-i+1;
      trimmedChar = (char*) malloc(lon2+1);
      for (k=0; k<lon2; k++)
      {
        trimmedChar[k] = charToTrim[i];
        i++;
      }
      trimmedChar[lon2] = '\0';
    }
  }
  return trimmedChar;
}

// Methods for class astr_tokenizer.

bool
astr_tokenizer::
is_delimiter
(int position)
const
{
  {
    int i;

    for (i = 0; i < total_delimiters_; i++)
    {
      if (delimiters_.end () != (delimiters_.find (local_source_[position]))) return true;
    }
  }
  return false;
}

char*
astr_tokenizer::
next_token
(void)
{
  char* result;

  {
    int i, j, k;
    int token_length;

    //
    // Skip the delimiters that might be present before a non-delimiter
    // character.
    //

    i = current_char_;

    while ((i < local_source_length_) && (is_delimiter (i))) i++;

    //
    // At this point, we might have reached the end of the string to
    // parse. If so, there are no more tokens; therefore, we'll return
    // the NULL character.

    if (i == local_source_length_) return NULL;

    //
    // Here we'll find AT LEAST one character not included in the
    // set of (user) delimiters. But there might be more. Let's
    // find how many.
    //

    current_char_ = i + 1;

    while ((current_char_ < local_source_length_) && (!is_delimiter (current_char_))) current_char_++;

    //
    // Well, we have at least one character (maybe more) that constitute
    // a token.
    //
    // Let's instantiate the result, making enough room to hold as
    // many characters as we have found PLUS one more for the
    // terminating NULL character.
    //


    token_length = (current_char_ - 1) - i + 1;
    result = (char*)malloc ((token_length + 1) * sizeof (char));

    //
    // Set the result, copying the characters found in the string
    // to parse.
    //

    j = 0;

    for (k = i; k <= current_char_; k++) result[j++] = local_source_[k];

    // Don't forget to append the NULL terminating character.

    result[token_length] = '\0';
  }

  return result;
}

astr_tokenizer::
astr_tokenizer
(const char* to_parse,
 const char* delimiters)
{
  {
    int i;

    // Get the total number of delimiters.

    total_delimiters_ = (int)strlen (delimiters);

    // Insert the delimiters in our local set.

    for (i = 0; i < total_delimiters_; i++) delimiters_.insert (delimiters[i]);

    // Get a copy of the input string, so we can use it freely.

    local_source_ = strdup (to_parse);

    // Find out how many characters has the source string.

    local_source_length_ = (int)strlen (to_parse);

    // Initialize the position pointer.

    current_char_ = 0;
  }
}

astr_tokenizer::
~astr_tokenizer
(void)
{
  {
    free (local_source_);
  }
}
