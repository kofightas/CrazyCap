#ifndef __TOOLS
#define  __TOOLS

#include "cocos2d.h"

using namespace cocos2d;

inline long millisecondNow()  
{ 
	struct cc_timeval now; 
	CCTime::gettimeofdayCocos2d(&now, NULL); 
	return (now.tv_sec * 1000 + now.tv_usec / 1000); 
} 

class Timer
{
public:
	enum TimerEnum
	{
		ERunning,
		EStop,
	};
	static long s_currentTime;
public:
	Timer();
	~Timer();
	long GetTime();
	void Play();
	void Stop();
	TimerEnum GetState();
	void Adjust(long val);
private:
	long m_startTime;
};

inline Timer::TimerEnum Timer::GetState()
{
	if (m_startTime == -1)
	{
		return Timer::EStop;
	}
	return Timer::ERunning;
}



inline long Timer::GetTime()
{
	return s_currentTime - m_startTime;
}

inline void Timer::Adjust(long val)
{
	m_startTime -= val;
}

inline void Timer::Play()
{
	m_startTime = s_currentTime;
}

inline void Timer::Stop()
{
	m_startTime = -1;
}

inline Timer::Timer():m_startTime(-1)
{
}

inline Timer::~Timer()
{
}

#endif // !__TOOLS

