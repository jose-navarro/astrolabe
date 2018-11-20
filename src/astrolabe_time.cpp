#include "astrolabe_time.hpp"

astrolabe_time::
astrolabe_time
(void)
{
  {
    set_to_now();
  }
}

astrolabe_time::
astrolabe_time
(int y, int m, int day_in_month,
 int h, int i, long double s)
{
  SET_PRECONDITION(year__is_valid(y));
  SET_PRECONDITION(year__is_valid_month_and_day (y,m,day_in_month));
  SET_PRECONDITION(hour_minute_second__is_valid(h,i,s));
  {
    set(y,m,day_in_month,h,i,s);
  }
}

astrolabe_time::
~astrolabe_time
(void)
{
  {
    // intentionally left blank
  }
}

astrolabe_time &
astrolabe_time::
operator =
(const astrolabe_time & other)
{
  {
    set(other.yy,other.mm,other.dd,other.ho,other.mi,other.se);
  }
  set_postcondition(is_eq(other));
  return *this;
}

istream &
operator >>
(istream & s, astrolabe_time & result)
{
  {
    int year,month,day,hour,minute;
    long double second;
    char c1 = 0;
    char c2 = 0;
    char c3 = 0;
    char c4 = 0;

    try
    {
      s >> year >> c1 >> month  >> c2 >> day
        >> hour >> c3 >> minute >> c4 >> second;
      if (c1=='.' && c2=='.' && c3==':' && c4==':' &&
          year__is_valid_month_and_day (year,month,day) &&
          hour_minute_second__is_valid (hour,minute,second))
        result = astrolabe_time(year,month,day,hour,minute,second);
      else
        throw astrolabe_exception();
    }
    catch (...)
    {
      throw astrolabe_exception();
    }
  }
  return s;
}

ostream &
operator <<
(ostream & result, const astrolabe_time & the_time)
{
  {

    if (the_time.month() < 10)
      result << the_time.year()   << "-0";
    else
      result << the_time.year()   << "-";

    if (the_time.day() < 10)
      result << the_time.month()  << "-0";
    else
      result << the_time.month()  << "-";

    if (the_time.hour() < 10)
      result << the_time.day()    << "T0";
    else
      result << the_time.day()    << "T";

    if (the_time.minute() < 10)
      result << the_time.hour()   << ":0";
    else
      result << the_time.hour()   << ":";

    if (the_time.second() < 10.0l)
      result << the_time.minute() << ":0" << the_time.second();
    else
      result << the_time.minute() << ":"  << the_time.second();
  }
  return result;
}

void
astrolabe_time::
add_week
(int n_weeks)
{
  {
    add_day(n_weeks*7);
  }
}

void
astrolabe_time::
add_day
(int n_days)
{
  {
    int y,n,h,m;
    long double s;

    y = year();
    h = hour();
    m = minute();
    s = second();
    n = day_in_year() + n_days;

    if (n > 0)
    {
      if (n <= year__get_no_of_days(y))
        set_to_day_in_year(y,n,h,m,s);
      else
      {
        while (n > year__get_no_of_days(y))
        {
          n = n - year__get_no_of_days(y);
	      y = y + 1;
          if (y==0) y = 1;
        }
        set_to_day_in_year(y,n,h,m,s);
      }
    }
    else if (n == 0)
    {
      y = y-1;
      if (y==0) y=-1;
      n = year__get_no_of_days(y);
      set_to_day_in_year(y,n,h,m,s);
    }
    else
    {
      while (n < 0)
      {
	       y = y - 1;
        if (y==0) y =-1;
        n = n + year__get_no_of_days(y);
        if (n==0)
        {
          y = y-1;
          if (y==0) y=-1;
          n = year__get_no_of_days(y);
        }
      }
      set_to_day_in_year(y,n,h,m,s);
    }
  }
}

void
astrolabe_time::
add_hour
(int n_hours)
{
  {
    int h_total;
    int h_new;
    int carry_days;
    h_total = hour() + n_hours;
    if (h_total >= 24)
    {
      h_new = h_total % 24;
      carry_days = h_total / 24;
      set_hour(h_new);
      add_day(carry_days);
    }
    else if (h_total < 0)
    {
      h_new = -((- h_total) % 24);
      carry_days = -((- h_total) / 24);
      if(h_new < 0)
      {
        h_new = h_new + 24;
        carry_days = carry_days - 1;
      }
      set_hour(h_new);
      add_day(carry_days);
    }
    else
      set_hour(h_total);
  }
}

void
astrolabe_time::
add_minute
(int n_minutes)
{
  {
    int m_total;
    int m_new;
    int carry_hours;
    m_total = minute() + n_minutes;
    if (m_total >= 60)
    {
      m_new = m_total % 60;
      carry_hours = m_total / 60;
      set_minute(m_new);
      add_hour(carry_hours);
    }
    else if (m_total < 0)
    {
      m_new = -((- m_total) % 60);
      carry_hours = -((- m_total) / 60);
      if(m_new < 0)
      {
        m_new = m_new + 60;
  	     carry_hours = carry_hours - 1;
      }
      set_minute(m_new);
      add_hour(carry_hours);
    }
    else
      set_minute(m_total);
  }
}

void
astrolabe_time::
add_second
(long double n_seconds)
{
  {
    long double s_total;
    long double s_new;
    long double aux;
    int  carry_minutes;

    s_total = second() + n_seconds;
    if (s_total >= 60.l)
    {
      aux = s_total / 60.l;
      carry_minutes = static_cast<int>(floor(aux));
      s_new = fmod(static_cast<double>(s_total),60.l);
      if(s_new < 0l)
        s_new = 0;
      set_second(s_new);
      add_minute(carry_minutes);
    }
    else if (s_total <= 0.l)
    {
      aux = s_total / 60.l;
      carry_minutes = static_cast<int>(floor(aux));
      s_new = s_total - static_cast<long double>(carry_minutes*60);
      if(s_new < 0.l)
        s_new = 0;
      set_second(s_new);
      add_minute(carry_minutes);
    }
    else
      set_second(s_total);
  }
}

void
astrolabe_time::
average_to
(const astrolabe_time & other, astrolabe_time & result)
const
{
  {
    long double s;

    if ( (*this) == other)
    {
      copy_to(result);
      return;
    }
    else if((*this) < other)
    {
      copy_to(result);
    }
    else if((*this) > other)
    {
      other.copy_to(result);
    }

    s = difference_second(other);
    s = fabs(s) * static_cast<long double>(0.5e0);

    result.add_second(s);
  }
}

void
astrolabe_time::
copy_to
(astrolabe_time & result)
const
{
  {
    result.set(yy,mm,dd,ho,mi,se);
  }
}

void
astrolabe_time::
all_int
(int * result)
const
{
  SET_PRECONDITION(result != nullptr);
  {
    result[1] = yy;
    result[2] = mm;
    result[3] = dd;
    result[4] = ho;
    result[5] = mi;
    result[6] = static_cast<int>(se);
  }
}

int
astrolabe_time::
day
(void)
const
{
  int result;
  {
    result = dd;
  }
  return result;
}

int
astrolabe_time::
day_in_week_0_6
(void)
const
{
  int result;
  // Sunday ... 0
  // Monday ... 1
  // ...
  {
    result = static_cast<int>(((floor)(julian_date() + 1.5l))) % 7;
  }
  SET_POSTCONDITION(0 <= result && result <= 6);
  return result;
}

int
astrolabe_time::
day_in_week_1_7
(void)
const
{
  int result;
  // Sunday ... 1
  // Monday ... 2
  // ...
  {
    result = day_in_week_0_6() + 1;
  }
  SET_POSTCONDITION(1 <= result && result <= 7);
  return result;
}

int
astrolabe_time::
day_in_year
(void)
const
{
  int result;
  {
    int y;
    int m;
    int i_month;
    y = year();
    m = month();
    result = 0;
    for (i_month=1;i_month<m;i_month++)
    {
      result = result + month__get_no_of_days (i_month,y);
    }
    result = result + day();
  }
  SET_POSTCONDITION(result>0);
  return result;
}

long double
astrolabe_time::
difference_day
(const astrolabe_time & other)
const
{
  long double result;
  {
    result = difference_second(other);
    result = result / (3600.0l * 24);
  }
  return result;
}

long double
astrolabe_time::
difference_hour
(const astrolabe_time & other)
const
{
  long double result;
  {
    result = difference_second(other);
    result = result / 3600.0l;
  }
  return result;
}

long double
astrolabe_time::
difference_minute
(const astrolabe_time & other)
const
{
  long double result;
  {
    result = difference_second(other);
    result = result / 60.0l;
  }
  return result;
}

long double
astrolabe_time::
difference_second
(const astrolabe_time & other)
const
{
  long double result;
  {
    long int i;

    if (is_same_civilian_day(other))
      i = 0;
    else
      i = modified_julian_day_no() -
          other.modified_julian_day_no();

    i = i * 24;
    i = i + hour() - other.hour();
    i = i * 60;
    i = i + minute() - other.minute();
    i = i * 60;
    result = second() - other.second();
    result = result + static_cast<long double>(i);
  }
  return result;
}

long double
astrolabe_time::
difference_week
(const astrolabe_time & other)
const
{
  long double result;
  {
    result = difference_second(other);
    result = result / (3600.0l * 24 *7);
  }
  return result;
}

int
astrolabe_time::
year
(void)
const
{
  int result;
  {
    result = yy;
  }
  SET_POSTCONDITION(result != 0);
  return result;
}

int
astrolabe_time::
month
(void)
const
{
  int result;
  {
    result = mm;
  }
  SET_POSTCONDITION(result>0);
  return result;
}

void
astrolabe_time::
fprint
(FILE * f)
const
{
  SET_PRECONDITION(f != nullptr);
  {

    fprintf(f," %i.%02i.%02i %2i:%02i:%05.2Lf",
      year(),
      month(),
      day(),
      hour(),
      minute(),
      second());
  }
}

void
astrolabe_time::
fprint_all_int
(FILE* f)
const
{
  SET_PRECONDITION(f != nullptr);
  {
    int    se;

    se = static_cast<int>(second());
    if (se==60) se = 59;
    fprintf(f," %6i.%02i.%02i %2i:%02i:%02i",
      year(),
      month(),
      day(),
      hour(),
      minute(),
      se);
  }
}

void
astrolabe_time::
fprint_now_all_int
(FILE* f)
const
{
  set_precondition(f != nullptr);
  {
    astrolabe_time n;
    int   se;

    n.set_to_now();
    se = static_cast<int>(n.second());
    if (se==60) se = 59;
    fprintf(f," %6i.%02i.%02i %2i:%02i:%02i",
      n.year(),
      n.month(),
      n.day(),
      n.hour(),
      n.minute(),
      se);
  }
}

int
astrolabe_time::
hour
(void)
const
{
  int result;
  {
    result = ho;
  }
  set_postcondition(result>=0);
  return result;
}

int
astrolabe_time::
minute
(void)
const
{
  int result;
  {
     result = mi;
  }
  set_postcondition(result>=0);
  return result;
}

long double
astrolabe_time::
second
(void)
const
{
  long double result;
  {
    result = se;
  }
  set_postcondition(result>= 0.0l);
  return result;
}

int
astrolabe_time::
week_in_year
(void)
const
{
  int result;
  {
    result = 0;
    printf(" \n subroutine not available: week_in_year");
  }
  SET_POSTCONDITION(result>0);
  return result;
}

long double
astrolabe_time::
julian_date
(void)
const
{
  long double result;
  {
    //
    // the Julian Day Number is a count of days elapsed since Greenwich mean
    // noon on 1 January 4713 B.C., Julian proleptic calendar. The Julian Date
    // is, then, the Julian day number followed by the fraction of the day
    // elapsed since the preceding noon.
    //
    // Julian dates can be expressed in UT, TAI, TDT, etc. and so for precise
    // applications the timescale should be specified, e.g. MJD 49135.3824 TAI.
    //
    int       i;
    long      j;

    j = julday_g(yy,mm,dd);

    i = hour();
    i = i * 60;
    i = i + minute();
    i = i * 60;
    result = second() + static_cast<long double>(i);
    result = result / (24*60*60);
    result = result - static_cast<long double>(0.5);
    result = result + static_cast<long double>(j);
  }
  return result;
}

long int
astrolabe_time::
julian_day_no
(void)
const
{
  long int result;
  {
    //
    // the Julian Day Number is a count of days elapsed since Greenwich mean
    // noon on 1 January 4713 B.C., Julian proleptic calendar. The Julian Date
    // is, then, the Julian day number followed by the fraction of the day
    // elapsed since the preceding noon.
    //
    result = static_cast<long>(floor(julian_date()));
  }
  return result;
}

long double
astrolabe_time::
modified_julian_date
(void)
const
{
  long double result;
  {
    //
    // The Modified Julian Date (MJD) is defined as MJD = JD - 2400000.5.
    // An MJD day thus begins at midnight, civil date.
    //
    result = julian_date() - static_cast<long double>(2400000.5);
  }
  return result;
}

long int
astrolabe_time::
modified_julian_day_no
(void)
const
{
  long int result;
  {
    result = static_cast<long>(floor(modified_julian_date()));
  }
  return result;
}

bool
astrolabe_time::
is_day_oclock
(float tol_second)
const
{
  bool result;
  {
    int         h;
    int         m;
    long double rounded_second;
    long double s;

    h = hour();
    m = minute();
    s = second();
    rounded_second = static_cast<long double>(static_cast<int>(s));
    if (fabs(rounded_second) <= static_cast<long double>(tol_second)  &&
        m == 0  &&  h == 0)
      result = true;
    else
      result = false;
  }
  return result;
}

bool
astrolabe_time::
is_eq
(const astrolabe_time & other)
const
{
  bool result;
  {
    result = false;
    if (year() == other.year())
      if (month() == other.month())
        if (day() == other.day())
          if (hour() == other.hour())
            if (minute() == other.minute())
              if (second() == other.second())
                result = true;
  }
  return result;
}

bool
astrolabe_time::
is_eq
(const astrolabe_time & other, float tolerance_in_seconds)
const
{
  bool result;
  SET_PRECONDITION(tolerance_in_seconds >= 0.0f);
  {
    if (fabs(difference_second(other)) <= fabs(static_cast<long double>(tolerance_in_seconds)))
      result = true;
    else
      result = false;
  }
  return result;
}

bool
astrolabe_time::
is_ge
(const astrolabe_time & other)
const
{
  bool result;
  {
    if (!other.is_gt(*this))
      result = true;
    else
      result = false;
  }
  return result;
}

bool
astrolabe_time::
is_gt
(const astrolabe_time & other)
const
{
  bool result;
  {
    result = false;
    {
      int s_y,o_y;
      s_y = year();
      o_y = other.year();
      if (s_y > o_y)
        result = true;
      else if (s_y == o_y)
      {
        int s_m,o_m;
        s_m = month();
        o_m = other.month();
        if (s_m > o_m)
          result = true;
        else if (s_m == o_m)
        {
          int s_d,o_d;
          s_d = day();
          o_d = other.day();
          if (s_d > o_d)
            result = true;
          else if (s_d == o_d)
          {
            int s_h,o_h;
            s_h = hour();
            o_h = other.hour();
            if (s_h > o_h)
              result = true;
            else if (s_h == o_h)
            {
              int s_mi,o_mi;
              s_mi = minute();
              o_mi = other.minute();
              if (s_mi > o_mi)
                result = true;
              else if (s_mi == o_mi)
              {
                if (second() > other.second())
                  result = true;
              }
            }
          }
        }
      }
    }
  }
  return result;
}

bool
astrolabe_time::
is_gt
(const astrolabe_time & other, float tolerance_second)
const
{
  bool result;
  SET_PRECONDITION(static_cast<long double>(tolerance_second) >= 0.0l);
  {
    long double diff;
    result = is_gt(other);
    diff = difference_second(other);
    if (result == true)
    {
      if (fabs(diff) <= fabs(static_cast<long double>(tolerance_second))) result = false;
    }
  }
  return result;
}

bool
astrolabe_time::
is_hour_oclock
(float tol_second)
const
{
  bool result;
  {
    int         m;
    long double rounded_second;
    long double s;

    m = minute();
    s = second();
    rounded_second = static_cast<long double>(static_cast<int>(s));
    if (fabs(rounded_second) <= static_cast<long double>(tol_second)  &&  m == 0)
      result = true;
    else
      result = false;
  }
  return result;
}

bool
astrolabe_time::
is_le
(const astrolabe_time & other)
const
{
  bool result;
  {
    if (!other.is_lt(*this))
      result = true;
    else
      result = false;
  }
  return result;
}

bool
astrolabe_time::
is_lt
(const astrolabe_time & other)
const
{
  bool result;
  {
    result = false;
    {
      int s_y,o_y;
      s_y = year();
      o_y = other.year();
      if (s_y < o_y)
        result = true;
      else if (s_y == o_y)
      {
        int s_m,o_m;
        s_m = month();
        o_m = other.month();
        if (s_m < o_m)
          result = true;
        else if (s_m == o_m)
        {
          int s_d,o_d;
          s_d = day();
          o_d = other.day();
          if (s_d < o_d)
            result = false;
          else if (s_d == o_d)
          {
            int s_h,o_h;
            s_h = hour();
            o_h = other.hour();
            if (s_h < o_h)
              result = true;
            else if (s_h == o_h)
            {
              int s_mi,o_mi;
              s_mi = minute();
              o_mi = other.minute();
              if (s_mi < o_mi)
                result = true;
              else if (s_mi == o_mi)
              {
                if (second() < other.second())
                  result = true;
              }
            }
          }
        }
      }
    }
  }
  return result;
}

bool
astrolabe_time::
is_minute_oclock
(float tol_second)
const
{
  bool result;
  {
    long double rounded_second;
    long double s;

    s              = second();
    rounded_second = static_cast<long double>(static_cast<int>(s));
    if (fabs(rounded_second) <= static_cast<long double>(tol_second))
      result = true;
    else
      result = false;
  }
  return result;
}

bool
astrolabe_time::
is_ne
(const astrolabe_time & other)
const
{
  bool result;
  {
    result = true;
    if (year() == other.year())
      if (month() == other.month())
        if (day() == other.day())
          if (hour() == other.hour())
            if (minute() == other.minute())
              if (second() == other.second())
                result = false;
  }
  return result;
}

bool
operator ==
(const astrolabe_time & t1, const astrolabe_time & t2)
{
  bool result;
  {
    result = t1.is_eq(t2);
  }
  return result;
}

bool
operator !=
(const astrolabe_time & t1, const astrolabe_time & t2)
{
  bool result;
  {
    result = t1.is_ne(t2);
  }
  return result;
}

bool
operator <=
(const astrolabe_time & t1, const astrolabe_time & t2)
{
  bool result;
  {
    result = t1.is_le(t2);
  }
  return result;
}

bool
operator >=
(const astrolabe_time & t1, const astrolabe_time & t2)
{
  bool result;
  {
    result = t1.is_ge(t2);
  }
  return result;
}

bool
operator <
(const astrolabe_time & t1, const astrolabe_time & t2)
{
  bool result;
  {
    result = t1.is_lt(t2);
  }
  return result;
}

bool
operator >
(const astrolabe_time & t1, const astrolabe_time & t2)
{
  bool result;
  {
    result = t1.is_gt(t2);
  }
  return result;
}

bool
astrolabe_time::
is_same_civilian_day
(const astrolabe_time & other)
const
{
  bool result;
  {
    if ( (yy == other.year()) && (mm == other.month()) && (dd == other.day()) )
      result = true;
    else
      result = false;
  }
  return result;
}

bool
astrolabe_time::
is_second_oclock
(float tol_second)
const
{
  bool result;
  {
    long double rounded_second;
    long double s;

    s              = second();
    rounded_second = static_cast<long double>(static_cast<int>(s));
    if (fabs(s-rounded_second) <= static_cast<long double>(tol_second))
      result = true;
    else
      result = false;
  }
  return result;
}

bool
astrolabe_time::
is_valid
(void)
const
{
  bool result;
  {
    result = true;
    result = result && (yy!=0);
    result = result && year__is_valid_month_and_day(yy,mm,dd);
    result = result && hour_minute_second__is_valid(ho,mi,se);
  }
  return result;
}

long
astrolabe_time::
julday_g
(int the_year, int the_month, int the_day)
const
{
  long result;
  SET_PRECONDITION(the_year != 0);
  {
    #define IGREG (15+31L*(10+12L*1582))

    int ja,jy,jm;

    jy = the_year;
    if (jy < 0) ++jy;
    if (the_month > 2)
    {
      jm = the_month + 1;
    }
    else
    {
      --jy;
      jm = the_month + 13;
    }
    result = static_cast<long>(floor(365.25*jy)+floor(30.6001*jm)+the_day+1720995);
    if (the_day + 31L*(the_month+12L*the_year) >= IGREG)
    {
      ja = static_cast<int>(0.01*jy);
      result += 2-ja+static_cast<int>(0.25*ja);
    }

    #undef IGREG
  }
  return result;
}

void
astrolabe_time::
print
(void)
const
{
  {
    printf(" %i.%02i.%02i %2i:%02i:%05.2Lf",
      year(),
      month(),
      day(),
      hour(),
      minute(),
      second());
  }
}

void
astrolabe_time::
print_all_int
(void)
const
{
  {
    int    se;

    se = static_cast<int>(second());
    if (se==60) se = 59;
    printf(" %6i.%02i.%02i %2i:%02i:%02i",
      year(),
      month(),
      day(),
      hour(),
      minute(),
      se);
  }
}

void
astrolabe_time::
print_now_all_int
(void)
const
{
  {
    astrolabe_time n;
    int   se;

    n.set_to_now();
    se = static_cast<int>(n.second());
    if (se==60) se = 59;
    printf(" %6i.%02i.%02i %2i:%02i:%02i",
      n.year(),
      n.month(),
      n.day(),
      n.hour(),
      n.minute(),
      se);
  }
}

void
astrolabe_time::
reset
(void)
{
  {
    set_to_now();
  }
}

long double
astrolabe_time::
second_in_day
(void)
const
{
  long double result;
  {
    result = static_cast<long double>((ho * 60 + mi) * 60) + se;
  }
  set_postcondition(0.0l<=result && result<24.0l*60.0l*60.0l);
  return result;
}

long double
astrolabe_time::
second_in_week
(void)
const
{
  long double result;
  {
    result = static_cast<long double>(day_in_week_0_6() * 24 * 3600)
           + second_in_day();
  }
  set_postcondition(0<=result && result<7*24*60*60);
  return result;
}

void
astrolabe_time::
set
(int y, int m, int day_in_month,
 int h, int i, long double s)
{
  SET_PRECONDITION(year__is_valid(y));
  SET_PRECONDITION(year__is_valid_month_and_day (y,m,day_in_month));
  SET_PRECONDITION(hour_minute_second__is_valid(h,i,s));
  {
    yy=y;
    mm=m;
    dd=day_in_month;
    ho=h;
    mi=i;
    se=s;
  }
}

void
astrolabe_time::
set_from_string
(const char* str)
{
  SET_PRECONDITION(str != nullptr);
  SET_PRECONDITION(string__is_valid_time(str));
  {
    int         y,m,d;
    int         hor,min;
    long double sec;
    char*       prst;
    char*       ptst;


    y    = strtol(str,&prst,10);
    ptst = strchr(prst,'.');
    ptst = ptst + 1;
    m    = strtol(ptst,&prst,10);
    ptst = strchr(prst,'.');
    ptst = ptst + 1;
    d    = strtol(ptst,&prst,10);
    ptst = prst;
    hor  = strtol(ptst,&prst,10);
    ptst = strchr(prst,':');
    ptst = ptst + 1;
    min  = strtol(ptst,&prst,10);
    ptst = strchr(prst,':');
    ptst = ptst + 1;
    sec  = static_cast<long double>(strtod(ptst,&prst));
    set(y,m,d,hor,min,sec);
  }
}

void
astrolabe_time::
set_hour
(int h)
{
  SET_PRECONDITION(0 <= h && h < 24);
  {
    ho = h;
  }
}

void
astrolabe_time::
set_minute
(int m)
{
  SET_PRECONDITION(0 <= m && m < 60);
  {
    mi = m;
  }
}

void
astrolabe_time::
set_second
(long double s)
{
  SET_PRECONDITION(0.0l <= s && s < 60.0l);
  {
    se = s;
  }
}

void
astrolabe_time::
set_second_in_week
(long double s)
{
  SET_PRECONDITION(0.0l <= s && s < 7.0l * 24.0l * 60.0l * 60.0l);
  {
    long double aux;

    aux = second_in_week();
    add_second(s-aux);
  }
}

void
astrolabe_time::
set_to_day_in_year
(int year, int day_in_year,
 int hour, int minute, long double second)
{
  SET_PRECONDITION(year__is_valid(year));
  SET_PRECONDITION(year__is_valid_day(year,day_in_year));
  SET_PRECONDITION(hour_minute_second__is_valid(hour,minute,second));
  {
    int month = 1;
    int day_in_month = day_in_year;

    while (day_in_month > month__get_no_of_days (month,year))
    {
      day_in_month = day_in_month - month__get_no_of_days (month,year);
      month = month +1;
    }
    set (year, month, day_in_month, hour, minute, second);
  }
}

void
astrolabe_time::
set_to_now
(void)
{
  {
    time_t * tp;
    int y,m,d;
    int hour,minute;
    long double second;

    tp = nullptr;
	try
	{
      tp = new time_t[1];
	}
	catch(...)
	{
    tp = nullptr;
	}
    if (tp != nullptr)
    {
      time(tp);
      y = localtime(tp)->tm_year + 1900;
      m = localtime(tp)->tm_mon + 1;
      d = localtime(tp)->tm_mday;
      hour   = localtime(tp)->tm_hour;
      minute = localtime(tp)->tm_min;
      second = static_cast<long double>(localtime(tp)->tm_sec);
      if (second >= 60.0l)
      {
        // ANSI C specification states that tm_sec
        // ranges between 0 and 61
        set(y,m,d,hour,minute,0.0l);
        add_second(second);
      }
      else
      {
        set(y,m,d,hour,minute,second);
      }
      delete [] tp;
      //delete tp;
      tp = nullptr;
    }
    else
      reset();
  }
}

bool
hour_minute_second__is_valid
(int hour, int minute, long double second)
{
  bool result;
  {
    // valid hour, minute and second:
    // hour in [0,23], minute in [0,59] and second in [0,60[

    result = true;
    result = result && ( 0    <= hour   && hour   < 24 );
    result = result && ( 0    <= minute && minute < 60 );
    result = result && ( 0.0l <= second && second < 60.0l);
  }
  return result;
}

int
month__get_no_of_days
(int self, int year)
{
  int result;
  SET_PRECONDITION(month__is_valid(self));
  SET_PRECONDITION(year__is_valid(year));
  {
    if (self==1 || self==3 || self==5  ||
        self==7 || self==8 || self==10 || self==12)
    {
      result = 31;
    }
    else if (self==2)
    {
      if (year__is_leap(year))
        result = 29;
      else
        result = 28;
    }
    else
    {
      result = 30;
    }
  }
  return result;
}

bool
month__is_valid
(int self)
{
  bool result;
  {
    if (1 <= self  &&  self <= 12)
      result = true;
    else
      result = false;
  }
  return result;
}

bool
second_in_week__is_valid
(long double second)
{
  bool result;
  {
    if (0.0l <= second && second < static_cast<long double>(7*24*3600))
      result = true;
    else
      result = false;
  }
  return result;
}

int
year__get_no_of_days
(int year)
{
  int result;
  SET_PRECONDITION(year__is_valid(year));
  {
    if (year__is_leap(year))
      result = 366;
    else
      result = 365;
  }
  set_postcondition(result==365 || result==366);
  return result;
}

bool
year__is_leap
(int year)
{
  bool result;
  SET_PRECONDITION(year__is_valid(year));
  // year is leap if it is divisible by 4 but not by 100
  // except that years divisible by 400 are leap years
  {
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
      result =true;
    else
      result = false;
  }
  return result;
}

bool
year__is_valid
(int year)
{
  bool result;
  {
    if (year != 0)
      result = true;
    else
      result = false;
  }
  return result;
}

bool
year__is_valid_day
(int year,int day_in_year)
{
  bool result;
  SET_PRECONDITION(year__is_valid(year));
  {
    int d_max;
    if (year__is_leap(year))
      d_max = 366;
    else
      d_max = 365;
    if (1 <= day_in_year && day_in_year <= d_max)
      result = true;
    else
      result = false;
  }
  return result;
}

bool
year__is_valid_month_and_day
(int year, int month, int day_in_month)
{
  bool result;
  SET_PRECONDITION(year__is_valid(year));
  {
    if (month__is_valid(month))
    {
      if (1<= day_in_month &&
          day_in_month <= month__get_no_of_days(month,year))
        result = true;
      else
        result = false;
    }
    else
    {
      result = false;
    }
  }
  return result;
}

bool
string__is_valid_time
(const char * str)
{
  bool result;
  {
    int         y,m,d;
    int         ho,mi;
    long double se;
    char*       prst;
    char*       ptst;

    // valid string: y . m . d  ho : mi : se
    // y  : any non-zero integer
    // m  : any integer in the range [1,12]
    // d  : any integer in the range [1,x] where
    //      x = month__get_no_of_days (m,y)
    // ho : any integer in the range [0,23]
    // mi : any integer in the range [0,59]
    // se : any real in the range    [0,60[

    if (str != nullptr)
    {
      y    = strtol(str,&prst,10);
      ptst = strchr(prst,'.');
      ptst = ptst + 1;
      if (year__is_valid(y))
      {
        m    = strtol(ptst,&prst,10);
        ptst = strchr(prst,'.');
        ptst = ptst + 1;
        d    = strtol(ptst,&prst,10);
        ptst = prst;
        if (year__is_valid_month_and_day (y,m,d))
        {
          ho   = strtol(ptst,&prst,10);
          ptst = strchr(prst,':');
          ptst = ptst + 1;
          mi   = strtol(ptst,&prst,10);
          ptst = strchr(prst,':');
          ptst = ptst + 1;
          se   = static_cast<long double>(strtod(ptst,&prst));
          if (hour_minute_second__is_valid(ho,mi,se))
            result = true;
          else
            result = false;
        }
        else
          result = false;
      }
      else
        result = false;
    }
    else
      result = false;
  }
  return result;
}
