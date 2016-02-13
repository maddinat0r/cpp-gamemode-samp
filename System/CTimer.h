#pragma once
#ifndef INC_TIMER_H
#define INC_TIMER_H


#include <unordered_set>
#include <functional>
#include <boost/chrono.hpp>

using std::unordered_set;
using std::function;
namespace chrono = boost::chrono;


class CTimer 
{
public: //functions (static)
	static inline CTimer *Create(unsigned int interval, bool repeat, function<void ()> func)
	{
		auto *timer = new CTimer(interval, repeat, std::move(func));
		sm_Timers.insert(timer);
		return timer;
	}
	static void ProcessGameTimers();
	

public: //functions
	inline void Kill()
	{
		sm_Timers.erase(this);
		delete this;
	}
	

private: //constructor / deconstructor
	CTimer(unsigned int interval, bool repeat, function<void()> &&func);
	~CTimer() = default;


private: //variables
	chrono::steady_clock::duration m_Interval;
	bool m_Repeat;
	function<void ()> m_Func;
	chrono::steady_clock::time_point m_LastStart;


private: //variables (static)
	static unordered_set<CTimer *> sm_Timers;

};

#endif // INC_TIMER_H

