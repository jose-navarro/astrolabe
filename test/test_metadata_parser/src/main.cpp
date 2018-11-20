#include "gp_metadata_parser.hpp"
#include "gp_metadata.hpp"

#include <iostream>
using namespace std;

void print_lineage         (int indent, lineage_data*       lineage);
void print_clist_item_type (int indent, gp_clist_item_type* gci_type);
void print_ref             (int indent, gp_ref*             ref);
void print_tspec           (int indent, gp_tspec*           tspec);

int main
(int   argC,
 char* argV[])
{
  {

    // The parser.

    gp_metadata_parser  metadata_parser;

    //
    // The metadata object that will hold the values parsed from the
    // input file.
    //

    gp_metadata         metadata;

    // Input metadata, UDUNITS2 units database and XML schema file names.

    vector<string>      metadata_files;
    string              udunits_units_database;
    string              xml_schema;

    //
    // Auxiliary pointers / objects used to retrieve the metadata
    // once loaded.
    //

    bool                baux;
    gp_clist*           clist;
    gp_clist_item       citem;
    gp_clist_item_type* citem_type;
    gp_covariance*      cov;
    double              daux;
    gp_ilist*           ilist;
    gp_ispec            ispec;
    lineage_data*       lineage;
    gp_llist*           llist;
    gp_lspec            lspec;
    gp_mspec            mspec;
    gp_plist_item       pitem;
    gp_plist*           plist;
    gp_pspec            pspec;
    gp_ref*             ref;
    string              saux;
    string              saux2;
    gp_scale*           scale;
    gp_sub_m_list*      submlist;
    gp_tspec*           tspec;
    gp_timespec*        timespec;
    gp_unit*            units;

    // Error and warning handling variables.

    int                 error_total;
    int                 error_index;
    string              error_message;
    int                 warning_total;
    int                 warning_index;
    string              warning_message;

    // Auxiliar variables used in loops.

    int                 i;
    int                 j;
    int                 k;
    int                 list_dim, list_dim2;
    int                 total_items;

    //
    // Test the number of parameters. We must receive an input metadata file!
    // argV[0] is the name of the program and argV[1] is the name of the
    // input file. argV[2] is the full path to the UDUNITS2 file defining its
    // units and, finally, we may also receive the name of a schema in argV[3];
    //

    if (argC < 3)
    {
      cout << "Usage: test_metadata_parser metadata_file UDUNITS2_units_database_file [schema_file]" << endl;
      return 1;
    }

    //
    // Depending on the number of parameters, we'll have just a metadata file
    // or, additionally, an schema to validate the syntax of the former.
    //
    // Set up the parser depending on that.
    //

    metadata_files.push_back(argV[1]);
    udunits_units_database = argV[2];

    if (argC == 3)
    {
      // Developer's setup (NO schema to help validating the input file)


      metadata_parser.set_parser_parameters_developer(metadata_files,
                                                      udunits_units_database,
                                                      &metadata);
    }
    else
    {
      // Production set up, with a schema included.

      xml_schema = argV[3];
      metadata_parser.set_parser_parameters(metadata_files,
                                            udunits_units_database,
                                            xml_schema,
                                            &metadata);
    }

    // Parse the input file.

    metadata_parser.parse();

    // Check if the parsing process produced warnings or errors.

    error_total = metadata_parser.errors_dim();

    if (error_total > 0)
    {
      cout << "Parsing finished with errors!" << endl << endl;

      for (error_index = 0; error_index < error_total; error_index++)
      {
        error_message = metadata_parser.errors_get(error_index);
        cout << error_message << endl;
      }
    }
    else
    {
      cout << "Parsing finished with no errors!" << endl;
    }

    warning_total = metadata_parser.warnings_dim();

    if (warning_total > 0)
    {
      cout << "Parsing finished with warnings!" << endl << endl;

      for (warning_index = 0; warning_index < warning_total; warning_index++)
      {
        warning_message = metadata_parser.warnings_get(warning_index);
        cout << warning_message << endl;
      }
    }
    else
    {
      cout << "Parsing finished with no warnings!" << endl;
    }

    //
    // If errors have been detected, stop the process here.
    //
    // Otherwise, dump the metadata to screen. This will show how to
    // access the information stored in the metadata object.
    //

    if (error_total > 0) return 0;

    // -----------------------------
    // 1 - Dump the p_spec metadata.
    // -----------------------------

    total_items = metadata.get_dim_pspec();

    for (i = 0; i < total_items; i++)
    {
      cout << "p_spec #" << i + 1 << endl;

      // Retrieve the ith pspec in the metadata object.

      pspec = metadata.get_pspec(i);

      // Retrieve the lineage and all its elements.

      lineage = pspec.get_lineage_ptr();
      print_lineage(2, lineage);

      // Retrieve its type.

      saux = pspec.get_type();
      cout << "  type: " << saux << endl;

      // Retrieve its toolbox (optional).

      saux = pspec.get_toolbox();
      if (!saux.empty()) cout << "  toolbox: " << saux << endl;

      // Retrieve its dimension.

      j = pspec.get_dimension();
      cout << "  dimension: " << j << endl;

      // Retrieve a pointer to its ref. Print it.

      ref = pspec.get_ref_ptr();
      print_ref(2, ref);

      //
      // Retrieve a pointer to its time_spec. Print it.
      // Note that it contains a ref plus a units object!
      //

      timespec = pspec.get_timespec_ptr();

      cout << "  time spec" << endl;

      ref = timespec->get_ref_ptr();
      print_ref(4, ref);

      units = timespec->get_unit_ptr();
      list_dim = units->get_dim();

      cout << "    units:";

      for (j = 0; j < list_dim; j++)
      {
        saux = units->get_unit(j);
        cout << " " << saux;
      }
      cout << endl;

      // Retrieve the units (if any!)

      units = pspec.get_unit_ptr();
      list_dim = units->get_dim();

      if (list_dim > 0)
      {
        cout << "  units:";

        for (j = 0; j < list_dim; j++)
        {
          saux = units->get_unit(j);
          cout << " " << saux;
        }
        cout << endl;
      }

      // Retrieve the covariance (if any!)

      cov = pspec.get_covariance_ptr();
      list_dim = cov->get_dim();

      if (list_dim > 0)
      {
        cout << "  covariance matrix:";

        for (j = 0; j < list_dim; j++)
        {
          daux = cov->get_value(j);
          cout << " " << daux;
        }
        cout << endl;
      }

      // Retrieve the scale factors (if any!)

      scale = pspec.get_scale_ptr();
      list_dim = scale->get_dim();

      if (list_dim > 0)
      {
        cout << "  scale factors:";

        for (j = 0; j < list_dim; j++)
        {
          daux = scale->get_value(j);
          cout << " " << daux;
        }
        cout << endl;
      }

      // Retrieve the t_spec (if any).

      tspec = pspec.get_tspec_ptr();
      if (tspec->get_dimension() > 0) print_tspec (2, tspec);

    }

    // -----------------------------
    // 2 - Dump the i_spec metadata.
    // -----------------------------

    total_items = metadata.get_dim_ispec();

    for (i = 0; i < total_items; i++)
    {
      cout << "i_spec #" << i + 1 << endl;

      // Retrieve the ith ispec in the metadata object.

      ispec = metadata.get_ispec(i);

      // Retrieve the lineage and all its elements.

      lineage = ispec.get_lineage_ptr();
      print_lineage(2, lineage);

      // Retrieve its type.

      saux = ispec.get_type();
      cout << "  type: " << saux << endl;

      // Retrieve its toolbox (optional).

      saux = ispec.get_toolbox();
      if (!saux.empty()) cout << "  toolbox: " << saux << endl;

      //
      // Retrieve a pointer to its time_spec. Print it.
      // Note that it contains a ref plus a units object!
      //

      timespec = pspec.get_timespec_ptr();

      cout << "  time spec" << endl;

      ref = timespec->get_ref_ptr();
      print_ref(4, ref);

      units = timespec->get_unit_ptr();
      list_dim = units->get_dim();

      cout << "    units:";

      for (j = 0; j < list_dim; j++)
      {
        saux = units->get_unit(j);
        cout << " " << saux;
      }
      cout << endl;

      // Retrieve the list of constants.

      clist = ispec.get_clist_ptr();
      cout << "  c_list " << endl;

      list_dim2 = clist->get_dim();

      cout << "    dimension: " << list_dim2 << endl;

      for (k = 1; k <= list_dim2; k++)
      {
        citem = clist->get_item(k);
        cout << "    item # " << k << endl;

        // item type.

        citem_type = citem.get_type_ptr();
        print_clist_item_type(6, citem_type);

        // item ref.

        // Retrieve a pointer to its ref. Print it.

        ref = citem.get_ref_ptr();
        print_ref(6, ref);

        // Retrieve the units (if any!)

        units = citem.get_unit_ptr();
        list_dim = units->get_dim();

        if (list_dim > 0)
        {
          cout << "      units:";

          for (j = 0; j < list_dim; j++)
          {
            saux = units->get_unit(j);
            cout << " " << saux;
          }
          cout << endl;
        }
      }

      // Retrieve the t_spec (if any).

      tspec = ispec.get_tspec_ptr();
      if (tspec->get_dimension() > 0) print_tspec (2, tspec);
      
    }

    // -----------------------------
    // 3 - Dump the l_spec metadata.
    // -----------------------------

    total_items = metadata.get_dim_lspec();

    for (i = 0; i < total_items; i++)
    {
      cout << "l_spec #" << i + 1 << endl;

      // Retrieve the ith lspec in the metadata object.

      lspec = metadata.get_lspec(i);

      // Retrieve the lineage and all its elements.

      lineage = lspec.get_lineage_ptr();
      print_lineage(2, lineage);

      // Retrieve its type.

      saux = lspec.get_type();
      cout << "  type: " << saux << endl;

      // Retrieve its toolbox (optional).

      saux = lspec.get_toolbox();
      if (!saux.empty()) cout << "  toolbox: " << saux << endl;

      // Retrieve its dimension.

      j = lspec.get_dimension();
      cout << "  dimension: " << j << endl;

      // Retrieve a pointer to its ref. Print it.

      ref = lspec.get_ref_ptr();
      print_ref(2, ref);

      //
      // Retrieve a pointer to its time_spec. Print it.
      // Note that it contains a ref plus a units object!
      //

      timespec = pspec.get_timespec_ptr();

      cout << "  time spec" << endl;

      ref = timespec->get_ref_ptr();
      print_ref(4, ref);

      units = timespec->get_unit_ptr();
      list_dim = units->get_dim();

      cout << "    units:";

      for (j = 0; j < list_dim; j++)
      {
        saux = units->get_unit(j);
        cout << " " << saux;
      }
      cout << endl;

      // Retrieve the units (if any!)

      units = lspec.get_unit_ptr();
      list_dim = units->get_dim();

      if (list_dim > 0)
      {
        cout << "  units:";

        for (j = 0; j < list_dim; j++)
        {
          saux = units->get_unit(j);
          cout << " " << saux;
        }
        cout << endl;
      }

      // Retrieve the covariance (if any!)

      cov = lspec.get_covariance_ptr();
      list_dim = cov->get_dim();

      if (list_dim > 0)
      {
        cout << "  covariance matrix:";

        for (j = 0; j < list_dim; j++)
        {
          daux = cov->get_value(j);
          cout << " " << daux;
        }
        cout << endl;
      }

      // Retrieve the scale factors (if any!)

      scale = lspec.get_scale_ptr();
      list_dim = scale->get_dim();

      if (list_dim > 0)
      {
        cout << "  scale factors:";

        for (j = 0; j < list_dim; j++)
        {
          daux = scale->get_value(j);
          cout << " " << daux;
        }
        cout << endl;
      }

      // Retrieve the t_spec (if any).

      tspec = lspec.get_tspec_ptr();
      if (tspec->get_dimension() > 0) print_tspec (2, tspec);
    }

    // -----------------------------
    // 4 - Dump the m_spec metadata.
    // -----------------------------

    total_items = metadata.get_dim_mspec();

    for (i = 0; i < total_items; i++)
    {
      cout << "m_spec #" << i + 1 << endl;

      // Retrieve the ith mspec in the metadata object.

      mspec = metadata.get_mspec(i);

      // Retrieve the lineage and all its elements.

      lineage = mspec.get_lineage_ptr();
      print_lineage(2, lineage);

      // Retrieve its type.

      saux = mspec.get_type();
      cout << "  type: " << saux << endl;

      // Retrieve its toolbox (optional).

      saux = mspec.get_toolbox();
      if (!saux.empty()) cout << "  toolbox: " << saux << endl;

      // Is it dynamic?

      baux = mspec.get_dynamic();
      if (baux) cout << "  dynamic: YES" << endl;
      else      cout << "  dynamic: NO" << endl;

      // Process the l_list element.

      llist = mspec.get_llist_ptr();

      cout << "  clist" << endl;

      // Get its dimension.

      list_dim = llist->get_dim();
      cout << "    dimension: " << list_dim << endl;

      // Process all the item elements in the l_list.

      for (j = 1; j <= list_dim; j++)
      {
        cout << "    item #" << j << endl;
        saux = llist->get_item_id(j);
        cout << "      id: " << saux << endl;
      }

      // Process the p_list element.

      plist = mspec.get_plist_ptr();

      cout << "  plist" << endl;

      // Get its dimension.

      list_dim = plist->get_dim();
      cout << "    dimension: " << list_dim << endl;

      // Process all the item elements in the p_list.

      for (j = 1; j <= list_dim; j++)
      {
        pitem = plist->get_item(j);
        cout << "    item #" << j << endl;
        saux = pitem.get_id();
        cout << "      id  : " << saux << endl;
        saux = pitem.get_role();
        cout << "      role: " << saux << endl;
      }

      // Process the OPTIONAL i_list element.

      ilist = mspec.get_ilist_ptr();
      list_dim = ilist->get_dim();

      if (list_dim > 0)
      {
        cout << "  ilist" << endl;

        // Print (since we already have) its dimension.

        cout << "    dimension: " << list_dim << endl;

        // Process all the item elements in the l_list.

        for (j = 1; j <= list_dim; j++)
        {
          cout << "    item #" << j << endl;
          saux = ilist->get_item_id(j);
          cout << "      id: " << saux << endl;
        }
      }

      // Process the OPTIONAL sub-m_list element.

      submlist = mspec.get_sub_m_list_ptr();
      list_dim = submlist->get_dim();

      if (list_dim > 0)
      {
        cout << "  sub-m_list" << endl;

        // Print (since we already have) its dimension.

        cout << "    dimension: " << list_dim << endl;

        // Process all the item elements in the sub-m_list.

        for (j = 1; j <= list_dim; j++)
        {
          cout << "    item #" << j << endl;
          saux = submlist->get_item_id(j);
          cout << "      id: " << saux << endl;
        }
      }
    }

    // THAT'S ALL.

    cout << "Process finished" << endl;
  }
  return 0;
}

void
print_lineage
(int indent,
 lineage_data* lineage)
{
  {
    int    i;
    int    j;
    int    list_dim;
    string saux;
    string tab1, tab2, tab3;

    tab1 = "";
    for (i = 0; i < indent; i++) tab1 += " ";
    tab2 = tab1 + "  ";
    tab3 = tab2 + "  ";


    cout << tab1 << "lineage" << endl;

    saux = lineage->id_get();
    cout << tab2 << "id: " << saux << endl;

    saux = lineage->name_get();
    if (!saux.empty()) cout << tab2 << "name: " << saux << endl;

    saux = lineage->date_time_get();
    if (!saux.empty()) cout << tab2 << "date & time: " << saux << endl;

    list_dim = lineage->author_item_dim();
    if (list_dim > 0)
    {
      cout << tab2 << "author" << endl;
      for (j = 0; j < list_dim; j++)
      {
        saux = lineage->author_item_get(j);
        cout << tab3 << "item: " << saux << endl;
      }
    }

    saux = lineage->organization_get();
    if (!saux.empty()) cout << tab2 << "organization: " << saux << endl;

    saux = lineage->department_get();
    if (!saux.empty()) cout << tab2 << "department: " << saux << endl;

    saux = lineage->date_time_get();
    if (!saux.empty()) cout << tab2 << "date_time: " << saux << endl;

    list_dim = lineage->ref_document_item_dim();
    if (list_dim > 0)
    {
      cout << tab2 << "ref_document" << endl;
      for (j = 0; j < list_dim; j++)
      {
        saux = lineage->ref_document_item_get(j);
        cout << tab3 << "item: " << saux << endl;
      }
    }

    saux = lineage->project_get();
    if (!saux.empty()) cout << tab2 << "project: " << saux << endl;

    saux = lineage->task_get();
    if (!saux.empty()) cout << tab2 << "task: " << saux << endl;

    saux = lineage->remarks_get();
    if (!saux.empty()) cout << tab2 << "remarks: " << saux << endl;
  }
}

void
print_clist_item_type
(int indent,
 gp_clist_item_type* gci_type)
{
  {
    int    i;
    int    dimension;
    string tab1;
    int    total_dimensions;

    tab1 = "";
    for (i = 0; i < indent; i++) tab1 += " ";
    cout << tab1 << "type: ";

    //
    // Depending on the item type (scalar, matrix) we have,
    // we have to print it in this or that way.
    //

    if (gci_type->is_scalar())
    {
      cout << "scalar" << endl;
      return;
    }

    //
    // Well, here we have a matrix type. We've got to print
    // not only the keyword "matrix" but also the opening
    // and closing parenthesis as well as the dimensions of
    // such matrix.
    //

    cout << "matrix(";
    total_dimensions = gci_type->n_of_matrix_dimensions();
    for (i = 0; i < total_dimensions - 1; i++)
    {
      gci_type->get_dimension_at(i, dimension);
      cout << dimension << ",";
    }

    gci_type->get_dimension_at(total_dimensions - 1, dimension);
    cout << dimension << ")" << endl;
  }
}

void
print_ref
(int     indent,
 gp_ref* ref)
{
  {
    int         i;
    int         j;
    int         list_dim;
    gp_ref_code ref_code;
    int         ref_type;
    string      saux;
    string      saux2;
    string      tab1;
    string      tab2;

    tab1 = "";
    for (i = 0; i < indent; i++) tab1 += " ";

    // Discover what type of ref we've got.

    ref_type  = ref->get_type();

    if (ref_type == gp_ref::GP_REF_TYPE_CRF)
    {
      // Coordinate reference frame.

      list_dim = ref->get_dim();

      // Retrieve the codes in the list of coordinate reference frames.

      cout << tab1 << "CRF:";

      for (j = 0; j < list_dim; j++)
      {
        ref_code = ref->get_CRF(j);
        saux = ref_code.to_string();
        cout << " " << saux;
      }
      cout << endl;
    }
    else // (ref_type == gp_ref::GP_REF_TYPE_CS_RF)
    {
      // Reference frame plus optional coordinate system.

      list_dim = ref->get_dim();

      //
      // Retrieve the mandatory codes in the list of reference frames.
      // Get also the OPTIONAL codes of the coordinate systems. These
      // may be empty strings!!!
      //

      cout << tab1 << "RF&CS:";

      for (j = 0; j < list_dim; j++)
      {
        ref_code  = ref->get_RF(j);
        saux = ref_code.to_string();

        cout << " " << saux;

        saux2 = "";
        if (ref->has_CS())
        {
          ref_code = ref->get_CS(j);
          saux2 = ref_code.to_string();
          cout << "&" << saux2;
        } 
      }
      cout << endl;
    }
  }
}

void
print_tspec
(int       indent,
 gp_tspec* tspec)
{
  {

    int      i;
    int      j;
    int      list_dim;
    gp_ref*  ref;
    string   saux;
    string   tab1, tab2;
    gp_unit* units;

    tab1 = "";
    for (i = 0; i < indent; i++) tab1 += " ";
    tab2 = tab1 + "  ";

    cout << tab1 << "t_spec" << endl;

    // Get the dimension.

    j = tspec->get_dimension();
    cout << tab2 << "dimension: " << j << endl;

    // Get the ref pointer. Print it.

    ref = tspec->get_ref_ptr();
    print_ref(indent + 2, ref);

    // Get the units pointer.

    units = tspec->get_unit_ptr();
    list_dim = units->get_dim();

    cout << tab2 << "Units:";

    for (j = 0; j < list_dim; j++)
    {
      saux = units->get_unit(j);
      cout << " " << saux;
    }
    cout << endl;

  }
}
