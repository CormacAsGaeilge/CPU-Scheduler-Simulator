#pragma once
#include "Job.hpp"
#include <stack>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

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
	std::stack<Job> reverseScheduled(std::stack<Job> stack);
	void startJob(SchedulerType type);
	void finishJob(SchedulerType type);
	void printOutput();
	std::stack<Job> sortSJF(std::stack<Job> stack);
	std::stack<Job> orderRunningByTimeRemaining(std::stack<Job> stack);
	std::stack<Job> roundRobinTimeShare(int start, int split, std::stack<Job> stack);

private:
	int globalRunTime, currentTime, rr1Start, rr2Start, rr1Split = 1, rr2Split = 5;
	std::stack<Job> scheduledJobs, //for storing jobs
		fifoScheduledJobs,
		fifoRunningJobs,
		fifoFinishedJobs,
		sjfScheduledJobs,
		sjfRunningJobs,
		sjfFinishedJobs,
		stcfScheduledJobs,
		stcfRunningJobs,
		stcfFinishedJobs,
		rr1ScheduledJobs,
		rr1RunningJobs,
		rr1FinishedJobs;
};

