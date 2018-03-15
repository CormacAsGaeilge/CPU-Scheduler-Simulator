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
	void setStartTime(int startTime);
	int getArrivalTime() const;
	void decrementTime();
	int getTimeRemaining() const;
	float getTurnAroundTime();
	float getResponseTime();
private:
	std::string name;
	int arrivalTime, runTime, timeRemaining, startTime, endTime;
};

