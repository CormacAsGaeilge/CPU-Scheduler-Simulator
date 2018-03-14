#include "Scheduler.hpp"

Scheduler::Scheduler()
	: scheduledJobs()
	, fifoScheduledJobs()
	, sjfScheduledJobs()
	, fifoRunningJobs()
	, fifoFinishedJobs()
	, sjfRunningJobs()
	, sjfFinishedJobs()
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
	//parse jobs from file, format
	std::string line, delimiter = " ", jobName;
	int startTime, runTime;
	std::ifstream jobFile(fileUrl);

	globalRunTime = 0; //initialise as zero, will be incremented based on parsed files

	if (jobFile.is_open()) {
		while (std::getline(jobFile, line)) //loop through each line
		{
			size_t pos = 0;
			std::string words[3];
			int i = 0;

			while ((pos = line.find(delimiter)) != std::string::npos) { //loop through each word
				words[i] = line.substr(0, pos);
				line.erase(0, pos + delimiter.length());
				i++;
			}

			std::string name;
			int arrivalTime, runTime;

			if (words[0] != " " && words[1] != " ") {
				name = words[0];
				arrivalTime = std::stoi(words[1]);
				runTime = std::stoi(line);

				Job job = Job(name, arrivalTime, runTime);
				scheduledJobs.push(job);
				globalRunTime += std::stoi(line);
			}
		}
		reverseScheduled(); //flip the stack
		jobFile.close();
	}

	//copy scheduledJobs everywhere
	fifoScheduledJobs = scheduledJobs;
	sjfScheduledJobs = scheduledJobs;
}

void Scheduler::reverseScheduled()
{
	//flips the stack - not very efficient, might switch to queues in the future
	std::stack<Job> reversed;
	while (!scheduledJobs.empty()) {
		reversed.push(scheduledJobs.top());
		scheduledJobs.pop();
	}
	scheduledJobs = reversed;
}

void Scheduler::run()
{
	std::cout << "T\tFIFO\tSJF\tSTCF\tRR1\tRR2" << std::endl;
	while (currentTime <= globalRunTime) {
		tick(); //call tick function
		currentTime++; //increment 'time'
	}
}

void Scheduler::tick()
{
	//FIFO
	if (!fifoRunningJobs.empty()) {
		//decrement time remaining on top item in running queue
		fifoRunningJobs.top().decrementTime();
		if (fifoRunningJobs.top().getTimeRemaining() == 0) {
			//end process
			finishJob(FIFO);
		}
	}

	if (!fifoScheduledJobs.empty()) {
		if (fifoRunningJobs.empty()) { //if no running jobs
									   //start new process
			startJob(FIFO);
		}
	}


	//SJF
	if (!sjfRunningJobs.empty()) {
		//decrement time remaining on top item in running queue
		sjfRunningJobs.top().decrementTime();
		if (sjfRunningJobs.top().getTimeRemaining() == 0) {
			//end process
			finishJob(SJF);
		}
	}

	//SORT

	if (!sjfScheduledJobs.empty()) {
		if (sjfRunningJobs.empty()) { //if no running jobs
									  //start new process
			startJob(SJF);
		}
	}
	//STCF


	//RR1


	//RR1


	printOutput();
}

void Scheduler::startJob(SchedulerType type)
{
	//move Job from scheduled to running
	switch (type) {
	case FIFO:
		fifoRunningJobs.push(fifoScheduledJobs.top());
		fifoScheduledJobs.pop();
		break;
	case SJF:
		break;
	}
	
}

void Scheduler::finishJob(SchedulerType type)
{
	//move Job from running to finished
	switch (type) {
	case FIFO:
		fifoFinishedJobs.push(fifoRunningJobs.top());
		fifoRunningJobs.pop();
		fifoFinishedJobs.top().setEndTime(currentTime);
		break;
	case SJF:
		break;
	}
}

void Scheduler::printOutput()
{
	std::string outputString, fifoJob, sjfJob, stcfJob, rr1Job, rr2Job;

	if (!fifoRunningJobs.empty()) //job running
		fifoJob = fifoRunningJobs.top().getName().substr(0,4);
	else 
		fifoJob = "NA";

	if (!sjfRunningJobs.empty())
		sjfJob = sjfRunningJobs.top().getName();
	else
		sjfJob = "NA";

	outputString = "\t" + fifoJob + "\t" + sjfJob;

	std::cout << currentTime << outputString << std::endl;
}