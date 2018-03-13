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

			name = words[0];
			arrivalTime = std::stoi(words[1]);

			runTime = std::stoi(line);

			Job job = Job(name, arrivalTime, runTime);
			scheduledJobs.push(job);
			globalRunTime += std::stoi(line);
		}
		jobFile.close();
	}
}
