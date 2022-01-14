#pragma once

#include <chrono>

class CELLTimestamp
{
public:
	CELLTimestamp()
	{
		//QueryPerformanceFrequency(&_frequency);
		//QueryPerformanceCounter(&_startCount);
		update();
	}

	~CELLTimestamp()
	{

	}

	void update()
	{
		//QueryPerformanceCounter(&_startCount);
		_begin = std::chrono::high_resolution_clock::now();
	}

	//��ȡ��ǰ��
	double getElapsedSecond()
	{
		return getElapsedTimeInMicroSec() * 0.000001;
	}

	//��ȡ����
	double getElapsedTimeInMilliSec()
	{
		return this->getElapsedTimeInMicroSec() * 0.001;
	}

	//��ȡ΢��
	long long getElapsedTimeInMicroSec()
	{
		//LARGE_INTEGER endCount;
		//QueryPerformanceCounter(&endCount);

		//double startTimeInMicroSec = _startCount.QuadPart * (1000000.0 / _frequency.QuadPart);
		//double endTimeInMicroSec = endCount.QuadPart * (1000000.0 / _frequency.QuadPart);

		//return endTimeInMicroSec - startTimeInMicroSec;
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}



protected:
	/*LARGE_INTEGER _frequency;
	LARGE_INTEGER _startCount;*/
	std::chrono::time_point<std::chrono::high_resolution_clock> _begin;
};
