/** \file lineage_data.cpp
  \brief Implementation file for lineage_data.hpp
  \ingroup ASTROLABE_metadata
*/

#include "lineage_data.hpp"

void
lineage_data::
author_item_add
(string& author)
{
  set_precondition (!author.empty());
  {
    author_.push_back(author);
  }
}

int
lineage_data::
author_item_dim
(void)
const
{
  {
    return (int)author_.size();
  }
}

string
lineage_data::
author_item_get
(int at_position)
const
{
  set_precondition (author_item_dim() > 0);
  set_precondition (at_position >= 0);
  set_precondition (at_position < author_item_dim());

  string result;

  {

    result = author_[at_position];
  }

  return result;
}

string
lineage_data::
date_time_get
(void)
const
{
  string result;

  {
    result = date_time_;
  }

  return result;
}

void
lineage_data::
date_time_set
(string& date_time)
{
  set_precondition (!date_time.empty());

  {
    date_time_ = date_time;
  }
}

string
lineage_data::
department_get
(void)
const
{
  string result;

  {
    result = department_;
  }

  return result;
}

void
lineage_data::
department_set
(string& department)
{
  set_precondition (!department.empty());

  {
    department_ = department;
  }
}

string
lineage_data::
id_get
(void)
const
{
  string result;

  {
    result = id_;
  }

  return result;
}

void
lineage_data::
id_set
(string& id)
{
  set_precondition (!id.empty());

  {
    id_ = id;
  }
}

lineage_data::
lineage_data
(void)
{
  {
    author_.clear();
    ref_document_.clear();

    date_time_    = "";
    department_   = "";
    id_           = "";
    name_         = "";
    organization_ = "";
    project_      = "";
    remarks_      = "";
    task_         = "";
  }
}


lineage_data::
~lineage_data
(void)
{
  {
    author_.clear();
    ref_document_.clear();
  }
}

string
lineage_data::
name_get
(void)
const
{
  string result;

  {
    result = name_;
  }

  return result;
}

void
lineage_data::
name_set
(string& name)
{
  set_precondition (!name.empty());

  {
    name_ = name;
  }
}

string
lineage_data::
organization_get
(void)
const
{
  string result;

  {
    result = organization_;
  }

  return result;
}

void
lineage_data::
organization_set
(string& organization)
{
  set_precondition (!organization.empty());

  {
    organization_ = organization;
  }
}

string
lineage_data::
project_get
(void)
const
{
  string result;

  {
    result = project_;
  }

  return result;
}

void
lineage_data::
project_set
(string& project)
{
  set_precondition (!project.empty());

  {
    project_ = project;
  }
}

void
lineage_data::
ref_document_item_add
(string& ref_document)
{
  set_precondition (!ref_document.empty());
  {
    ref_document_.push_back(ref_document);
  }
}

int
lineage_data::
ref_document_item_dim
(void)
const
{
  {
    return (int)ref_document_.size();
  }
}

string
lineage_data::
ref_document_item_get
(int at_position)
const
{
  set_precondition (ref_document_item_dim() > 0);
  set_precondition (at_position >= 0);
  set_precondition (at_position < ref_document_item_dim());

  string result;

  {

    result = ref_document_[at_position];
  }

  return result;
}

string
lineage_data::
remarks_get
(void)
const
{
  string result;

  {
    result = remarks_;
  }

  return result;
}

void
lineage_data::
remarks_set
(string& remarks)
{
  set_precondition (!remarks.empty());

  {
    remarks_ = remarks;
  }
}

string
lineage_data::
task_get
(void)
const
{
  string result;

  {
    result = task_;
  }

  return result;
}

void
lineage_data::
task_set
(string& task)
{
  set_precondition (!task.empty());

  {
    task_ = task;
  }
}
