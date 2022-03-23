#pragma once
#include<thread>
#include<atomic>
#include<Windows.h>
#include<tchar.h>

class initAnimation	//用于播放初始化动画
{
public:
	explicit initAnimation(uint32_t times = 1);
	~initAnimation();

	void waitAnimationFinish();

protected:
	void createAnimateWin();
	void Run();

	virtual void play();

private:
	HWND hwnd;
	HINSTANCE hInst;
	int32_t playTimes = 1;//最少播放次数

	std::atomic_bool endAnimeNow = false;
protected:
	std::unique_ptr<std::thread> initThread;//线程对象
};

LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
