#include <System/CTimer.h>


unordered_set<CTimer *> CTimer::sm_Timers;


void CTimer::ProcessGameTimers() 
{
	chrono::steady_clock::time_point time = chrono::steady_clock::now();
	decltype(sm_Timers) trash_timers;

	for (auto &t : sm_Timers)
	{
		if( (time - t->m_LastStart) >= t->m_Interval) 
		{
			if(t->m_Func)
				t->m_Func();

			if(t->m_Repeat == false) 
			{
				delete t;
				trash_timers.insert(t);
			}
			else
				t->m_LastStart = time;
		}
	}

	for (auto &t : trash_timers)
		sm_Timers.erase(t);
}

CTimer::CTimer(unsigned int interval, bool repeat, function<void ()> &&func) :
	m_Interval(chrono::milliseconds(interval)), 
	m_Repeat(repeat),
	m_Func(func),
	m_LastStart(chrono::steady_clock::now())
{ }
