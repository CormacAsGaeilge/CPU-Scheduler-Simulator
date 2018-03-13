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
