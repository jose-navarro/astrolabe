/** \file gp_navfile_manager.cpp
  \brief Implementation file for gp_navfile_manager.hpp.
  \ingroup ASTROLABE_data_IO
*/

#include "gp_navfile_manager.hpp"

// Initialize constants.

const int gp_navfile_manager::MODE_FORWARD        = 0;
const int gp_navfile_manager::MODE_BACKWARDS      = 1;
const int gp_navfile_manager::MODE_COMBINED_BLOCK = 2;
const int gp_navfile_manager::MODE_COMBINED_FWS   = 3;

const vector<string> gp_navfile_manager::NAVDIR_FILENAMES({"nav_directory_fw.ndf",
                                                           "nav_directory_bw.ndf",
                                                           "nav_directory_cb.ndf"});

bool
gp_navfile_manager::
check_navdir_names
(int             mode,
 vector<string>& the_navdir_filenames)
{
  {
    size_t found_at;
    int    i;
    size_t l_looking_for;
    size_t l_search_in;
    string looking_for;
    string search_in;

    if (mode == MODE_COMBINED_FWS)
    {
      // We must check THREE navigation files.

      for (i = 0; i <= 2; i++)
      {
        search_in   = the_navdir_filenames[i];
        looking_for = NAVDIR_FILENAMES[i];

        found_at = search_in.rfind(looking_for);

        if (found_at == string::npos) return false;

        if (found_at != 0)
        {
          // There is a path in front of the name of the directory file.

          l_looking_for = looking_for.size();
          l_search_in   = search_in.size();

          if ((l_search_in - l_looking_for) != found_at) return false;
        }
      }
    }
    else
    {
      // In this case we just check ONE navigation file.

      search_in = the_navdir_filenames[0];
      looking_for = NAVDIR_FILENAMES[mode];

      found_at = search_in.rfind(looking_for);

      if (found_at == string::npos) return false;

      if (found_at != 0)
      {
        // There is a path in front of the name of the directory file.

        l_looking_for = looking_for.size();
        l_search_in = search_in.size();

        if ((l_search_in - l_looking_for) != found_at) return false;
      }
    }
    return true;
  }
}

int
gp_navfile_manager::
create
(int             mode,
 vector<string>& the_navdir_filenames,
 vector<string>& the_rest_of_filenames,
 const string&   navfile_name)
{
  {
    int                   end;
    vector<string>        files_to_compress;
    int                   i;
    size_t                j;
    int                   start;
    int                   retcode;
    size_t                total_navdir_files;

    total_navdir_files = the_navdir_filenames.size();

    // Check our preconditions.

    if ((mode < MODE_FORWARD)      || (mode > MODE_COMBINED_FWS)) return 1;
    if ((mode == MODE_COMBINED_FWS) & (total_navdir_files != 3))  return 2;
    if ((mode != MODE_COMBINED_FWS) & (total_navdir_files != 1))  return 2;

    if (!check_navdir_names(mode, the_navdir_filenames))          return 2;

    if (the_rest_of_filenames.size() == 0)                        return 3;

    if (navfile_name.empty())                                     return 4;

    //
    // Well, we can prepare the list of files to add to the output
    // (compressed) navigation file.
    //

    files_to_compress.clear();

    if (mode == MODE_COMBINED_FWS)
    {
      start = 0;
      end   = 2;
    }
    else
    {
      start = end = 0;
    }

    for (i = start; i <= end; i++) files_to_compress.push_back(the_navdir_filenames[i]);
    for (j = 0; j < the_rest_of_filenames.size(); j++) files_to_compress.push_back(the_rest_of_filenames[j]);

    //
    // At this point, no matter what the mode selected was, all the files referenced
    // by the (maybe several) navigation directory(ies) object(s) have been included
    // in the list of files to compress. 
    //
    // We can now compress all these files and create the navigation file.
    //

    retcode = cmanager_.zip(navfile_name, files_to_compress);
    if (retcode != 0) return 5; // Error compressing.

    // That's all.

    return 0;
  }
}

int
gp_navfile_manager::
create_a_folder
(const string & new_folder_path)
const
{
  {
    int retcode;

    // First, check whether the folder exists.

    retcode = is_a_folder(new_folder_path);

    // If the folder already existed, we'll simply do nothing.
    if (retcode == 0) return 0;

    // If the path pointing to the new folder exist, but it
    // is in fact a regular filename... well, we'll return an error!

    if (retcode == 1) return 1;

    //
    // The only remaining possibility is that the path pointed
    // so something that didn't exist yet (retcode is 2). So
    // we'll create the folder.
    //

    if (MKDIR(new_folder_path.c_str()) != 0) return 2;

    // That's all. Folder created.

    return 0;
  }
}

int
gp_navfile_manager::
extract
(const string& navfile_name,
 const string& where)
{
  int retcode;
  {
    //
    // First of all, we'll see if the destination folder
    // exist, if it has to be created or, in the contrary,
    // if the path received points to a regular file that
    // should not be destroyed...
    //

    retcode = create_a_folder(where);

    if (retcode != 0) return retcode;

    //
    // We've got a folder to extract our navigation file.
    // Let's do it.
    //

    retcode = cmanager_.unzip(navfile_name, where);

    if (retcode != 0) return 3; // Unable to extract.

    return 0;
  }
}

gp_navfile_manager::
gp_navfile_manager
(void)
{
  {
    // Intentionally left blank.
  }
}


gp_navfile_manager::
~gp_navfile_manager
(void)
{
  {
    // Intentionally left blank.
  }
}

int
gp_navfile_manager::
is_a_folder
(const string & path_to_check)
const
{
  {
    struct stat info;

    if (stat(path_to_check.c_str(), &info) != 0)
      return 2; // Cannot access the path given. Does it exist???
    else if (info.st_mode & S_IFDIR)
      return 0; // The given path is a directory / folder.
    else
      return 1; // The given path is NOT a directory.
  }
}
