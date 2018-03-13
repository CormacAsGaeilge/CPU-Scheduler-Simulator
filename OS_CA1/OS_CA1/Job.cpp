#include "Job.hpp"

Job::Job()
{
}

Job::Job(std::string name, int arrivalTime, int runTime)
	: name(name)
	, arrivalTime(arrivalTime)
	, runTime(runTime)
	, timeRemaining(runTime)
	, endTime(-1)
{
}


Job::~Job()
{
}

std::string Job::getName() const
{
	return name;
}

void Job::setEndTime(int endTime)
{
	Job::endTime = endTime;
}

int Job::getArrivalTime() const
{
	return arrivalTime;
}

void Job::decrementTime()
{
	timeRemaining--;
}

int Job::getTimeRemaining() const
{
	return timeRemaining;
}