/** \file adp_domtree_utilities.cpp
  \brief Implementation file for adp_domtree_utilities.hpp
  \ingroup adp_DOMparser_group
*/
#include "adp_domtree_utilities.hpp"

//
// Constructor
//

adp_domtree_utilities::
adp_domtree_utilities
(void)
{
  {
    // Intentionally left blank.
  }
}

//
// Destructor.
//

adp_domtree_utilities::
~adp_domtree_utilities
(void)
{
  {
    // Intentionally left blank.
  }
}

//
// Retrieves the name of a non-text node (xml tag name)
//

char*
adp_domtree_utilities::
get_element_name
(const DOMNode* current)
const
{
  char* tmp;
  char* elem_name;
  int   lon, i;

  if (current==NULL) return NULL;
  {
    tmp = XMLString::transcode(current->getNodeName());

    lon = (int)strlen(tmp);

    try
    {
      elem_name = new char[lon+1];
    }
    catch(...)
    {
      // Next sentence is used to simulate the old, wrong behaviour
      elem_name = NULL;
      XMLString::release(&tmp);
      throw astrolabe_exception ();
    }

    for (i=0; i<lon; i++) elem_name[i] = tmp[i];
    elem_name[lon] = '\0';

    XMLString::release(&tmp);

    return elem_name;
  }
}

//
// Get the first non-text child of a given node.
//
// Returns NULL if such node does not exist.
//

DOMNode*
adp_domtree_utilities::
get_first_element_child
(const DOMNode* current)
const
{
  if (current==NULL) return NULL;
  {
    DOMNode* child;
    child = current->getFirstChild();
    if (child == NULL) return child;

    while ((child!=NULL) && (child->getNodeType() != DOMNode::ELEMENT_NODE))
    {
      child = child->getNextSibling();
    }
    return child;
  }
}

//
// Get the integer value of an integer attribute for a
// given DOMNode.
//

int
adp_domtree_utilities::
get_integer_attribute
(const DOMNode* node,
 char* attrName,
 int*  error)
const
{
  if (node==NULL) 
  { // EXCEPTION !!!
    *error = 1;
    return 0;
  }

  if (attrName == NULL)
  { // EXCEPTION !!!
    *error = 1;
    return 0;
  }
  {
    DOMNamedNodeMap *attr;
    DOMAttr         *n;
    XMLCh           *item_name;
    char            *value;
    int              result;

    if(!node->hasAttributes())
    { // EXCEPTION !!!
      *error = 1;
      return 0;
    }
    
    attr      = node->getAttributes();
    item_name = XMLString::transcode(attrName);
    n         = (DOMAttr*)attr->getNamedItem(item_name);
    XMLString::release(&item_name);

    if (n == NULL)
    { // EXCEPTION !!!
      *error = 1;
      return 0;
    }

    value = XMLString::transcode(n->getValue());
    result = atoi(value);
    XMLString::release(&value);

    *error = 0;
    return result;
  }
}


//
// Get the next element sibling for a given element.
//

DOMNode*
adp_domtree_utilities::
get_next_sibling_element
(const DOMNode* currentElement)
const
{
  if (currentElement == NULL) return NULL; // EXCEPTION !!
  {
    DOMNode* sibling;
    sibling = currentElement->getNextSibling();
    while ((sibling!=NULL) && (sibling->getNodeType() != DOMNode::ELEMENT_NODE))
    {
      sibling = sibling->getNextSibling();
    }
    return sibling;
  }
}

//
// Retrieve the node's unique named son.
//

DOMNode*
adp_domtree_utilities::
get_node_unique_son_by_name
(const DOMNode* node,
 const char* sonName)
const
{
  if (node==NULL) return NULL; // EXCEPTION !!!
  {
    DOMNode* son;
    char*    cmp;

    son = get_first_element_child(node);
    if (son==NULL) return NULL; // EXCEPTION !!!

    cmp = get_element_name(son);

    while ((son!=NULL) && (strcmp(cmp,sonName)!=0))
    {
      delete cmp;
      cmp = NULL;

      son = get_next_sibling_element(son);

      if (son != NULL) cmp = get_element_name(son);
    }

    if (cmp != NULL)
    {
      delete cmp;
      cmp = NULL;
    }

    return son;
  }
}

//
// Count the number of repeated sons for a given
// node and son's name.
//

int
adp_domtree_utilities::
get_number_of_repeated_sons_by_name
(const DOMNode* node,
 const char* sonName,
 int* error)
const
{
  if (node==NULL)
  { // EXCEPTION !!!
    *error = 1;
    return 0;
  }

  int   count;
  char* cmp;

  {
    DOMNode* son;
    count = 0;

    son = get_first_element_child(node);
    if (son==NULL)
    { // EXCEPTION !!!
      *error = 1;
      return 0;
    }

    while (son!=NULL)
    {
      cmp = get_element_name(son);

      if (strcmp(cmp,sonName)==0) count++;

      delete cmp;
      cmp = NULL;

      son = get_next_sibling_element(son);
    }
  }
  *error = 0;
  return count;
}

//
// Get the string value of a string attribute for a
// given DOMNode.
//

char*
adp_domtree_utilities::
get_string_attribute
(const DOMNode* node,
 char* attrName)
const
{
  if (node==NULL) return NULL; // EXCEPTION !!!
  if (attrName == NULL) return NULL; // EXCEPTION !!!
  {
    DOMNamedNodeMap *attr;
    DOMAttr         *n;
    XMLCh           *item_name;
    char            *value;
    char            *result;

    if(!node->hasAttributes()) return NULL; // EXCEPTION !!!
    
    attr      = node->getAttributes();
    item_name = XMLString::transcode(attrName);
    n         = (DOMAttr*)attr->getNamedItem(item_name);
    XMLString::release(&item_name);

    if (n == NULL) return NULL; // EXCEPTION !!!

    value = XMLString::transcode(n->getValue());
    result = strdup(value);
    XMLString::release(&value);

    return result;
  }
}

//
// Check if the given element node is active.
//

bool
adp_domtree_utilities::
is_element_active
(const DOMNode* node)
const
{
  if (node==NULL) return false; // EXCEPTION !!!
  {
    DOMNamedNodeMap *attr;
    DOMAttr         *active;
    XMLCh           *item_name;
    char            *value;
    bool             result;

    if(!node->hasAttributes()) return true; // Active by default.
    
    attr   = node->getAttributes();
    item_name = XMLString::transcode("s");
    active = (DOMAttr*)attr->getNamedItem(item_name);
    XMLString::release(&item_name);

    if (active == NULL) return true; // Active by default.

    value = XMLString::transcode(active->getValue());
    result = (strcmp(value,"a") == 0);
    XMLString::release(&value);

    return result;
  }
}

//
// Replace the text value of a given node
//

void
adp_domtree_utilities::
replace_node_text
(const DOMNode* node, 
 const char* new_text)
{
  if (node == NULL) return;
  if (new_text == NULL) return;

  {
    XMLCh* write_to_dom = NULL;

    write_to_dom = XMLString::transcode(new_text);
    (node->getFirstChild())->setNodeValue(write_to_dom);
    XMLString::release(&write_to_dom);
  }
}

//
// Set the integer value of an integer attribute for a
// given DOMNode.
//

bool
adp_domtree_utilities::
set_integer_attribute
(const DOMNode* node,
 char* attrName     ,
 int   attrValue)
{
  if (node==NULL) 
  { // EXCEPTION !!!
    return false;
  }

  if (attrName == NULL)
  { // EXCEPTION !!!
    return false;
  }
  {
    char*  value;
    XMLCh* item_name;
    XMLCh* item_value;
    
    try
    {
      value = new char[11];
    }
    catch(...)
    {
      value = NULL;
      throw astrolabe_exception();
    }

    sprintf(value,"%d",attrValue);

    item_name  = XMLString::transcode(attrName);
    item_value = XMLString::transcode(value);

    ((DOMElement*)node)->setAttribute(item_name, item_value);

    XMLString::release(&item_name);
    XMLString::release(&item_value);
    
    delete value;

    return true;
  }
}

//
// Set the string value of a string attribute for a
// given DOMNode.
//

bool
adp_domtree_utilities::
set_string_attribute
(const DOMNode* node,
 char* attrName     ,
 char* attrValue)
{
  if (node==NULL) 
  { // EXCEPTION !!!
    return false;
  }

  if (attrName == NULL)
  { // EXCEPTION !!!
    return false;
  }

  if (attrValue == NULL)
  { // EXCEPTION !!!
    return false;
  }

  {
    XMLCh *item_name;
    XMLCh *item_value;

    item_name  = XMLString::transcode(attrName);
    item_value = XMLString::transcode(attrValue);
    ((DOMElement*)node)->setAttribute(item_name, item_value);
    XMLString::release(&item_name);
    XMLString::release(&item_value);

    return true;
  }
}

//
// Retrieve the trimmed (no whitespace around) text
// string stored in an element.
//

char*
adp_domtree_utilities::
trim_element_text_contents
(const DOMNode* node)
const
{
  char *trimmedText;

  if (node == NULL) return NULL; // EXCEPTION!!!

  {
    const char* no_text = "";
    char*       text;
    char*       trimText;

    if (NULL == node->getFirstChild ())
    {
      trimText = NULL;
    }
    else
    {
      text = (char*)XMLString::transcode ((node->getFirstChild ())->getNodeValue ());
      trimText = astr_trim_whitespace (text);
      XMLString::release (&text);
    }

    if (trimText == NULL)
    {
      trimmedText = strdup (no_text);
    }
    else
    {
      trimmedText = strdup (trimText);
      free (trimText);
      trimText = NULL;
    }
  }

  return trimmedText;
}
