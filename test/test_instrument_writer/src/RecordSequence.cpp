#include "RecordSequence.hpp"

int
RecordSequence::
cycle_length
(void)
{
  {
    return record_sequence_length_;
  }
}

void
RecordSequence::
get_next
(int& record_id_index, int& cycle)
{
  {
    record_id_index = record_sequence_ids_[record_sequence_index_];
    cycle           = record_sequence_cycle_;

    record_sequence_index_++;

    if (record_sequence_index_ == record_sequence_length_)
    {
      record_sequence_index_ = 0;
      record_sequence_cycle_++;
    }
  }
}

RecordSequence::
RecordSequence
(void)
{
  {
    record_sequence_length_ = 10;
    record_sequence_ids_  = new int[record_sequence_length_];

    record_sequence_ids_[0]  = 3;
    record_sequence_ids_[1]  = 2;
    record_sequence_ids_[2]  = 1;
    record_sequence_ids_[3]  = 1;
    record_sequence_ids_[4]  = 0;
    record_sequence_ids_[5]  = 3;
    record_sequence_ids_[6]  = 2;
    record_sequence_ids_[7]  = 1;
    record_sequence_ids_[8]  = 0;
    record_sequence_ids_[9]  = 1;

    record_sequence_index_ = 0;
    record_sequence_cycle_ = 0;
  }
}

RecordSequence::
~RecordSequence
(void)
{
  {
    delete record_sequence_ids_;
  }
}
