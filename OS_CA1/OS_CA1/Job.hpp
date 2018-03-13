#pragma once
#include <string>
class Job
{
public:
	Job();
	Job(std::string name, int arrivalTime, int runTime);
	~Job();

	std::string getName() const;
	void setEndTime(int endTime);
	int getArrivalTime() const;
	void decrementTime();
	int getTimeRemaining() const;

private:
	std::string name;
	int arrivalTime, runTime, timeRemaining, endTime;
};

