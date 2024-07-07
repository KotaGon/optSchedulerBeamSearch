#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <string>
#include <queue>
#include <cmath>
#include <chrono>
#include <tuple>
#include <iomanip>
#include <vector>
#include <sstream>
#include <map>
#include <bitset>
#include <iomanip>


using namespace std;

/* xor128 function */
static unsigned long xor128()
{
	static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
	unsigned long t;
	t = (x ^ (x << 11)); x = y; y = z; z = w;
	return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

/* randLR */
inline int randLR(int L, int R)
{
	return (L < R) ? (L + xor128() % (R - L)) : -1;
}

/* sq */
inline int sq(int x) { return x * x; }

/* timer class */
class timerClass
{
private:
	std::chrono::system_clock::time_point mBegin;
public:
	timerClass() = default;
	inline std::chrono::system_clock::time_point now() const { return std::chrono::system_clock::now(); }
	void start() { mBegin = now(); }
	double elapsed() { return std::chrono::duration_cast<std::chrono::nanoseconds>(now() - mBegin).count() * 1.0e-9; }
};


inline std::vector<string> split(const string &str)
{
  vector<string> v;
  char delim[] = {','};
  string item;

  for(int i = 0; i < (int) str.size(); ++i)
  {
    char ch = str[i];

    if(ch == delim[0])
    {
      if(!item.empty())
      {
        v.push_back(item);
      }
      item.clear();
    }
    else 
    {
      item += ch;
    }
  } 

  if(!item.empty())
  {
    v.push_back(item);
  }

  return v;
}

/*
template<typename var>
inline void stringParse(string &str, var &val)
{
  stringstream ss;
  ss << val;
  return ss.str();
}
*/

inline double doubleParse(string &str)
{
    double val;
    std::istringstream(str) >> val;
    return val;
}

inline int stop_time(int s1, int e1, int s2, int e2)
{
    if(s1 < s2 && s2 < e1 && e1 < e2) 
        return e2 - s2;
    else if(s2 < s1 && s1 < e2)
        return e2 - s2;
    else if(s2 < s1 && s1 < e2 && e2 < e1)
        return e2 - s1;
    else 
        return 0;
}

#endif