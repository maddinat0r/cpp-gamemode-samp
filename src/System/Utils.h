#pragma once

#include <string>
#include <functional>

using std::string;

#include <System/CDispatcher.hpp>


template<typename T>
bool ConvertStrToData(const char *src, T &dest);

template<typename T>
bool ConvertDataToStr(T src, string &dest);


void ExecuteThreaded(std::function<void()> &&func);
inline void ExecuteInPawnThread(CDispatcher::Func_t &&func)
{
	CDispatcher::Get()->Dispatch(std::move(func));
}
