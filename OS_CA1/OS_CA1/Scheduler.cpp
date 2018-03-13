#include "Scheduler.hpp"

Scheduler::Scheduler()
	: scheduledJobs()
	, runningJobs()
	, finishedJobs()
	, currentTime(0)
{
	readJobsFromFile("test.txt"); //use test.txt if not specified
}

Scheduler::Scheduler(std::string fileName) : Scheduler()
{
	readJobsFromFile(fileName);
}


Scheduler::~Scheduler()
{
}

void Scheduler::readJobsFromFile(std::string fileUrl)
{
	//TODO
}
