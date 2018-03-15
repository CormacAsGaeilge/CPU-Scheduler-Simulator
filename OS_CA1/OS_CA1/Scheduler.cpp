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
				if (arrivalTime > globalRunTime) //if the global run time at this point is further on than the accum runtime then push forward the runtime
					globalRunTime = arrivalTime;
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
	rr1ScheduledJobs = sortSJF(scheduledJobs);	//RR1 - inital sort the same as SJF 
	rr2ScheduledJobs = sortSJF(scheduledJobs);	//RR2 - inital sort the same as SJF 
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
	std::cout << "T\t\tFIFO\tSJF\tSTCF\tRR1\tRR2" << std::endl;
	while (currentTime <= globalRunTime) {
		tick(); //call tick function
		currentTime++; //increment 'time'
	}
	std::cout << "= SIMULATION COMPLETE" << std::endl;
}

void Scheduler::tick()
{
	//determine if a scheduled job has arrived
	if (!scheduledJobs.empty()) {
		if (scheduledJobs.size() > 1) {
			while (scheduledJobs.top().getArrivalTime() == currentTime) {
				std::cout << "* ARRIVED: " << scheduledJobs.top().getName() << std::endl;
				scheduledJobs.pop();
			}
		}
		else if (scheduledJobs.top().getArrivalTime() == currentTime) {
			std::cout << "* ARRIVED: " << scheduledJobs.top().getName() << std::endl;
			scheduledJobs.pop();
		}
	}

	fifoTick();
	sjfTick();
	stcfTick();
	rr1Tick();
	rr2Tick();

	printOutput();
}

void Scheduler::fifoTick()
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
}

void Scheduler::sjfTick()
{
	//SJF - sorted by shortest time and arrival time 

	if (!sjfRunningJobs.empty()) {
		//decrement time remaining on top item in running queue
		sjfRunningJobs.top().decrementTime();
		if (sjfRunningJobs.top().getTimeRemaining() == 0) {
			finishJob(SJF);//end process
		}
	}

	if (!sjfScheduledJobs.empty()) {
		if (sjfRunningJobs.empty()) {									//if no running jobs
			if (currentTime >= sjfScheduledJobs.top().getArrivalTime()) //only if it matches or surpasses scheduled time
				startJob(SJF);											//start new process
		}
	}
}

void Scheduler::stcfTick()
{
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
}

void Scheduler::rr1Tick()
{
	//RR1
	if (!rr1RunningJobs.empty()) {
		//decrement time remaining on top item in running queue
		rr1RunningJobs.top().decrementTime();
		if (rr1RunningJobs.top().getTimeRemaining() == 0) {
			finishJob(RR1);//end process
		}
	}

	if (!rr1ScheduledJobs.empty()) {										//unlike SJF, allow a job to start even if others are running
		if (currentTime >= rr1ScheduledJobs.top().getArrivalTime()) {		//only if it matches scheduled time
			startJob(RR1);
			if (rr1RunningJobs.size() > 1)
				rr1RunningJobs = orderRunningByTimeRemaining(rr1RunningJobs);	//shuffle running jobs so the shortest time to completion Job is first
		}
		if (rr1RunningJobs.size() > 1)
			rr1RunningJobs = roundRobinTimeShare(rr1Start, rr1Split, rr1RunningJobs);	//reorder round robin if needed
	}
}

void Scheduler::rr2Tick()
{
	//RR2
	if (!rr2RunningJobs.empty()) {
		//decrement time remaining on top item in running queue
		rr2RunningJobs.top().decrementTime();
		if (rr2RunningJobs.top().getTimeRemaining() == 0) {
			finishJob(RR2);//end process
		}
	}

	if (!rr2ScheduledJobs.empty()) {										//unlike SJF, allow a job to start even if others are running
		if (currentTime >= rr2ScheduledJobs.top().getArrivalTime()) {		//only if it matches scheduled time
			startJob(RR2);
			if (rr2RunningJobs.size() > 1)
				rr2RunningJobs = orderRunningByTimeRemaining(rr2RunningJobs);	//shuffle running jobs so the shortest time to completion Job is first
		}
		if (rr2RunningJobs.size() > 1)
			rr2RunningJobs = roundRobinTimeShare(rr2Start, rr2Split, rr2RunningJobs);	//reorder round robin if needed
	}
}

void Scheduler::startJob(SchedulerType type)
{
	//move Job from scheduled to running
	switch (type) {
	case FIFO:
		fifoRunningJobs.push(fifoScheduledJobs.top());
		fifoScheduledJobs.pop();
		fifoRunningJobs.top().setStartTime(currentTime);
		std::cout << "? STARTED:\t" + fifoRunningJobs.top().getName() << std::endl;
		break;
	case SJF:
		sjfRunningJobs.push(sjfScheduledJobs.top());
		sjfScheduledJobs.pop();
		sjfRunningJobs.top().setStartTime(currentTime);
		std::cout << "? STARTED:\t\t" + sjfRunningJobs.top().getName() << std::endl;
		break;
	case STCF:
		stcfRunningJobs.push(stcfScheduledJobs.top());
		stcfScheduledJobs.pop();
		stcfRunningJobs.top().setStartTime(currentTime);
		std::cout << "? STARTED:\t\t\t" + stcfRunningJobs.top().getName() << std::endl;
		break;
	case RR1:
		rr1RunningJobs.push(rr1ScheduledJobs.top());
		rr1ScheduledJobs.pop();
		rr1RunningJobs.top().setStartTime(currentTime);
		std::cout << "? STARTED:\t\t\t\t" + rr1RunningJobs.top().getName() << std::endl;
		break;
	case RR2:
		rr2RunningJobs.push(rr2ScheduledJobs.top());
		rr2ScheduledJobs.pop();
		rr2RunningJobs.top().setStartTime(currentTime);
		std::cout << "? STARTED:\t\t\t\t\t" + rr2RunningJobs.top().getName() << std::endl;
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
		std::cout << "* COMPLETE:\t" + fifoFinishedJobs.top().getName() << std::endl;
		break;
	case SJF:
		sjfFinishedJobs.push(sjfRunningJobs.top());
		sjfRunningJobs.pop();
		sjfFinishedJobs.top().setEndTime(currentTime);
		std::cout << "* COMPLETE:\t\t" + sjfFinishedJobs.top().getName() << std::endl;
		break;
	case STCF:
		stcfFinishedJobs.push(stcfRunningJobs.top());
		stcfRunningJobs.pop();
		stcfFinishedJobs.top().setEndTime(currentTime);
		std::cout << "* COMPLETE:\t\t\t" + stcfFinishedJobs.top().getName() << std::endl;
		break;
	case RR1:
		rr1FinishedJobs.push(rr1RunningJobs.top());
		rr1RunningJobs.pop();
		rr1FinishedJobs.top().setEndTime(currentTime);
		rr1Start = currentTime;
		std::cout << "* COMPLETE:\t\t\t\t" + rr1FinishedJobs.top().getName() << std::endl;
		break;
	case RR2:
		rr2FinishedJobs.push(rr2RunningJobs.top());
		rr2RunningJobs.pop();
		rr2FinishedJobs.top().setEndTime(currentTime);
		rr2Start = currentTime;
		std::cout << "* COMPLETE:\t\t\t\t\t" + rr2FinishedJobs.top().getName() << std::endl;
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
	if (!stcfRunningJobs.empty())
		stcfJob = stcfRunningJobs.top().getName().substr(0, 4);
	else
		stcfJob = "NA";
	//RR1
	if (!rr1RunningJobs.empty())
		rr1Job = rr1RunningJobs.top().getName().substr(0, 4);
	else
		rr1Job = "NA";
	//RR2
	if (!rr2RunningJobs.empty())
		rr2Job = rr2RunningJobs.top().getName().substr(0, 4);
	else
		rr2Job = "NA";

	outputString = "\t\t" + fifoJob + "\t" + sjfJob + "\t" + stcfJob + "\t" + rr1Job + "\t" + rr2Job;

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

std::stack<Job> Scheduler::roundRobinTimeShare( int start, int split, std::stack<Job> stack)
{
	if ((currentTime - start) % split == 0) { //only run whenever timesplit needs to be incremented
		std::vector<Job> tempJobs;
		std::stack<Job> returnStack;
		while (!stack.empty()) {
			tempJobs.push_back(stack.top());
			stack.pop();
		}

		std::rotate(tempJobs.begin(),
		tempJobs.end() - 1, // this will be the new first element
		tempJobs.end());

		for (int i = 0; i < tempJobs.size(); i++) {
			returnStack.push(tempJobs.at(i));
		}

		return reverseScheduled(returnStack);
	}
	else //don't change anything, change isn't needed
		return stack;
}

void Scheduler::analise()
{
	std::vector<Job> fifoJobs = sortJobs(fifoFinishedJobs);
	std::vector<Job> sjfJobs = sortJobs(sjfFinishedJobs);
	std::vector<Job> stcfJobs = sortJobs(stcfFinishedJobs);
	std::vector<Job> rr1Jobs = sortJobs(rr1FinishedJobs);
	std::vector<Job> rr2Jobs = sortJobs(rr2FinishedJobs);

	std::cout << "#\tJOB\tFIFO\tSJF\tSTCF\tRR1\tRR2" << std::endl;
	perJobStats(fifoJobs, sjfJobs, stcfJobs, rr1Jobs, rr2Jobs);
	std::cout << "= INDIVIDUAL STATS COMPLETE" << std::endl;

	std::cout << "# SCHEDULER\tAVG_TURNAROUND\tAVG_RESPONSE" << std::endl;
	aggregateStats(fifoJobs, sjfJobs, stcfJobs, rr1Jobs, rr2Jobs);
	std::cout << "= AGGREGATE STATS COMPLETE" << std::endl;
}

void Scheduler::perJobStats(std::vector<Job> fifoJobs, std::vector<Job> sjfJobs, std::vector<Job> stcfJobs, std::vector<Job> rr1Jobs, std::vector<Job> rr2Jobs)
{
	for (int i = 0; i < fifoJobs.size(); i++) {
		std::cout << "T\t" << fifoJobs[i].getName() << "\t" << fifoJobs[i].getTurnAroundTime() << "\t" << sjfJobs[i].getTurnAroundTime() << "\t" << stcfJobs[i].getTurnAroundTime() << "\t" << rr1Jobs[i].getTurnAroundTime() << "\t" << rr2Jobs[i].getTurnAroundTime() << std::endl;
	}

	for (int i = 0; i < fifoJobs.size(); i++) {
		std::cout << "R\t" << fifoJobs[i].getName() << "\t" << fifoJobs[i].getResponseTime() << "\t" << sjfJobs[i].getResponseTime() << "\t" << stcfJobs[i].getResponseTime() << "\t" << rr1Jobs[i].getResponseTime() << "\t" << rr2Jobs[i].getResponseTime() << std::endl;
	}
}

void Scheduler::aggregateStats(std::vector<Job> fifoJobs, std::vector<Job> sjfJobs, std::vector<Job> stcfJobs, std::vector<Job> rr1Jobs, std::vector<Job> rr2Jobs)
{
	std::cout << "@ FIFO\t\t" << averageTurnAround(fifoJobs) << "\t\t" << averageResponse(fifoJobs) << std::endl;
	std::cout << "@ SJF\t\t" << averageTurnAround(sjfJobs) << "\t\t" << averageResponse(sjfJobs) << std::endl;
	std::cout << "@ STCF\t\t" << averageTurnAround(stcfJobs) << "\t\t" << averageResponse(stcfJobs) << std::endl;
	std::cout << "@ RR1\t\t" << averageTurnAround(rr1Jobs) << "\t\t" << averageResponse(rr1Jobs) << std::endl;
	std::cout << "@ RR2\t\t" << averageTurnAround(rr2Jobs) << "\t\t" << averageResponse(rr2Jobs) << std::endl;
}

std::vector<Job> Scheduler::sortJobs(std::stack<Job> jobs)
{
	std::vector<Job> tempJobs;
	std::stack<Job> returnStack;
	while (!jobs.empty()) {
		tempJobs.push_back(jobs.top());
		jobs.pop();
	}

	std::sort(tempJobs.begin(), tempJobs.end(), [](const Job& lhs, const Job& rhs)
	{
		return lhs.getName() < rhs.getName();
	});

	return tempJobs;
}

float Scheduler::averageTurnAround(std::vector<Job> jobs)
{
	float avg = 0;
	for (int i = 0; i < jobs.size(); i++) 
		avg += jobs[i].getTurnAroundTime();
	return avg/jobs.size();
}

float Scheduler::averageResponse(std::vector<Job> jobs)
{
	float avg = 0;
	for (int i = 0; i < jobs.size(); i++) 
		avg += jobs[i].getResponseTime();
	return avg / jobs.size();
}
