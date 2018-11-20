/** \file lineage_data.hpp
  \brief Entity class defining the lineage section of many ASTROLABE XML files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _LINEAGE_DATA_HPP_
#define _LINEAGE_DATA_HPP_

#include <string>
#include <vector>

#include "astrolabe_exceptions.hpp"

using namespace std;

/// \brief Entity class defining the lineage section of many ASTROLABE XML files.
/**

  This class implents an in-memory representation of the \<lineage\>
  section of many ASTROLABE XML files.

  To make an easy to use, clear to undesrtand interface, methods have been named
  after the following pattern:

   \<label_1\>_\<label_2\>_..._\<label_n\>_\<action\>(parameters)

   where:

  - \<label_i\> stands for a label at a level i identifying a tag in
  the options file,
  - level 1 corresponds to the first level of labels just below tag
    \<lineage\>
  - \<action\> stands for either of the following:
    - set: methods that set a value for a given tag.
    - get: methods that retrieve the value assigned to a given tag.
    - add: methods that adds an additional value to a list of these. Used typically
           when a label may have repeated sublabels (item, for instance).
    - dim: methods that retrieve the number of values already stored in a list of these.

  According to these rules, the method to add an item to the list of authors, which
  is represented by tag

  \<lineage\>\<author\>\<item\>

  is

  author_item_add()

  To retrieve the value of tag
  
  \<lineage\>\<project\> the method to call would be:

  project_get()

*/

class lineage_data
{
  public:

    /// \brief Add an author to the list of authors.
    /**
      \pre (!author.empty())
      \param author The new author to add to the current list of authors.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The new author is added at the end of the list of authors. It may not
      be empty or an exception will be thrown.
    */

    void   author_item_add       (string& author);

    /// \brief Retrieve the number of authors included in the list of authors.
    /**
      \return The number of authors included in the list of authors. May be zero.
    */

    int    author_item_dim       (void) const;

    /// Retrieve an author from the list of authors.
    /**
      \pre (author_item_dim() > 0)
      \pre (at_position >= 0)
      \pre (at_position < authors_item_dim())
      \param at_position Index of the author to retrieve. The legal
             values for this parameter go from 0 to the actual number
             of authors in the list minus one (zero-based index).
      \return The author stored at position at_position in the list of
             authors.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      Use this method to retrieve a singled out author from the list
      of authors.

      Parameter at_position is used to select the author desired. This
      index may vary from 0 to the actual number of elements in the
      list of authors minus one.

      Using this method with an invalid at_position index or when the
      list is empty will produce an exception.
    */

    string author_item_get       (int at_position) const;

    /// \brief Retrieve the date and time
    /**
      \return The date and time. Note that it may be the empty string
             if it never was set using set_date_time();
    */

    string date_time_get         (void) const;

    /// \brief Set the date and time.
    /**
      \pre (!date_time.empty())
      \param date_time The date and time to set.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The date and time to set may not be an empty string or an exception will
      be thrown. See preconditions for details.
    */

    void   date_time_set         (string& date_time);

    /// \brief Retrieve the department.
    /**
      \return The department. Note that it may be the empty string
             if it never was set using set_department();
    */

    string department_get        (void) const;

    /// \brief Set the department.
    /**
      \pre (!department.empty())
      \param department The department to set.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The department to set may not be an empty string or an exception will
      be thrown. See preconditions for details.
    */

    void   department_set        (string& department);

    /// \brief Retrieve the identifier.
    /**
      \return The identifier. Note that it may be the empty string
             if it never was set using set_id();
    */

    string id_get                (void) const;

    /// \brief Set the identifier.
    /**
      \pre (!id.empty())
      \param id The identifier to set.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The identifier to set may not be an empty string or an exception will
      be thrown. See preconditions for details.
    */

    void   id_set                (string& id);

    /// \brief Default constructor.
           lineage_data          (void);

    /// Destructor.

           ~lineage_data         (void);

    /// \brief Retrieve the name.
    /**
      \return The name. Note that it may be the empty string
             if it never was set using set_name();
    */

    string name_get              (void) const;

    /// \brief Set the name.
    /**
      \pre (!name.empty())
      \param name The name to set.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The name to set may not be an empty string or an exception will
      be thrown. See preconditions for details.
    */

    void   name_set              (string& name);

    /// \brief Retrieve the organization.
    /**
      \return The organization. Note that it may be the empty string
             if it never was set using set_organization();
    */

    string organization_get      (void) const;

    /// \brief Set the organization.
    /**
      \pre (!organization.empty())
      \param organization The organization to set.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The organization to set may not be an empty string or an exception will
      be thrown. See preconditions for details.
    */

    void   organization_set      (string& organization);

    /// \brief Retrieve the project.
    /**
      \return The project. Note that it may be the empty string
             if it never was set using set_project();
    */

    string project_get           (void) const;

    /// \brief Set the project.
    /**
      \pre (!project.empty())
      \param project The project to set.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The project to set may not be an empty string or an exception will
      be thrown. See preconditions for details.
    */

    void   project_set           (string& project);

    /// \brief Add a reference document to the list of reference documents.
    /**
      \pre (!reference_document.empty())
      \param reference_document The new reference document to add to the current list of
             reference documents.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The new reference document is added at the end of the list of reference documents.
      It may not be empty or an exception will be thrown.
    */

    void   ref_document_item_add (string& reference_document);

    /// \brief Retrieve the number of reference documents included in the list
    ///        of reference documents.
    /**
      \return The number of reference documents included in the list of
              reference documents. May be zero.
    */

    int    ref_document_item_dim (void) const;

    /// Retrieve a reference document from the list of reference documents.
    /**
      \pre (ref_document_item_dim() > 0)
      \pre (at_position >= 0)
      \pre (at_position < ref_document_item_dim())
      \param at_position Index of the reference document to retrieve. The legal
             values for this parameter go from 0 to the actual number
             of reference documents in the list minus one (zero-based index).
      \return The reference document stored at position at_position in the list of
             reference documents.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      Use this method to retrieve a singled out reference document from the list
      of reference documents.

      Parameter at_position is used to select the reference document desired. This
      index may vary from 0 to the actual number of elements in the
      list of reference documents minus one.

      Using this method with an invalid at_position index or when the
      list is empty will produce an exception.
    */

    string ref_document_item_get (int at_position) const;

    /// \brief Retrieve the remarks.
    /**
      \return The remarks. Note that it may be the empty string
             if it never was set using set_remarks();
    */

    string remarks_get           (void) const;

    /// \brief Set the remarks.
    /**
      \pre (!remarks.empty())
      \param remarks The remarks to set.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The remarks to set may not be an empty string or an exception will
      be thrown. See preconditions for details.
    */

    void   remarks_set           (string& remarks);

    /// \brief Retrieve the task.
    /**
      \return The task. Note that it may be the empty string
             if it never was set using set_task();
    */

    string task_get              (void) const;


    /// \brief Set the task.
    /**
      \pre (!task.empty())
      \param task The task to set.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated. See the list of preconditions for more details.

      The task to set may not be an empty string or an exception will
      be thrown. See preconditions for details.
    */

    void   task_set              (string& task);

  protected:

    /// \brief List of authors.

    vector<string> author_;

    /// \brief Date and time.

    string date_time_;

    /// \brief Department.

    string department_;

    /// \brief Identifier.

    string id_;

    /// \brief Name.

    string name_;

    /// \brief Organization

    string organization_;

    /// \brief Project

    string project_;

    /// \brief List of reference documents.

    vector <string> ref_document_;

    /// \brief Remarks.

    string remarks_;

    /// \brief Task.

    string task_;

};

#endif
