#include "Job.hpp"
#include "Scheduler.hpp"
#include <iostream>
int main() {
	//TODO - make Job Class, make Scheduler Class
	//Input from file
	//FIFO
	//SJF
	//STCF
	//RR (2 variants / time slice values)

	Scheduler s = Scheduler("test.txt");
	s.run(); //startup scheduler

	//Job j = Job("Test", 1, 10); //Name is test, arrival time is 1, run time is 10

	getchar(); //pause output
	return 0;
}