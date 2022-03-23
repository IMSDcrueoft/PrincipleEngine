#pragma once
#ifndef PERFORMANCETIMER_H
#define PERFORMANCETIMER_H
#include <windows.h>
#include <cstdint>
class PerformanceTimer
{
public:
	PerformanceTimer();

	float TotalTime()const; //秒
	float DeltaTime()const; //秒

	bool IsStoped()const;   //是否暂停

	void Reset(); //开始消息循环前调用
	void Start(); //解除暂停时调用
	void Stop();  //暂停时调用
	void Tick();  //每帧调用,计算时间间隔

private:
	double mSecondsPerCount;	//计数器每一次需要多少秒
	double mDeltaTime;			//每帧时间(前一帧和当前帧的时间差)

	int64_t mBaseTime;	//开始时刻时间
	int64_t mPausedTime;//暂停累计时间
	int64_t mStopTime;	//停止时刻的时间
	int64_t mPrevTime;	//上一帧时刻时间
	int64_t mCurrTime;	//本帧时刻时间

	bool Stopped;		//是否为停止的状态
};

//--------------------------------------------------------------------------
inline PerformanceTimer::PerformanceTimer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	mPausedTime(0), mStopTime(0), mPrevTime(0), mCurrTime(0), Stopped(false)
{
	//计算计数器每秒多少次，并存入countsPerSec中返回
	//QueryPerformanceFrequency函数
	int64_t countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

inline float PerformanceTimer::TotalTime() const
{
	if (Stopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

inline float PerformanceTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

inline bool PerformanceTimer::IsStoped() const
{
	return Stopped;
}

inline void PerformanceTimer::Reset()
{
	static int64_t currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;	//当前时间作为基准时间
	mPrevTime = currTime;	//当前时间作为上一帧时间，因为重置了，此时没有上一帧
	mStopTime = 0;			//重置停止那一刻时间为0
	Stopped = false;		//重置后的状态为不停止
}

inline void PerformanceTimer::Start()
{
	static int64_t startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	//如果处于暂停则更新
	if (Stopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;//重置
		Stopped = false;
	}
}

inline void PerformanceTimer::Stop()
{
	//已经暂停择无动作
	if (!Stopped)
	{
		static int64_t currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;//存储暂停时刻
		Stopped = true;
	}
}

inline void PerformanceTimer::Tick()
{
	if (Stopped)
	{
		//如果当前是停止状态，则帧间隔时间为0
		mDeltaTime = 0.0;
		return;
	}

	//获取本帧的开始时刻
	static int64_t currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	//渲染时间=时间差,计算当前帧和前一帧的时间差(计数差*每次多少秒)
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	//准备计算当前帧和下一帧的时间差
	mPrevTime = mCurrTime;

	//可能存在为负值
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

#endif