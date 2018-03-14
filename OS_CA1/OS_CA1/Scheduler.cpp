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
	std::stack<Job> temp;
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
				temp.push(job);
				globalRunTime += std::stoi(line);
			}
		}
		jobFile.close();
	}

	scheduledJobs = reverseScheduled(temp); //reverse stack 
	//copy scheduledJobs everywhere
	fifoScheduledJobs = scheduledJobs;			//FIFO - Make no changes, basic FIFO implementation
	sjfScheduledJobs = sortSJF(scheduledJobs);	//SJF  - sorted by shortest time and arrival time 
	stcfScheduledJobs = sortSJF(scheduledJobs);	//STCF - inital sort the same as SJF 
}

std::stack<Job> Scheduler::reverseScheduled(std::stack<Job> stack)
{
	//flips the stack - not very efficient, might switch to queues in the future
	std::stack<Job> reversed;
	while (!stack.empty()) {
		reversed.push(stack.top());
		stack.pop();
	}
	return reversed;
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
		if (fifoRunningJobs.empty()) {										//if no running jobs, otherwise wait til process is finished  
			if (currentTime >= fifoScheduledJobs.top().getArrivalTime())	//only if it matches or surpasses scheduled time
				startJob(FIFO);												//start new process
		}
	}

	//SJF - sorted by shortest time and arrival time 

	if (!sjfRunningJobs.empty()) {
		//decrement time remaining on top item in running queue
		sjfRunningJobs.top().decrementTime();
		if (sjfRunningJobs.top().getTimeRemaining() == 0) {
			finishJob(SJF);//end process
		}
	}

	if (!sjfScheduledJobs.empty()) {
		if (sjfRunningJobs.empty())	{									//if no running jobs
			if (currentTime >= sjfScheduledJobs.top().getArrivalTime()) //only if it matches or surpasses scheduled time
				startJob(SJF);											//start new process
		}
	}
	//STCF
	if (!stcfRunningJobs.empty()) {
		//decrement time remaining on top item in running queue
		stcfRunningJobs.top().decrementTime();
		if (stcfRunningJobs.top().getTimeRemaining() == 0) {
			finishJob(STCF);//end process
		}
	}

	if (!stcfScheduledJobs.empty()) {										//unlike SJF, allow a job to start even if others are running
		if (currentTime >= stcfScheduledJobs.top().getArrivalTime()) {		//only if it matches scheduled time
			startJob(STCF);													//start new process
			if (stcfRunningJobs.size() > 1)
				stcfRunningJobs = orderRunningByTimeRemaining(stcfRunningJobs);	//shuffle running jobs so the shortest time to completion Job is first
		}
	}


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
		sjfRunningJobs.push(sjfScheduledJobs.top());
		sjfScheduledJobs.pop();
		break;
	case STCF:
		stcfRunningJobs.push(stcfScheduledJobs.top());
		stcfScheduledJobs.pop();
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
		sjfFinishedJobs.push(sjfRunningJobs.top());
		sjfRunningJobs.pop();
		sjfFinishedJobs.top().setEndTime(currentTime);
		break;
	case STCF:
		stcfFinishedJobs.push(stcfRunningJobs.top());
		stcfRunningJobs.pop();
		stcfFinishedJobs.top().setEndTime(currentTime);
		break;
	}
}

void Scheduler::printOutput()
{
	std::string outputString, fifoJob, sjfJob, stcfJob, rr1Job, rr2Job;

	//FIFO
	if (!fifoRunningJobs.empty()) //job running
		fifoJob = fifoRunningJobs.top().getName().substr(0,4);
	else 
		fifoJob = "NA";
	//SJF
	if (!sjfRunningJobs.empty())
		sjfJob = sjfRunningJobs.top().getName().substr(0, 4);
	else
		sjfJob = "NA";
	//STCF
	if (!sjfRunningJobs.empty())
		stcfJob = stcfRunningJobs.top().getName().substr(0, 4);
	else
		stcfJob = "NA";

	outputString = "\t" + fifoJob + "\t" + sjfJob + "\t" + stcfJob;

	std::cout << currentTime << outputString << std::endl;
}

std::stack<Job> Scheduler::sortSJF(std::stack<Job> stack)
{
	std::vector<Job> tempJobs;
	std::stack<Job> returnStack;
	while (!stack.empty()) {
		tempJobs.push_back(stack.top());
		stack.pop();
	}

	std::sort(tempJobs.begin(), tempJobs.end(), [](const Job& lhs, const Job& rhs)
	{ //orders list by arrival time and if processes are scheduled at the same time then it orders those jobs by time remaining on those jobs
		if (lhs.getArrivalTime() == rhs.getArrivalTime()) {
			return lhs.getTimeRemaining() < rhs.getTimeRemaining();
		}
		else {
			return lhs.getArrivalTime() < rhs.getArrivalTime();
		}
	});

	for (int i = 0; i < tempJobs.size(); i++) {
		returnStack.push(tempJobs.at(i));
	}

	return reverseScheduled(returnStack); //flip the stack as it's in the wrong order
}

std::stack<Job> Scheduler::orderRunningByTimeRemaining(std::stack<Job> stack)
{
	std::vector<Job> tempJobs;
	std::stack<Job> returnStack;
	while (!stack.empty()) {
		tempJobs.push_back(stack.top());
		stack.pop();
	}

	std::sort(tempJobs.begin(), tempJobs.end(), [](const Job& lhs, const Job& rhs)
	{ 
		return lhs.getTimeRemaining() < rhs.getTimeRemaining();
	});

	for (int i = 0; i < tempJobs.size(); i++) {
		returnStack.push(tempJobs.at(i));
	}

	return reverseScheduled(returnStack); //flip the stack as it's in the wrong order
}