/** \file astrolabe_header_file_writer.cpp 
  \brief Implementation file for astrolabe_header_file_writer.hpp.
  \ingroup ASTROLABE_data_IO 
*/ 

#include "astrolabe_header_file_writer.hpp"

astrolabe_header_file_writer::
astrolabe_header_file_writer
(void)
{
  {
    file_mode_is_binary_     = false;
    device_type_             = "";
    external_base_file_name_ = "";
    header_file_name_        = "";    
    lineage_info_            = NULL; 
  }
}

astrolabe_header_file_writer::
~astrolabe_header_file_writer
(void)
{
  {
    // Intentionally left blank.
  }
}

void
astrolabe_header_file_writer::
set_data_channel
(const string&       astrolabe_header_file_name,
 const string&       external_base_file_name,
 const string&       device_type,
 bool                binary,
 const lineage_data* lineage_info)
{
  {
    device_type_             = device_type;
    header_file_name_        = astrolabe_header_file_name;
    external_base_file_name_ = external_base_file_name;
    file_mode_is_binary_     = binary;
    lineage_info_            = lineage_info;
  }
}


int
astrolabe_header_file_writer::
write
(void)
{
  {
    FILE*              header_file;
    int                i;
    time_t             now;
    string             stmp;
    char               time_buffer[100];
    int                total_items;

    //
    // Check our preconditions. The following strings may not
    // be empty:
    //

    if (device_type_.empty())             return 1;
    if (external_base_file_name_.empty()) return 1;
    if (header_file_name_.empty())        return 1;

    // The lineage pointer must also be non-NULL.

    if (lineage_info_ == NULL)            return 1;

    // Let's open a new text file.

    header_file = fopen(header_file_name_.c_str(), "w");
    if (header_file == NULL) return 2;

    // Let's write the header!!!

    fprintf(header_file, "<astrolabe-header_file xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
    fprintf(header_file, "  version=\"1.0\" xsi:noNamespaceSchemaLocation=\"astrolabe-header_file.xsd\">\n");
    fprintf(header_file,"  <lineage version=\"1.0\">\n");

    // Dump the lineage object completely (at least, those fields that have been set).

    // Dump the id (the only mandatory field).

    stmp = lineage_info_->id_get();
    fprintf(header_file,"    <id> %s </id>\n", stmp.c_str());

    // Name.

    stmp = lineage_info_->name_get();
    if (!stmp.empty())
    {
      fprintf(header_file,"    <name> %s </name>\n", stmp.c_str());
    }

    // Authors

    total_items = lineage_info_->author_item_dim();
    if (total_items > 0)
    {
      fprintf(header_file,"    <author>\n");
      for (i = 0; i < total_items; i++)
      {
        stmp = lineage_info_->author_item_get(i);
        fprintf(header_file,"      <item> %s </item>\n", stmp.c_str());
      }
      fprintf(header_file,"    </author>\n");
    }

    // Organization

    stmp = lineage_info_->organization_get();
    if (!stmp.empty())
    {
      fprintf(header_file,"    <organization> %s </organization>\n", stmp.c_str());
    }

    // Department

    stmp = lineage_info_->department_get();
    if (!stmp.empty())
    {
      fprintf(header_file,"    <department> %s </department>\n", stmp.c_str());
    }

    // Date and time.

    //
    // We ignore the date and time that could have been stored in the lineage
    // object, since we'll write the time associated to the moment when
    // this header is written.
    //

    now = time(0);
    strftime(time_buffer, 100, "%Y-%m-%dT%H:%M:%S.000", localtime (&now));
    fprintf(header_file,"    <date_time>%s</date_time>\n", time_buffer);

    // Reference documents.

    total_items = lineage_info_->ref_document_item_dim();
    if (total_items > 0)
    {
      fprintf(header_file,"    <ref_document>\n");
      for (i = 0; i < total_items; i++)
      {
        stmp = lineage_info_->ref_document_item_get(i);
        fprintf(header_file,"      <item> %s </item>\n", stmp.c_str());
      }
      fprintf(header_file,"    </ref_document>\n");
    }

    // Project

    stmp = lineage_info_->project_get();
    if (!stmp.empty())
    {
      fprintf(header_file,"    <project> %s </project>\n", stmp.c_str());
    }

    // Task

    stmp = lineage_info_->task_get();
    if (!stmp.empty())
    {
      fprintf(header_file,"    <task> %s </task>\n", stmp.c_str());
    }

    // Remarks

    stmp = lineage_info_->remarks_get();
    if (!stmp.empty())
    {
      fprintf(header_file,"    <remarks> %s </remarks>\n", stmp.c_str());
    }


    // Close the lineage tag.

    fprintf(header_file,"  </lineage>\n");

    // Describe the data channel characteristics.

    fprintf(header_file, "  <data>\n");


    if (file_mode_is_binary_) stmp = "binary_file";
    else                      stmp = "text_file";

    fprintf(header_file, "    <device type=\"%s\" format=\"%s\"> %s </device>\n",
                         device_type_.c_str(), stmp.c_str(), external_base_file_name_.c_str());

    fprintf(header_file, "  </data>\n");

    fprintf(header_file, "</astrolabe-header_file>");

    // Close the file.

    fclose(header_file);

    // That's all.

    return 0;
  }
}