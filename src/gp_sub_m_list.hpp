/** \file gp_sub_m_list.hpp
  \brief Entity class to hold the \<sub-m_list\> element in
         ASTROLABE metadata files.
  \ingroup ASTROLABE_metadata
*/

#ifndef _GP_SUB_M_LIST_HPP_
#define _GP_SUB_M_LIST_HPP_

#include "astrolabe_exceptions.hpp"

#include "lineage_data.hpp"

#include <string>
#include <map>
using namespace std;

/// \brief Entity class to hold the \<sub-m_list\> element in ASTROLABE metadata files.

class gp_sub_m_list
{
  public:

  /// \brief Check if an item with attribute "n" equal to n already exists.
    /**
      \param n The value of the n attribute.
      \return True if a an item with an "n" attribute whose value
             is equal to the input parameter n exists, false otherwise.
    */
    
    bool   exists_item       (int n) const;

    /// \brief Retrieve the number of \<item\>s that may be stored in
    /// the gp_sub_m_list object.

    int    get_dim           (void);

    /// \brief Retrieve the identifier of the \<item\> whose "n" attribute is n.
    /**
      \pre (exists_item(n))
      \pre ((n >= 1) && (n <= get_dim()))
      \param n The value of the "n" attribute of the \<item\> whose identifier is to be retrieved.
             It must be in the range [1..get_dim()].
      \return The identifier of the \<item\> whose "n" attribute is n.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    string get_item_id       (int n);

    /// \brief Default constructor.

         gp_sub_m_list       (void);

    /// \brief Destructor.

         ~gp_sub_m_list      (void);

    /// \brief Set the number of \<item\>s composing the gp_sub_m_list object.
    /**
      \pre (get_dim() == 0)
      \pre (n_items > 0)
      \param n_items Number of \<item\>s to be stored in the
             gp_sub_m_list object.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.

      Use this method to indicate how many \<item\>s will be stored in the
      gp_sub_m_list object.

      It may be called only once (typically, after the gp_sub_m_list object has been
      instantiated); that is why the precondition states that the current
      dimension (number of sub-models) must be zero.

      Obviously, parameter n_items must be strictly positive, so at least
      one \<item\> may be stored.
    */

    void set_dim             (int n_items);

    /// \brief Set the identifier of the \<item\> element at
    /// a given position.
    /**
      \pre (!exists_item(n))
      \pre ((n >= 1) && (n <= get_dim()))
      \pre (!id.empty())
      \param n The value of the "n" attribute of the \<item\> whose identifier
             has to be set. It must be in the range [1..get_dim()].
      \param id The identifier of the \<item\> whose "n" attribute is n. May not be
             empty.
      \throw astrolabe_precondition_violation One or more preconditions have been
             violated.
    */

    void set_item_id         (int n, string& id);

  protected:

    /// \brief Number of \<item\>s that may be managed by the sub-m_list object.

    int              n_items_;

    /// \brief The identifiers of the several \<item\> elements included in
    /// the \<sub-m_list\> element.

    map<int, string> item_ids_;
};

#endif // _GP_SUB_M_LIST_HPP_
