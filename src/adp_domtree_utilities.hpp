/** \file adp_domtree_utilities.hpp
  \brief General utilities to simplify the process of parsing XML files via DOM.
  \ingroup adp_DOMparser_group
*/

#ifndef __ADP_DOMTREE_UTILITIES_HPP__
#define __ADP_DOMTREE_UTILITIES_HPP__

#include <stdio.h>

#include "astrolabe_string_utilities.hpp"
#include "astrolabe_exceptions.hpp"

#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

/// \brief General utilities to simplify the process of parsing XML files via DOM.

/**

  Provides with a set of methods performing some common,
  repetitive tasks involved in parsing of XML files when
  a Xerces DOM parser is uded..

  */

class adp_domtree_utilities
{
public:

  /// \brief Default constructor.
 
              adp_domtree_utilities               (void);

  /// \brief Default destructor.

              ~adp_domtree_utilities              (void);


  /// \brief Given a DOMNode::ELEMENT_NODE, retrieve its name (tag name).
  /**
    \pre node != NULL

    \param node Node whose name we are interested in.
    \throw astrolabe_exception Not enough memory to
           complete the request.
    \return The name of the node or NULL if node was NULL.

    Given a DOMNode::ELEMENT_NODE node, this method returns its
    name.
    
    The result is already a char* C type; all the transcoding
    operations required to handle XMLString(s) are already applied.
  */

  char*       get_element_name                    (const DOMNode* node) const;

  /// \brief Given a DOMNode::ELEMENT_NODE, retrieve its first
  /// DOMNode::ELEMENT_NODE child.
  /**
    \pre node != NULL

    \param node Parent node of the first DOMNode::ELEMENT_NODE child sought.
    \return The first DOMNode::ELEMENT_NODE child of node, or NULL if node
            was null.

    Given a DOMNode::ELEMENT_NODE node, this method returns its
    first DOMNode::ELEMENT_NODE child. This discards other child nodes
    that are not DOMNode::ELEMENT_NODE(s), i.e. comments, whitespace, etc. -
    even if these are located before the returned child
  */

  DOMNode*    get_first_element_child             (const DOMNode* node) const;

  /// \brief Given a DOMNode::ELEMENT_NODE, retrieve the value of
  /// a named integer attribute.
  /**
    \pre node != NULL
    \pre attr_name != NULL
    \pre node must have an integer attribute named attr_name.

    \param node Node whose integer attribute we are interested in.
    \param attr_name Name of the integer attribute sought.
    \param error Returns 0 if no errors are detected, 1 otherwise.
    \return The integer value of the attr_name attribute.
  */

  int         get_integer_attribute               (const DOMNode* node,
                                                   char* attr_name,
                                                   int* error) const;

  /// \brief Given a DOMNode::ELEMENT_NODE, returns its first sibling.
  /**
    \pre node != NULL
    \param node Node whose first sibling we are interested in.
    \return A pointer to the first DOMNode::ELEMENT_NODE sibling of node or
            NULL if either node was NULL or has no siblings.
  */

  DOMNode*    get_next_sibling_element            (const DOMNode* node) const;

  /// \brief Given a DOMNode::ELEMENT_NODE, returns its unique
  /// child DOMNode::ELEMENT_NODE with a given name.
  /**
    \pre node != NULL
    \pre node has an unique child whose name is son_name.
    \param node Node whose unique child we are interested in.
    \param son_name Name of the son (child) sought.
    \return A pointer to the unique DOMNode::ELEMENT_NODE child of node that
            is named "son_name" or NULL if either node was NULL or has no unique
            childs with the specified name..
  */

  DOMNode*    get_node_unique_son_by_name         (const DOMNode* node,
                                                   const char* son_name) const;

  /// \brief Given a DOMNode::ELEMENT_NODE, returns the number of children
  /// that has a certain name.
  /**
    \pre node != NULL
    \pre node has childs.
    \param node Node whose named children are sought.
    \param son_name Name of the sons (children) sought.
    \param error Set to 0 if no errors are detected, to 1 otherwise.
    \return The total number of children whose name is son_name.
  */

  int         get_number_of_repeated_sons_by_name (const DOMNode* node,
                                                   const char* son_name,
                                                   int* error ) const;

  /// \brief Given a DOMNode::ELEMENT_NODE, returns the value of a named string attribute.
  /**
    \pre node != NULL
    \pre attr_name != NULL
    \pre node has an atribute named attr_name
    \param node Node whose attribute is sought.
    \param attr_name Name of the attribute sought.
    \return The value of the node's attr_name string attribute, or NULL if
            either node is NULL, attr_name is NULL or an attribute named
            attr_name does not exist.
  */

  char*       get_string_attribute                (const DOMNode* node,
                                                   char* attr_name) const;

  /// \brief Given a DOMNode::ELEMENT_NODE, checks if it is "active".
  /**
    \pre node != NULL
    \param node Node to check.
    \return True if the node is active, false otherwise. Note that if node is
            NULL (therefore, violating the precondition) false is returned, since
            a NULL node never may be active.

     Nodes may be "active" or "removed". This is specified by means of
     a special attribute, "s" (for "status") that may take two
     values: "a" (active) and "r" (removed). If the attribute is not
     present, the node is supposed active.
     
     This method checks:

     - If the "s" attribute exists.
     - If it doesn't exist, returns true, since being active is the default.
     - If it exists, checks its value ("a" or "r") and returns true or
       false respectively.
  */

  bool        is_element_active                   (const DOMNode* node) const;

  /// \brief Replace the text value of a given DOMNode.
  /**
    \pre node != NULL
    \pre new_text != NULL
    \param node The DOMNode whose text is about to be changed.
    \param new_text The new text that will be assigned to node
  */

  void        replace_node_text                   (const DOMNode* node,
                                                   const char* new_text);

  /// \brief Given a DOMNode::ELEMENT_NODE, set the value of
  /// a named integer attribute.
  /**
    \pre node != NULL
    \pre attr_name != NULL
    \pre node must have an integer attribute named attr_name.

    \param node Node whose integer attribute we are interested in.
    \param attr_name Name of the integer attribute to change.
    \param attr_value Value to set.
    \throw astrolabe_exception Not enough memory to complete
           the request.
    \return True if the attribute is set. False if any of the preconditions
           is violated.
  */

  bool        set_integer_attribute               (const DOMNode* node,
                                                   char* attr_name,
                                                   int   attr_value);

  /// \brief Given a DOMNode::ELEMENT_NODE, set the value of
  /// a named string attribute.
  /**
    \pre node != NULL
    \pre attr_name != NULL
    \pre attr_value != NULL
    \pre node must have an integer attribute named attr_name.

    \param node Node whose integer attribute we are interested in.
    \param attr_name Name of the integer attribute to change.
    \param attr_value Value to set.
    \return True if the attribute is set. False if any of the preconditions
           is violated.
  */

  bool        set_string_attribute                (const DOMNode* node,
                                                   char* attr_name,
                                                   char* attr_value);

  /// \brief Given a DOMNode::ELEMENT_NODE, returns its text value without
  /// the surrounding whitespace.
  /**
    \pre node != NULL
    \param node The node whose text is about to be trimmed.
    \return The trimmed version (no surrounding whitespace) of node's
            text value, or NULL if node was NULL.
   */

  char*       trim_element_text_contents          (const DOMNode* node) const;
  
};

#endif // __ADP_DOMTREE_UTILITIES_HPP__
