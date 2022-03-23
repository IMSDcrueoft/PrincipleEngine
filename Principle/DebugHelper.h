#pragma once
//#include<cstdlib>
#include<tchar.h>
#include<windows.h>
#include<string>
#include"clockTimer.h"

class DebugHelper:public clockTimer
{
private:
	std::wstring eventIF;
	
public:
	explicit DebugHelper(std::wstring eventInfo = L"")
	{
		SetEventName(eventInfo);
	}

	inline void SetEventName(std::wstring eventInfo)
	{
		eventIF = eventInfo;
	}

	inline void Start()
	{
		std::wstring dbgword = L"[Debug Timer start,Event:" + eventIF + L"]\n";
		Log(dbgword.c_str());

		TimerStart();
	}

	inline void Stop()
	{
		TimerStop();
		std::wstring dbgword = L"[Debug Timer Stop,Event" + eventIF + L",Time Use : " + std::to_wstring(getTime()) + L"(s)]\n";
		Log(dbgword.c_str());
	}

public:
	template<typename T, typename ...Args>
	static inline void Log(T& value,const Args& ...args)
	{
		static LPCSTR Enter = "\n";
		static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "Log can only work with value type!");
		OutputDebugStringA(std::to_string(value));

		if constexpr (sizeof ...(args) > 0)
			Log(args...);
		else
			OutputDebugStringA(Enter);
	}

	static inline void Log(LPCSTR info)
	{
		OutputDebugStringA(info);
	}

	static inline void Log(LPCWSTR info)
	{
		OutputDebugString(info);
	}

	static inline void Log(const std::string &info)
	{
		OutputDebugStringA(info.c_str());
	}

	static inline void Log(const std::wstring &info)
	{
		OutputDebugString(info.c_str());
	}
};