/** \file gp_compression_manager.hpp
\brief Implementation file for gp_compression_manager.hpp
\ingroup ASTROLABE_data_IO
*/

#include "gp_compression_manager.hpp"
#include "gp_compression_manager.hpp"

gp_compression_manager::
gp_compression_manager
(void)
{
  {
    // Intentionally left blank.
  }
}


gp_compression_manager::
~gp_compression_manager
(void)
{
  {
    // Intentionally left blank.
  }
}

int
gp_compression_manager::
list
(const string& ziparchive,
 vector<string>& the_files)
{
  {
    ZipArchive::Ptr archive;
    size_t          entries;

    try
    {
      archive = ZipFile::Open(ziparchive.c_str());
    }
    catch (std::exception&)
    {
      return 1; // Error opening zip archive.
    }

    try
    {
      entries = archive->GetEntriesCount();
    }
    catch (std::exception&)
    {
      return 2; // Error retrieving the number of entries
                // zip archive.
    }

    the_files.clear();
    try
    {
      for (size_t i = 0; i < entries; ++i)
      {
        auto entry = archive->GetEntry(int(i));

        the_files.push_back(entry->GetFullName());
      }
    }
    catch (std::exception&)
    {
      return 3; // Error retrieving the names of the files
                // included in the zip archive.
    }

    return 0;
  }
}

int
gp_compression_manager::
unzip
(const string& ziparchive, 
 const string& destination_folder)
{
  ZipArchive::Ptr archive;
  string          dfolder;
  size_t          entries;
  string          full_single_filename;
  size_t          size;
  const char*     single_filename;

  // First of all, check if the destination folder is empty.

  if (destination_folder.empty())
  {
    // The destination assumed is the current directory, whatever it is.

    dfolder = "./";
  }
  else
  {
    //
    // We have a destination directory. Copy the input parameter so
    // we can manipulate it at our will.
    //

    dfolder = destination_folder;

    //
    // Look for the trailing slash, whatever it is (/ or \).
    // Add one if not found.
    //

    size = dfolder.size();
    if ((dfolder[size] != '\\') & (dfolder[size] != '/'))
      dfolder = dfolder + "/";
  }

  try
  {
    archive = ZipFile::Open(ziparchive.c_str());
  }
  catch (std::exception&)
  {
    return 1; // Error opening zip archive.
  }

  try
  {
    entries = archive->GetEntriesCount();
  }
  catch (std::exception&)
  {
    return 2; // Error retrieving the number of entries
              // zip archive.
  }

  try
  {

    for (size_t i = 0; i < entries; ++i)
    {
      auto entry = archive->GetEntry(int(i));

      single_filename = entry->GetFullName().c_str();
      full_single_filename = dfolder + single_filename;
      ZipFile::ExtractFile(ziparchive.c_str(), single_filename, full_single_filename.c_str());
    }
  }
  catch (std::exception&)
  {
    return 3; // Error extracting file from zip archive.
  }

  return 0;
}


int
gp_compression_manager::
zip
(const string& ziparchive,
 vector<string>& the_files)
{
  {
    size_t i;

    try
    {
      for (i = 0; i < the_files.size(); i++)
      {
        ZipFile::AddFile(ziparchive.c_str(), the_files[i].c_str());
      }
    }
    catch (std::exception&)
    {
      return 1; // Error adding file to zip archive.
    }

    return 0;
  }
}
