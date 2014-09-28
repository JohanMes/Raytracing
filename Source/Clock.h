#ifndef CLOCK_H
#define CLOCK_H

#include <windows.h>

class Clock {
	__int64 starttime;
	double secpercount;
	public:
		Clock(bool start);
		~Clock();
		
		void Start();
		double GetTimeSec();
		double Reset();
};

#endif
