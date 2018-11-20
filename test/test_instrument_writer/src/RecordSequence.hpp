#ifndef RECORDSEQUENCE_HPP
#define RECORDSEQUENCE_HPP


class RecordSequence
{
  public:

    /// @brief Return the length of the sequence cycle

    int cycle_length (void);

    /**
     * @brief Get the next item in the sequence.
     * @param record_id_index The index of the identifier of the
     *        record to generate.
     * @param cycle Cycle number. Each time that the sequence is
     *        exhausted, this cycle number is increased by one. May
     *        be used to simulate time tags.
     */

    void get_next(int& record_id_index, int& cycle);

    /// \brief Default constructor.

    RecordSequence  (void);

    /// \brief Destructor

    ~RecordSequence (void);

  protected:

    /// Current index of the record sequence.

    int record_sequence_index_;

    /// Current record sequence cycle.

    int record_sequence_cycle_;

    /// Period of the record sequence.

    int record_sequence_length_;

    ///
    /// Record sequence. Identifier indexes of the successive
    /// records in the sequence.
    ///

    int* record_sequence_ids_;

};

#endif // RECORDSEQUENCE_HPP
