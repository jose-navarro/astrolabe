/** \file observation_file_writer_txt_embedded.cpp 
  \brief  Implementation file for observation_file_writer_txt_embedded.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 
#include "observation_file_writer_txt_embedded.hpp"

int
observation_file_writer_txt_embedded::
attach
(FILE* file_to_attach)
{
  {
    // We do not accept NULL file pointers.

    if (file_to_attach == NULL) return 1;

    // If we're already attached, we'll complain too.

    if (file_is_attached_) return 2;

    // OK. Attach to the file given as input parameter.

    file_to_write_ = file_to_attach;

    // Update flags.

    file_is_attached_ = true;

    // That's all.

    return 0;
  }
}

void
observation_file_writer_txt_embedded::
detach
(void)
{
  {
    // Do not complain if the file is already detached.
    if (!file_is_attached_) return;

    // Forget the file.

    file_to_write_ = NULL;
    file_is_attached_ = false;

    // Successful completion.

    return;
  }
}

observation_file_writer_txt_embedded::
observation_file_writer_txt_embedded
(void)
{
  {

    //
    // Instantiate and assign the fixed parts of the l- and o-records
    // formats.
    //
    // Note that this is the place to change default formats for
    // integer (o-records) or double (l-records) if ever needed as well
    // as any other format involved in the process or writing.
    //

    format_fixed_l_               = strdup("    <l s=\"%c\" id=\"%s\" n=\"%ld\"> %23.15le");
    format_fixed_o_               = strdup("    <o s=\"%c\" id=\"%s\"> %23.15le");
    format_default_double_format_ = strdup(" %23.15le");
    format_default_int_format_    = strdup(" %10ld");

    // Initialize other members.

    file_is_attached_   = false;
    file_to_write_      = NULL;

    first_time_writing_ = true;
    first_l_written_    = false;

    last_time_tag_      = 0.0;

  }
}

observation_file_writer_txt_embedded::
~observation_file_writer_txt_embedded
(void)
{
  {
    // Make sure that the file has been detached.
    if (file_is_attached_) detach();

    // Destroy local objects.

    free(format_fixed_l_);
    format_fixed_l_ = NULL;

    free(format_fixed_o_);
    format_fixed_o_ = NULL;

    free(format_default_double_format_);
    format_default_double_format_ = NULL;

    free(format_default_int_format_);
    format_default_int_format_ = NULL;
  }
}

int
observation_file_writer_txt_embedded::
write_l
(bool         active,
 char*        identifier,
 int          instance_identifier,
 double       time,
 int          n_tags,
 double*      buffer_tags,
 int          n_expectations,
 double*      buffer_expectations,
 int          n_covariance_values,
 double*      buffer_covariance)

{
  {
    char  cactive;
    int   i;
    int   sizeof_identifier;

    // Won't work if the file is not attached.

    if (!file_is_attached_)    return 1;

    // identifier must be at least 1 char long excluding the terminating NULL char.

    if (identifier == NULL)    return 4;

    sizeof_identifier = (int)strlen(identifier);
    if (sizeof_identifier < 1) return 4;

    // The dimensions of the several double arrays must be correct.

    if (n_tags < 0)            return 5;
    if (n_expectations <= 0)   return 5;
    if ((n_covariance_values != 0)              &
        (n_covariance_values != n_expectations) &
        (n_covariance_values != (n_expectations*(n_expectations+1))/2))
                               return 5;

    //
    // Check if this is the first time we write to the file. If so, we
    // must give an initial value to the last time tag in use.
    //
    // Note that inactive records do not change flags at all. We
    // simply write to the output file, but it is as if we wouldn't
    // written nothing at all from the flags standpoint.
    //

    if (active && first_time_writing_)
    {
      first_time_writing_ = false;
      last_time_tag_      = time;
    }

    //
    // Check if an epoch change is just arriving... Inactive records do
    // not change flags.
    //

    if (active && (time != last_time_tag_))
    {
      // We've got a new last_time_tag_ to compare to.

      last_time_tag_ = time;

      //
      // Update the first_l_written_ tag. This is not strictly necessary,
      // since we are writing an l-record here and this means that if we've
      // been the first write_x method called after an epoch change, it is
      // OK (an l-record must be the first one in an epoch). However,
      // the next assignment is made for clarity reasons. Later on, at
      // the end of the method, this flag is set again to its proper value, true,
      // since, obviously, an l-record will have been written as the first
      // one in an epoch.
      //

      first_l_written_ = false;
    }

    //
    // Write the l-record to the output file.
    //
    // This is done in several steps:
    //
    // - The l tag (including the active / removed attribute), identifier,
    //   instance identifier and time tag first,
    // - the tags (if any), expectations and covariance values (again, if any),
    // - the closing l tag plus the newline to finish.
    //

    if (active) cactive = 'a';
    else        cactive = 'r';

    fprintf(file_to_write_, format_fixed_l_, cactive, identifier, instance_identifier, time);

    // Tags (if any)

    for (i = 0; i < n_tags; i++ )
    {
      fprintf(file_to_write_, format_default_double_format_, buffer_tags[i]);
    }

    // Expectations

    for (i = 0; i < n_expectations; i++ )
    {
      fprintf(file_to_write_, format_default_double_format_, buffer_expectations[i]);
    }

    // Covariance values (if any)

    for (i = 0; i < n_covariance_values; i++ )
    {
      fprintf(file_to_write_, format_default_double_format_, buffer_covariance[i]);
    }

    // Closing l tag plus newline.


    fprintf(file_to_write_, " </l>\n");

    //
    // If we've just written an ACTIVE l-record, we don't need extra
    // l-records the next time data is written, so o-records are legal now.
    // Note that inactive records will never change flags.
    //

    if (active) first_l_written_ = true;

    // That's all.

    return 0;
  }
}

int
observation_file_writer_txt_embedded::
write_o
(bool         active,
 char*        identifier,
 double       time,
 int          n_parameter_iids,
 int*         the_parameter_iids,
 int          n_observation_iids,
 int*         the_observation_iids,
 int          n_instrument_iids,
 int*         the_instrument_iids)
{
  {
    char  cactive;
    int   i;
    int   sizeof_identifier;

    // Won't work if the file is not attached.

    if (!file_is_attached_) return 1;

    //
    // Won't work neither if no l-records have been written yet for the
    // current time tag. This test, however, makes sense only when
    // writing ACTIVE o-records. Inactive ones are always written,
    // no matter what the situation is.
    //

    if (active && (!first_l_written_)) return 3;

    // identifier must contain something.

    if (identifier == NULL) return 4;

    sizeof_identifier = (int)strlen(identifier);
    if (sizeof_identifier < 1) return 4;

    //
    // We'll still refuse to do anything if the number of identifier does not
    // match with what we're expecting.
    //

    if (n_parameter_iids   <= 0) return 7;
    if (n_observation_iids <= 0) return 7;
    if (n_instrument_iids  <  0) return 7;

    //
    // Check if an epoch change is just arriving... Here, the problem
    // of the no l-records written could arise too.
    //
    // Note that this only matters when the record is active. Otherwise,
    // we simply write data but change NO flags at all.
    //

    if (active && (time != last_time_tag_))
    {
      //
      // Ooops! This is the first record in an epoch and we are trying to
      // write an o-record. This is an error!
      //

      return 3;
    }

    //
    // Write to the output file the required info.
    //
    // This is done in several steps:
    //
    // - The o tag (including the active / removed attribute), identifier
    //   and time tag first,
    // - the different lists of instance identifiers, using several loops then and
    // - the closing o tag plus the newline to finish.
    //

    // o tag, code and time tag.

    if (active) cactive = 'a';
    else        cactive = 'r';

    fprintf(file_to_write_, format_fixed_o_, cactive, identifier, time);

    // List of parameter instance identifiers.

    for (i = 0; i < n_parameter_iids; i++ )
    {
      fprintf(file_to_write_, format_default_int_format_, the_parameter_iids[i]);
    }

    // List of observation instance identifiers.

    for (i = 0; i < n_observation_iids; i++ )
    {
      fprintf(file_to_write_, format_default_int_format_, the_observation_iids[i]);
    }

    // List of parameter instrument identifiers, if any.

    if (n_instrument_iids > 0)
    {
      for (i = 0; i < n_instrument_iids; i++ )
      {
        fprintf(file_to_write_, format_default_int_format_, the_instrument_iids[i]);
      }
    }

    // Closing o tag plus newline.

    fprintf(file_to_write_, " </o>\n");

    // That's all.

    return 0;
  }
}
