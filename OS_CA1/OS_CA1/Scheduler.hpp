#pragma once
#include "Job.hpp"
#include <stack>
#include <fstream>
#include <iostream>

enum SchedulerType {
	FIFO,
	SJF,
	STCF,
	RR1,
	RR2,
};

class Scheduler
{
public:
	Scheduler();
	Scheduler(std::string fileName); //needed to parse file
	~Scheduler();

	void run();
	void tick();
	void readJobsFromFile(std::string fileUrl);
	void reverseScheduled();
	void startJob(SchedulerType type);
	void finishJob(SchedulerType type);
	void printOutput();

private:
	int globalRunTime, currentTime;
	std::stack<Job> scheduledJobs, //for storing jobs
		fifoScheduledJobs,
		fifoRunningJobs,
		fifoFinishedJobs,
		sjfScheduledJobs,
		sjfRunningJobs,
		sjfFinishedJobs;
};

