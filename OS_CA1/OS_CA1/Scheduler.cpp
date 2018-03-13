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
	while (currentTime <= globalRunTime) {
		tick(); //call tick function
		currentTime++; //increment 'time'
	}
}

void Scheduler::tick()
{
	//FIFO
	if (!scheduledJobs.empty()) {
		if (runningJobs.empty()) { //if no running jobs
			//start new process
			startJob();
		}
	}

	if (!runningJobs.empty()) {
		//decrement time remaining on top item in running queue
		runningJobs.top().decrementTime();
		std::cout << currentTime << "\t" << runningJobs.top().getName() << std::endl;
		if (runningJobs.top().getTimeRemaining() == 0) {
			//end process
			finishJob();
		}
	}


	//SJF


	//STCF


	//RR1


	//RR1

}

void Scheduler::startJob()
{
	//move Job from scheduled to running
	runningJobs.push(scheduledJobs.top());
	scheduledJobs.pop();
}

void Scheduler::finishJob()
{
	//move Job from running to finished
	finishedJobs.push(runningJobs.top());
	runningJobs.pop();
	finishedJobs.top().setEndTime(currentTime);
}

