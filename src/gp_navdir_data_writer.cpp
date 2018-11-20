/** \file gp_navdir_data_writer.cpp
\brief Implementation of file gp_navdir_data_writer.hpp
\ingroup ASTROLABE_metadata
*/

#include "gp_navdir_data_writer.hpp"

gp_navdir_data_writer::
gp_navdir_data_writer
(void)
{
  {
    // Intentionally left blank.
  }
}


gp_navdir_data_writer::
~gp_navdir_data_writer
(void)
{
  {
    // Intentionally left blank.
  }
}

int
gp_navdir_data_writer::
write
(gp_navdir_data& navdir_data,
 const string&   navigation_dir_file_name)
{
  {
    FILE*              navdir_file;
    int                i;
    lineage_data*      lineage_info;
    int                n_metadata_files;
    time_t             now;
    string             stmp, stmp2, stmp3;
    char               time_buffer[100];
    int                total_items;

    //
    // Check our preconditions. The following name of the output
    // file may not be empty.
    //

    if (navigation_dir_file_name.empty()) return 1;

    // Copy parameters to members.

    navigation_directory_file_name_ = navigation_dir_file_name;

    // Let's open a new text file.

    navdir_file = fopen(navigation_directory_file_name_.c_str(), "w");
    if (navdir_file == NULL) return 2;

    // Let's write the header!!!

    fprintf(navdir_file, "<nav-directory_file xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
    fprintf(navdir_file, "  xsi:noNamespaceSchemaLocation=\"nav-directory_file.xsd\">\n");
    fprintf(navdir_file, "  <lineage version=\"1.0\">\n");

    // Dump the lineage object completely (at least, those fields that have been set).

    lineage_info = navdir_data.get_lineage_ptr();

    // Dump the id (the only mandatory field).

    stmp = lineage_info->id_get();
    fprintf(navdir_file, "    <id> %s </id>\n", stmp.c_str());

    // Name.

    stmp = lineage_info->name_get();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "    <name> %s </name>\n", stmp.c_str());
    }

    // Authors

    total_items = lineage_info->author_item_dim();
    if (total_items > 0)
    {
      fprintf(navdir_file, "    <author>\n");
      for (i = 0; i < total_items; i++)
      {
        stmp = lineage_info->author_item_get(i);
        fprintf(navdir_file, "      <item> %s </item>\n", stmp.c_str());
      }
      fprintf(navdir_file, "    </author>\n");
    }

    // Organization

    stmp = lineage_info->organization_get();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "    <organization> %s </organization>\n", stmp.c_str());
    }

    // Department

    stmp = lineage_info->department_get();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "    <department> %s </department>\n", stmp.c_str());
    }

    // Date and time.

    //
    // We ignore the date and time that could have been stored in the lineage
    // object, since we'll write the time associated to the moment when
    // this header is written.
    //

    now = time(0);
    strftime(time_buffer, 100, "%Y-%m-%dT%H:%M:%S.000", localtime(&now));
    fprintf(navdir_file, "    <date_time>%s</date_time>\n", time_buffer);

    // Reference documents.

    total_items = lineage_info->ref_document_item_dim();
    if (total_items > 0)
    {
      fprintf(navdir_file, "    <ref_document>\n");
      for (i = 0; i < total_items; i++)
      {
        stmp = lineage_info->ref_document_item_get(i);
        fprintf(navdir_file, "      <item> %s </item>\n", stmp.c_str());
      }
      fprintf(navdir_file, "    </ref_document>\n");
    }

    // Project

    stmp = lineage_info->project_get();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "    <project> %s </project>\n", stmp.c_str());
    }

    // Task

    stmp = lineage_info->task_get();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "    <task> %s </task>\n", stmp.c_str());
    }

    // Remarks

    stmp = lineage_info->remarks_get();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "    <remarks> %s </remarks>\n", stmp.c_str());
    }


    // Close the lineage tag.

    fprintf(navdir_file, "  </lineage>\n");

    // Input files ------

    fprintf(navdir_file, "  <input>\n");

    // Optional options file.

    stmp = navdir_data.get_input_options_file_name();
    if (!stmp.empty())
    {
      stmp2 = navdir_data.get_input_options_file_format();
      stmp3 = navdir_data.get_input_options_file_version();

      fprintf(navdir_file, "    <options>\n");
      fprintf(navdir_file, "      <op_file format=\"%s\" version=\"%s\"> %s </op_file>\n",
                           stmp2.c_str(), stmp3.c_str(), stmp.c_str());
      fprintf(navdir_file, "    </options>\n");
    }

    // Mandatory metadata. There may be several of these.

    n_metadata_files = navdir_data.get_dim_input_metadata_navigation_files();

    fprintf(navdir_file, "    <metadata>\n");

    for (i = 0; i < n_metadata_files; i++)
    {
      stmp = navdir_data.get_input_navigation_metadata_file(i);
      fprintf(navdir_file, "      <nav_metadata> %s </nav_metadata>\n", stmp.c_str());
    }

    fprintf(navdir_file, "    </metadata>\n");

    // Observations. Note that the correlations are optional.

    fprintf(navdir_file, "    <observations>\n");

    stmp = navdir_data.get_input_observations_file_name();
    fprintf(navdir_file, "      <obs_e> %s </obs_e>\n", stmp.c_str());

    stmp = navdir_data.get_input_observations_correlations_file_name();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "      <tr_obs_correlation_Rll> %s </tr_obs_correlation_Rll>\n", stmp.c_str());
    }

    fprintf(navdir_file, "    </observations>\n");

    // Instruments. Watch out! These are optional.

    stmp = navdir_data.get_input_instruments_file_name();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "    <instruments>\n");
      fprintf(navdir_file, "      <obs_e> %s </obs_e>\n", stmp.c_str());
      fprintf(navdir_file, "    </instruments>\n");
    }

    // No more input files.

    fprintf(navdir_file, "  </input>\n");

    // Let's write the output ones.

    fprintf(navdir_file, "  <output>\n");

    // The optional log file.

    stmp = navdir_data.get_output_log_file_name();
    if (!stmp.empty()) fprintf(navdir_file, "    <log> %s </log>\n", stmp.c_str());

    // States (mandatory, but with optional correlations).

    fprintf(navdir_file, "    <states>\n");

    stmp = navdir_data.get_output_states_file_name();
    fprintf(navdir_file, "      <obs_e> %s </obs_e>\n", stmp.c_str());

    stmp = navdir_data.get_output_states_correlations_file_name();
    if (!stmp.empty())
    {
      fprintf(navdir_file, "      <tr_states_correlation_Rxx> %s </tr_states_correlation_Rxx>\n", stmp.c_str());
    }

    fprintf(navdir_file, "    </states>\n");

    // Finally, the residuals an their correlations. All of them optional.

    stmp = navdir_data.get_output_residuals_file_name();
    stmp2 = navdir_data.get_output_residuals_correlations_file_name();

    if ((!stmp.empty()) | (!stmp2.empty()))
    {
      fprintf(navdir_file, "    <residuals>\n");

      if (!stmp.empty())
      {
        fprintf(navdir_file, "      <obs_e> %s </obs_e>\n", stmp.c_str());
      }

      if (!stmp2.empty())
      {
        fprintf(navdir_file, "      <tr_res_correlation_Rvv> %s </tr_res_correlation_Rvv>\n", stmp2.c_str());
      }

      fprintf(navdir_file, "    </residuals>\n");
    }

    // There are no more output files.

    fprintf(navdir_file, "  </output>\n");

    // This is the end of the file. Write the closing tag.

    fprintf(navdir_file, "</nav-directory_file>");

    // Close the file.

    fclose(navdir_file);

    // That's all.

    return 0;
  }
}
