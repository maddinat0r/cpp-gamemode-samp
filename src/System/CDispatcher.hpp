#pragma once

#include <System/CSingleton.h>

#include <functional>
#include <queue>
#include <boost/thread/mutex.hpp>


class CDispatcher : public CSingleton <CDispatcher>
{
	friend class CSingleton <CDispatcher>;
public: //type definitions
	using Func_t = std::function<void()>;

private: //constructor / destructor
	CDispatcher() = default;
	~CDispatcher() = default;

private: //variables
	std::queue<Func_t> m_Queue;
	boost::mutex m_QueueMtx;

public: //functions
	void Dispatch(Func_t &&func);
	void Process();

};
