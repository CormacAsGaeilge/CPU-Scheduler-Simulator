#pragma once
#include "Job.hpp"
#include <stack>
#include <fstream>
#include <iostream>

class Scheduler
{
public:
	Scheduler();
	Scheduler(std::string fileName); //needed to parse file
	~Scheduler();
	void readJobsFromFile(std::string fileUrl);

private:
	int globalRunTime, currentTime;
	std::stack<Job> scheduledJobs, runningJobs, finishedJobs;  //for storing jobs
};

