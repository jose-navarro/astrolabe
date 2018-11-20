#ifndef ASTROLABE_TIME
#define ASTROLABE_TIME

#include <time.h>
#include <math.h>
#include <string.h>

#include "astrolabe_exception.hpp"
#include "astrolabe_precondition_violation.hpp"
#include "astrolabe_postcondition_violation.hpp"
#include "astrolabe_contracts.hpp"

#include <iostream>
#include <fstream>

using namespace std;

// valid time:
//
// year   : any non-zero integer
// month  : any integer in the range [1,12]
// day    : any integer in the range [1,x] where
//          x = month__get_no_of_days (month,year)
// hour   : any integer in the range [0,23]
// minute : any integer in the range [0,59]
// second : any real in the range    [0,60[
//
// also valid times in the UTC system: (in prepar.)
//
// 1997.06.30 23:59:s where s is in  [0,61[
// and all the coming times when a leap second is
// introduced to keep UTC close to TAI.

// remarks:
// a negative integer year means B.C.

class astrolabe_time
{
  public:

    astrolabe_time (void);

    /**
      \throw astrolabe_precondition_violation
    **/
    astrolabe_time (int y, int m, int day_in_month,
                    int h, int i, long double s);

   ~astrolabe_time (void);

  public:

    astrolabe_time & operator =              (const astrolabe_time & other);

    /**
      \throw astrolabe_precondition_violation
    */

    void             all_int                 (int * result) const;

    /**
      \throw astrolabe_postcondition_violation
    **/

    int              year                    (void) const;

    /**
      \throw astrolabe_postcondition_violation
    **/

    int              month                   (void) const;

    int              day                     (void) const;

    /**
      \throw astrolabe_postcondition_violation
    **/

    int              day_in_week_0_6         (void) const;

    /**
      \throw astrolabe_postcondition_violation
    **/

    int              day_in_week_1_7         (void) const;

    /**
      \throw astrolabe_postcondition_violation
    **/

    int              day_in_year             (void) const;

    int              hour                    (void) const;

    int              minute                  (void) const;

    long double      second                  (void) const;

    long double      second_in_day           (void) const;

    long double      second_in_week          (void) const;

    /**
      \throw astrolabe_postcondition_violation
    **/

    int              week_in_year            (void) const;

    long double      julian_date             (void) const;

    long int         julian_day_no           (void) const;

    long double      modified_julian_date    (void) const;

    long int         modified_julian_day_no  (void) const;

    void             reset                   (void);

    /**
      \throw astrolabe_precondition_violation
    **/

    void             set                     (int year, int month, int day_in_month,
                                              int hour, int minute, long double second);

    /**
      \throw astrolabe_precondition_violation
    **/

    void             set_from_string         (const char * str);

    /**
      \throw astrolabe_precondition_violation
    **/

    void             set_hour                (int hour);

    /**
      \throw astrolabe_precondition_violation
    **/

    void             set_minute              (int minute);

    /**
      \throw astrolabe_precondition_violation
    **/

    void             set_second              (long double second);

    /**
      \throw astrolabe_precondition_violation
    **/

    void             set_second_in_week      (long double second);

    /**
      \throw astrolabe_precondition_violation
    **/

    void             set_to_day_in_year      (int year, int day_in_year,
                                              int hour, int minute, long double second);

    void             set_to_now              (void);


  public: // copy tools


    void             copy_to                 (astrolabe_time & result) const;

  public: // time algebra

    void             add_week                (int n_weeks);

    void             add_day                 (int n_days);

    void             add_hour                (int n_hours);

    void             add_minute              (int n_minutes);

    void             add_second              (long double n_seconds);

    void             average_to              (const astrolabe_time & other,
                                                    astrolabe_time & result) const;

    long double      difference_day          (const astrolabe_time & other) const;

    long double      difference_hour         (const astrolabe_time & other) const;

    long double      difference_minute       (const astrolabe_time & other) const;

    long double      difference_second       (const astrolabe_time & other) const;

    long double      difference_week         (const astrolabe_time & other) const;

  public: // order operators

    bool             is_eq                   (const astrolabe_time & other) const;

    /**
      \throw astrolabe_precondition_violation
    **/

    bool             is_eq                   (const astrolabe_time & other,
                                              float tolerance_second) const;

    bool             is_ne                   (const astrolabe_time & other) const;

    bool             is_ne                   (const astrolabe_time & other,
                                              float tolerance_second) const;

    bool             is_le                   (const astrolabe_time & other) const;

    bool             is_ge                   (const astrolabe_time & other) const;

    bool             is_lt                   (const astrolabe_time & other) const;

    bool             is_gt                   (const astrolabe_time & other) const;

    /**
      \throw astrolabe_precondition_violation
    **/

    bool             is_gt                   (const astrolabe_time & other,
                                              float tolerance_second) const;

    bool             is_same_civilian_day    (const astrolabe_time & other) const;

  public: // properties


    bool             is_day_oclock           (float tolerance_second) const;

    bool             is_hour_oclock          (float tolerance_second) const;

    bool             is_minute_oclock        (float tolerance_second) const;

    bool             is_second_oclock        (float tolerance_second) const;

    bool             is_valid                (void) const;


  public: // IO tools

    /**
      \throw astrolabe_precondition_violation
    **/

    void             fprint                  (FILE * f) const;

    /**
      \throw astrolabe_precondition_violation
    **/

    void             fprint_all_int          (FILE * f) const;

    void             fprint_now_all_int      (FILE * f) const;

    void             print                   (void) const;

    void             print_all_int           (void) const;

    void             print_now_all_int       (void) const;

  private:

    /**
      \throw astrolabe_precondition_violation
   **/
    long             julday_g                (int the_year,
                                              int the_month,
                                              int the_day) const;

  private:
    int               yy;
    int               mm;
    int               dd;
    int               ho;
    int               mi;
    long double       se;
};

bool      operator == (const astrolabe_time & t1, const astrolabe_time & t2);

bool      operator != (const astrolabe_time & t1, const astrolabe_time & t2);

bool      operator <= (const astrolabe_time & t1, const astrolabe_time & t2);

bool      operator >= (const astrolabe_time & t1, const astrolabe_time & t2);

bool      operator >  (const astrolabe_time & t1, const astrolabe_time & t2);

bool      operator <  (const astrolabe_time & t1, const astrolabe_time & t2);

/**
   \throw astrolabe_exception
**/
istream & operator >> (istream & s,      astrolabe_time & result);

ostream & operator << (ostream & result, const astrolabe_time & the_time);

extern "C"
{
  /**
    \throw astrolabe_precondition_violation
  **/
  int  month__get_no_of_days        (int month, int year) noexcept(false);

  /**
    \throw astrolabe_precondition_violation
  **/
  int  year__get_no_of_days         (int year) noexcept(false);

  bool hour_minute_second__is_valid (int hour, int minute, long double second);

  bool month__is_valid              (int month);

  bool second_in_week__is_valid     (long double second);

  bool string__is_valid_time        (const char * str);

  /**
    \throw astrolabe_precondition_violation
  **/
  bool year__is_leap                (int year) noexcept(false);

  bool year__is_valid               (int year);

  /**
    \throw astrolabe_precondition_violation
  **/
  bool year__is_valid_day           (int year, int day_in_year) noexcept(false);

  /**
    \throw astrolabe_precondition_violation
  **/
  bool year__is_valid_month_and_day (int year, int month, int day_in_month) noexcept(false);
}

#endif
