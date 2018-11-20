/** \file gp_navfile_manager.hpp
  \brief Manage (create, extract) ASTROLABE navigation files.
  \ingroup ASTROLABE_data_IO
*/

#ifndef _GP_NAVFILE_MANAGER_HPP_
#define _GP_NAVFILE_MANAGER_HPP_

//
// Includes needed to check if some given path is a folder.
// Additionally, in Linux, they also provides with
// the mkdir function to create directories (stat).
//

#include <sys/types.h>
#include <sys/stat.h>

//
// Trick to use the same function name to create directories.
// Note that when using GCC, the former <sys/stat.h> inclusion
// is enough to make the mkdir available. In the case of
// Microsoft compilers, using a similar function (_mkdir)
// implies including an extra header, that is, <direct.h>
//

#ifdef __GNUC__
  /// \brief Create a directory.
  /**
    \param path The full path of the directory to create.
  */
  #define MKDIR(path) mkdir(path, ACCESSPERMS)
#else
  #include <direct.h>
  /// \brief Create a directory.
  /**
    \param path The full path of the directory to create.
  */
  #define MKDIR(path) _mkdir(path)
#endif

// To work with strings and vectors.
#include <string>
#include <vector>

// To compress and decompress files.

#include "gp_compression_manager.hpp"

using namespace std;

/// \brief Manage (create, extract) ASTROLABE navigation files.
/**
  The ASTROLABE navigation file is a compressed (zip format) file
  containing all the metadata and data involved in the generation
  of a trajectory.

  This class offers the necessary services to create and extract
  (that is, compress and decompress) such kind of files.
*/

class gp_navfile_manager
{

  public:

    /// \brief Create a navigation file for trajectories computed in
    ///        forward mode only.
    static const int MODE_FORWARD;

    /// \brief Create a navigation file for trajectories computed in
    ///        backwards mode only.
    static const int MODE_BACKWARDS;

    /// \brief Create a navigation file for trajectories computed by
    ///        means of block adjustment.
    static const int MODE_COMBINED_BLOCK;

    /// \brief Create a navigation file for trajectories computed by
    ///        means of a forward step, a backwards one, and, finally,
    ///        a smoothing step.
    static const int MODE_COMBINED_FWS;

  protected:

    /// \brief The names of the navigation directory files.

    static const vector<string> NAVDIR_FILENAMES;

  public:

    /// \brief Build (compress) a navigation file including all
    ///        the intervening files it must enclose.
    /**
      \param mode Processing mode. Legal values are one of
             these: gp_compression_manager::MODE_FORWARD,
             gp_compression_manager::MODE_BACKWARDS,
             gp_compression_manager::MODE_COMBINED_BLOCK or
             gp_compression_manager::MODE_COMBINED_FWS. These
             values denote how the trajectory has been generated
             and thus how many navigation directory files (see
             parameter the_navdir_filenames below) may be
             accepted to generate the navigation file.
      \param the_navdir_filenames Array containing EXACTLY
             one or three navigation directory file names
             (defining the contents of the output navigation
             file). The number of navigation directory files
             depends on the selected processing mode (see
             next parameter). Furthermore, the name or names
             of this (these) file name(s) must adhere to a
             strict naming convention. See the description
             of this method for details.
      \param the_rest_of_filenames The names of all the files
             (with the exception of the names of the navigation
             directory files, see parameter the_navdir_filenames
             above) that must be included in the (compressed)
             output navigation file.
      \param navfile_name File name of the (compressed)
             navigation file.
      \return An error code. 0 stands for successful completion. 1
             means that the mode selected is not valid (see
             description of parameter mode). 2 implies that
             the number of navigation directory objects and
             the mode selected do not match (see the extended
             description of this method) or that the names
             of these navigation files are not correct. 3 is
             the returned error code when the list of files
             to include (the_rest_of_filenames) is empty. 4 means
             that the name of the output navigation file is
             empty and, finally, 5 means that
             an error has been detected when actually
             compressing the output navigation file.

      A navigation file may include a single trajectory or a
      set of these depending on the processing mode used.

      Some trajectories are computed using the so-called forward
      mode only (which corresponds to gp_compression_manager::MODE_FORWARD);
      others may have been generated in backwards mode only
      (gp_compression_manager::MODE_BACKWARDS) while it is most usual
      to use a combined mode (gp_compression_manager::MODE_COMBINED_FBS)
      that generates a first one using the forward mode, another
      one using in this case the backwards mode and, finally, obtaining
      a third and final one by combining these two.

      The first two cases (gp_compression_manager::MODE_FORWARD and
      gp_compression_manager::MODE_BACKWARDS) will need a single
      navigation directory file to generate the output navigation
      file. The forward + backwards + smoothing mode will always
      need three navigation directory files to define how the
      trajectory has been generated.

      There is a fourth mode (gp_compression_manager::MODE_COMBINED_BLOCK)
      needing only one navigation directory file. In this case,
      the trajectory may be seen as a combined one, although it has
      been generated in a single step instead of three.

      The mode used has an impact not only in the number of navigation
      directory files needed to generate the navigation file, but also
      in the names of the corresponding navigation directory files to use.

      The list below states how parameters must be passed to this method depending
      on the mode selected. Note that the mode used corresponds to parameter
      mode, that the name (or names) of the navigation directory file(s) must
      be set by means of parameter the_navdir_filenames (which will have
      either one or three elements), that the names of the remaining files
      (that is, those that are NOT navigation directory files) must be set
      using parameter the_rest_of_filenames and, finally, parameter navfile_name
      will contain the name of the output (compressed) navigation file.

      - gp_compression_manager::MODE_FORWARD: 1 navigation directory file, with
        name: nav_directory_fw.ndf.
      - gp_compression_manager::MODE_BACKWARDS: 1 navigation directory file, with
        name: nav_directory_bw.ndf
      - gp_compression_manager::MODE_COMBINED_BLOCK: 1 navigation directory file,
        with name: nav_directory_cb.ndf.
      - gp_compression_manager::MODE_COMBINED_FBS: 3 navigation directory files,
        with names: nav_directory_fw.ndf, nav_directory_bw.ndf and nav_directory_cb.ndf
        (in this precise order).

      Note that the names of the navigation files shown in the former list (as
      for instance, nav_directory_fw.ndf) must be exactly the ones specified
      (even in case letter). These may be preceded, however, by a path pointing
      to the precise location where they are stored.

      There are no restrictions concerning the names of the rest of files, but for
      the fact that many of these must be correctly referenced in the corresponding
      navigation directory files.

      This method checks that the mode used and the name(s) of the navigation directory
      files match. That is, if mode gp_compression_manager::MODE_FORWARD is used,
      then the method will accept a single navigation directory file and its
      name (besides any leading path) must be exactly nav_directory_fw.ndf.
      
      No other tests related to the correctness of the contents of the output navigation
      file are made (as for instance, checking that all the files referenced by
      the navigation directory file(s) are present in the list of the rest of the
      files). Therefore, it is the responsibility of the calling module to guarantee
      the coherence of the contents of such output file.
      
    */

    int  create              (int             mode,
                              vector<string>& the_navdir_filenames,
                              vector<string>& the_rest_of_filenames,
                              const string&   navfile_name);

    /// \brief Extract (decompress) a navigation file.
    /**
      \param navfile_name Name of the navigation file to decompress.
      \param where Full path to a folder where the contents of the
             navigation file must be extracted. This parameter may be
             omitted; in such case, the current working directory
             is used. Note that forward slashes ("/") must be used.
             The last slash in the path is optional.
      \return An error code. 0 stands for success (either the directory already
             existed or it has been created sucessfully). 1 means that the path
             points to a regular file that should not be destroyed creating
             a directory using the same name and, finally, 2 means that
             the folder could not be created (much probably, because of invalid
             characters in the path). 3 means that it has not been possible
             to extract the navigation file, much probably because its name
             is not correct (although other reasons are possible, as not
             enough disk space available).
    */

    int  extract             (const string& navfile_name, const string& where = "./");

    /// \brief Default constructor.

         gp_navfile_manager  (void);

    /// \brief Destructor.

         ~gp_navfile_manager (void);

  protected:

    /// \brief Check that the name(s) of the navigation directory
    /// file(s) match with the selected mode.

    bool check_navdir_names (int mode, vector<string>& the_navdir_filenames);

    /// \brief Create a new folder.
    /**
      \param new_folder_path Full path and folder name of the folder to create.
      \return An error code. 0 stands for success (either the directory already
             existed or it has been created sucessfully). 1 means that the path
             points to a regular file that should not be destroyed creating
             a directory using the same name and, finally, 2 means that
             the folder could not be created (much probably, because of invalid
             characters in the path).
    */

    int  create_a_folder    (const string& new_folder_path) const;

    /// \brief Check if the path provided points to a directory, a file or
    ///        to something that does not exist at all.
    /**
      \param path_to_check The full path pointing to something that might
             be a folder, file or nothing at all.
      \return A code stating what has been found: 0 stands for "folder",
             1 means "file" and 2 stands for "no file or folder with the
             given name exists".
    */

    int  is_a_folder        (const string& path_to_check) const;

  protected:

    /// \brief The tool that will actually compress or decompress data.

    gp_compression_manager cmanager_;

};

#endif // _GP_NAVFILE_MANAGER_HPP_
