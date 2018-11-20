/** \file gp_plist_item.cpp
  \brief Implementation file for gp_plist_item.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_plist_item.hpp"

string
gp_plist_item::
get_id
(void)
{
  {
    return identifier_;
  }
}

int
gp_plist_item::
get_n
(void)
const
{
  {
    return n_;
  }
}

string
gp_plist_item::
get_role
(void)
{
  {
    return role_;
  }
}

gp_plist_item::
gp_plist_item
(void)
{
  {
    n_ = 0;

    identifier_.clear();
    role_.clear();
  }
}


gp_plist_item::
~gp_plist_item
(void)
{
  {
     // Intentionally left blank.
  }
}

void
gp_plist_item::
set_id 
(string& identifier)
{
  set_precondition (!identifier.empty());

  {
    identifier_ = identifier;
  }
}

void
gp_plist_item::  
set_n
(int n)
{
  set_precondition (n > 0);

  {
    n_ = n;
  }
}

void
gp_plist_item::
set_role
(string& role)
{
  set_precondition ((role == "FREE")     ||
                    (role == "free")     ||
                    (role == "CONSTANT") ||
                    (role == "constant"));

  {

    string tmp;

    //
    // Convert the input role to lowercase (note the use of the
    // std::transform function!!!!).
    //
    
    tmp = role;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);

    // Set the role always in lowercase.
    
    role_ = tmp;
  }
}
