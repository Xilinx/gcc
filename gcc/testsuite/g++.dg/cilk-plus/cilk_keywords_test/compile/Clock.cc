//***************************************************************************
//                                                                           
// File: Clock.cpp                                                            
//                                                                           
// Created: Thu Apr 21 14:53:08 2011                                         
//                                                                           
// Author: Balaji V. Iyer                                                    
//                                                                           
// $Id$                                                                      
//                                                                           
// Description:                                                              
//                                                                           
//***************************************************************************

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <list>
#include <assert.h>
#include <cilk/cilk.h>
#include <algorithm>


using namespace std;

class Clock
{
private:
  int hour;
  int minute;
  int second;

public:
  Clock();
  Clock(int hour, int minute, int seconds);
  ~Clock();
  void startClock();
  void printClock();
  Clock operator+(Clock &);
  Clock operator++();
  Clock operator-(Clock &);
  Clock operator--();
};


Clock::Clock()
{
  hour = 0;
  minute = 0;
  second = 0;

}

Clock::Clock (int sethour, int setminute, int setseconds)
{
  hour = sethour;
  minute = setminute;
  second = setseconds;
}


Clock::~Clock()
{
  hour = 0;
  minute = 0;
  second = 0;
}

void Clock::printClock()
{
  cout << "Time = " << hour << " : " << minute << " : " <<
    second << endl;

  return;
}

Clock Clock::operator++()
{
  this->second += 1;

  if (this->second >= 60)
  {
    this->second = this->second - 60;
    this->minute++;
    if (this->minute >= 60)
    {
      this->minute = this->minute-60;
      this->hour++;
    }
    if (this->hour >= 24)
    {
      this->hour = this->hour - 24;
    }
  }

  return *this;
}

Clock Clock::operator--()
{
  this->second -= 1;

  if (this->second < 0)
  {
    this->second = this->second + 60;
    this->minute++;
    if (this->minute < 0)
    {
      this->minute = this->minute+60;
      this->hour++;
    }
    if (this->hour <  0)
    {
      this->hour = this->hour + 24;
    }
  }

  return *this;
}


Clock Clock::operator+(Clock &currentTime)
{
  Clock thisVar = *this;

  thisVar.second += currentTime.second;

  if (thisVar.second >= 60)
  {
    thisVar.second = thisVar.second - 60;
    thisVar.minute++;
    if (thisVar.minute >= 60)
    {
      thisVar.minute = thisVar.minute-60;
      thisVar.hour++;
    }
    if (thisVar.hour >= 24)
    {
      thisVar.hour = thisVar.hour - 24;
    }
  }

  thisVar.minute += currentTime.minute;

  if (thisVar.minute >= 60)
  {
    thisVar.minute = thisVar.minute - 60;
    thisVar.hour++;

    if (thisVar.hour >= 24)
    {
      thisVar.hour = thisVar.hour-24;
    }
  }

  thisVar.hour += currentTime.hour;
  thisVar.hour = thisVar.hour % 24;

  return thisVar;
}

Clock Clock::operator-(Clock &currentTime)
{
  Clock thisVar = *this;

  thisVar.second -= currentTime.second;

  if (thisVar.second < 0)
  {
    thisVar.second += 60;
    thisVar.minute--;
    if (thisVar.minute < 0)
    {
      thisVar.minute = thisVar.minute+60;
      thisVar.hour--;
    }
    if (thisVar.hour < 0)
    {
      thisVar.hour = thisVar.hour + 24;
    }
  }

  thisVar.minute -= currentTime.minute;

  if (thisVar.minute < 0)
  {
    thisVar.minute = thisVar.minute + 60;
    thisVar.hour--;

    if (thisVar.hour < 0)
    {
      thisVar.hour = thisVar.hour+24;
    }
  }

  thisVar.hour -= currentTime.hour;
  if (thisVar.hour < 0)
  {
    thisVar.hour += 24;
  }

  return thisVar;
}


class ClockWall
{
private:
  vector<Clock> clocks;
  int NumberOfClocks;
public:
  ClockWall(int numberOfclocks);
  ~ClockWall();
  void DisplayAllClocks();
  void IncrementAllClocks();
  void IncrementAllClocksBy(int hr, int min , int sec);
  void DecrementAllClocks();
  void DecrementAllClocksBy(int hr, int min , int sec);
};


ClockWall::ClockWall(int numberOfclocks)
{
  Clock newClock;
  assert(numberOfclocks > 0);

  for (int ii = 0; ii < numberOfclocks; ii++)
  {
    clocks.push_back(newClock);
  }

  NumberOfClocks = numberOfclocks;
}

ClockWall::~ClockWall()
{
  clocks.clear();
}


void ClockWall::DisplayAllClocks()
{
  cilk_for (int ii = 0; ii < NumberOfClocks; ii++)
  {
    cout << "Clock Number " << ii << "     ";
    clocks[ii].printClock();
  }

  return;
}

void ClockWall::IncrementAllClocksBy (int hr, int min, int sec)
{

  Clock incrTime(hr, min, sec);

  cilk_for (int ii = 0; ii < NumberOfClocks; ii++)
  {
    clocks[ii] = clocks[ii] + incrTime; 
  }

  return;
}

void ClockWall::IncrementAllClocks ()
{
  cilk_for (int ii = 0; ii < NumberOfClocks; ii++)
  {
    ++clocks[ii];
  }
  return;
}

void ClockWall::DecrementAllClocks ()
{
  cilk_for (int ii = 0; ii < NumberOfClocks; ii++)
  {
    --clocks[ii];
  }
  return;
}
void ClockWall::DecrementAllClocksBy (int hr, int min, int sec)
{

  Clock incrTime(hr, min, sec);

  cilk_for (int ii = 0; ii < NumberOfClocks; ii++)
  {
    clocks[ii] = clocks[ii] - incrTime; 
  }

  return;
}



int main(void)
{
  ClockWall cWall(5);

  cWall.DisplayAllClocks();
 
  cWall.IncrementAllClocksBy(1,60,60);
  
  cWall.DecrementAllClocksBy(3,0,0);

  
  
  cWall.DisplayAllClocks();

  return 0;
}
  
