/** \file gp_navdir_data.hpp
\brief Entity class to hold the information retrieved from or to
       store in an ASTROLABE navigation directory file.
\ingroup ASTROLABE_metadata
*/

#ifndef _GP_NAVDIR_DATA_HPP_
#define _GP_NAVDIR_DATA_HPP_

#include "astrolabe_exceptions.hpp"
#include "lineage_data.hpp"

#include <string>
#include <vector>
using namespace std;


/// \brief Entity class to hold the information retrieved from or to
///        store in an ASTROLABE navigation directory file.

class gp_navdir_data
{
public:

  /// \brief Add one more file name to the list of metadata navigation
  ///        file names.
  /**
    \param filename The name of the file to set.
  */
  void          add_metadata_navigation_file_name              (string& filename);

  /// \brief Return the name of the (optional) input instruments file
  /// name or the empty string if it was never set.
  string&       get_input_instruments_file_name                (void) const;

  /// \brief Return the number of input navigation metadata files.
  int           get_dim_input_metadata_navigation_files        (void);

  /// \brief Return the name of the navigation metadata file at position
  /// position.
  /**
    \pre ((position >= 0) & (position < get_dim_input_metadata_navigation_files()))
    \param position The index of the navigation metadata file to retrieve.
           Its value must be within the valid range (see preconditions)
    \return The name of the navigation metadata file stored in the position
           pointed to by parameter "position".
    \throw astrolabe_precondition_violation One or more preconditions
           have been violated.
     
  */
  string&       get_input_navigation_metadata_file             (int position);

  /// \brief Return the name of the input observations file
  /// name or the empty string if it was never set.
  string&       get_input_observations_file_name               (void) const;

  /// \brief Return the name of the (optional) input observations
  /// correlations file name or the empty string if it was never set.
  string&       get_input_observations_correlations_file_name  (void) const;

  /// \brief Return the format of the (optional) input options file
  /// or the empty string if it was never set.
  string&       get_input_options_file_format                  (void) const;

  /// \brief Return the name of the (optional) input options file
  /// name or the empty string if it was never set.
  string&       get_input_options_file_name                    (void) const;

  /// \brief Return the version of the (optional) input options file
  /// or the empty string if it was never set.
  string&       get_input_options_file_version                 (void) const;

  /// \brief Retrieve a pointer to the lineage_data object included
  /// in the gp_navdir_data object.
  /**
    This method provides with an easy way to access the methods of
    the lineage_data object, so the API of this class is not unnecessarily
    complicated.

    Note that the object referenced by the pointer thus retrieved MUST
    NEVER BE DELETED by the calling module. Doing so would corrupt the
    gp_navdir_data object and this could crash the application.
  */

  lineage_data* get_lineage_ptr                                (void);

  /// \brief Return the name of the (optional) output log file
  /// name or the empty string if it was never set.
  string&       get_output_log_file_name                       (void) const;

  /// \brief Return the name of the output residuals file
  /// name or the empty string if it was never set.
  string&       get_output_residuals_file_name                 (void) const;

  /// \brief Return the name of the (optional) output residuals
  /// correlations file name or the empty string if it was never set.
  string&       get_output_residuals_correlations_file_name    (void) const;

  /// \brief Return the name of the output states file
  /// name or the empty string if it was never set.
  string&       get_output_states_file_name                    (void) const;

  /// \brief Return the name of the (optional) output states
  /// correlations file name or the empty string if it was never set.
  string&       get_output_states_correlations_file_name       (void) const;


  /// \brief Default constructor.
                gp_navdir_data                                 (void);

   /// \brief Destructor.
               ~gp_navdir_data                                 (void);

   /// \brief Set the name of the (optional) input instruments file
   /// name.
   /**
     \param filename The name of the file to set.
   */
   void         set_input_instruments_file_name                (string& filename);

   /// \brief Set the name of the input observations file name.
   /**
   \param filename The name of the file to set.
   */
   void         set_input_observations_file_name               (string& filename);

   /// \brief Set the name of the (optional) input observation
   /// correlations file name.
   /**
   \param filename The name of the file to set.
   */
   void         set_input_observations_correlations_file_name  (string& filename);

   /// \brief Set the format of the (optional) input options file
   /// name.
   /**
   \param format The format of the file to set.
   */
   void         set_input_options_file_format                  (string& format);

   /// \brief Set the name of the (optional) input options file
   /// name.
   /**
     \param filename The name of the file to set.
   */
   void         set_input_options_file_name                    (string& filename);

   /// \brief Set the version of the (optional) input options file
   /// name.
   /**
     \param version The version of the file to set.
   */
   void         set_input_options_file_version                 (string& version);

   /// \brief Set the name of the (optional) output log file name.
   /**
   \param filename The name of the file to set.
   */
   void         set_output_log_file_name                       (string& filename);

   /// \brief Set the name of the output residuals file name.
   /**
   \param filename The name of the file to set.
   */
   void         set_output_residuals_file_name                 (string& filename);

   /// \brief Set the name of the (optional) output residuals
   /// correlations file name.
   /**
   \param filename The name of the file to set.
   */
   void         set_output_residuals_correlations_file_name    (string& filename);

   /// \brief Set the name of the output states file name.
   /**
   \param filename The name of the file to set.
   */
   void         set_output_states_file_name                    (string& filename);

   /// \brief Set the name of the (optional) output states
   /// correlations file name.
   /**
   \param filename The name of the file to set.
   */
   void         set_output_states_correlations_file_name       (string& filename);


protected:

  /// \brief Name of the (optional) input instruments file.
  string         instruments_file_name_;

  /// \brief The lineage element.
  lineage_data   lineage_;

  /// \brief Name of the (optional) output log file.
  string         log_file_name_;

  /// \brief The names of the input metadata files.
  vector<string> metadata_file_names_;

  /// \brief Name of the input observations file name.
  string         observations_file_name_;

  /// \brief Name of the (optional) input observation correlation matrices file.
  string         observations_correlations_file_name_;

  /// \brief Format of the (optional) input options file. Makes sense
  /// only when the options file name is nonempty.
  string         options_file_format_;

  /// \brief Name of the (optional) input options file.
  string         options_file_name_;

  /// \brief Version of the (optional) input options file. Makes sense
  /// only when the options file name is nonempty.
  string         options_file_version_;

  /// \brief Name of the output residuals file.
  string         residuals_file_name_;

  /// \brief Name of the (optional) residuals correlations file name.
  string         residuals_correlations_file_name_;

  /// \brief Name of the output states file.
  string         states_file_name_;

  /// \brief Name of the (optional) output states correlation matrices file.
  string         states_correlations_file_name_;

};

#endif // _GP_NAVDIR_DATA_HPP_
