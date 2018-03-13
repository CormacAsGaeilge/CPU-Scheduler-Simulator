#pragma once
#include <string>
class Job
{
public:
	Job();
	Job(std::string name, int arrivalTime, int runTime);
	~Job();

private:
	std::string name;
	int arrivalTime, runTime, timeRemaining, endTime;
};

