#include "RecordStructure.hpp"

RecordStructure::
RecordStructure
(void)
{
  {

    l_records_available_ = 4;

    l_identifiers_[0] = "LR01";
    l_identifiers_[1] = "LR02";
    l_identifiers_[2] = "LR03";
    l_identifiers_[3] = "LR04";

    l_n_tags_[0] = 1;
    l_n_tags_[1] = 0;
    l_n_tags_[2] = 2;
    l_n_tags_[3] = 0;

    l_n_expectations_[0] = 2;
    l_n_expectations_[1] = 1;
    l_n_expectations_[2] = 3;
    l_n_expectations_[3] = 2;

    lid_to_index[l_identifiers_[0]] = 0;
    lid_to_index[l_identifiers_[1]] = 1;
    lid_to_index[l_identifiers_[2]] = 2;
    lid_to_index[l_identifiers_[3]] = 3;
  }
}

bool
RecordStructure::
l_record_data
(const char* id,
 int&        n_tags,
 int&        n_expectations)
{
  {
    string looking_for(id);
    map<string,int>::iterator finder;
    int index;

    finder = lid_to_index.find(looking_for);
    if (finder == lid_to_index.end()) return false;

    index = finder->second;

    n_tags = l_n_tags_[index];
    n_expectations = l_n_expectations_[index];


    return true;
  }
}

bool
RecordStructure::
l_record_data
(int     index,
 string& id,
 int&    n_tags,
 int&    n_expectations)
{
  {
    if (index < 0) return false;
    if (index >= l_records_available_) return false;

    id = l_identifiers_[index];
    n_tags = l_n_tags_[index];
    n_expectations = l_n_expectations_[index];

    return true;
  }
}

int
RecordStructure::
n_l_record_types
(void)
{
  {
    return l_records_available_;
  }
}
