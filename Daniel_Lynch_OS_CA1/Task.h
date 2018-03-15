#pragma once
#pragma once
#include <deque>



class Task
{
public:

	//local Variables
	int Runtime, Arrival_Time, Progress, Completed_Time;
	std::string ID;

	//black constructor (When user creates a new email)
	Task();

	//Runtime is how long the task has left to run, Arrival time is when the task is going to arrive
	Task(std::string ID, int Runtime, int Arrival_Time);

	//Progress Functions
	void tick();
	bool checkComplete();

	void setID(std::string input);
	void setRuntime(int input);
	void setArrival_Time(int input);
	void setCompleted_Time(int input);
	

	//getters
	int getRuntime();
	int getArrival_Time();
	std::string getID();
	int getProgress();
	int getCompleted_Time();

	//sorters
	void sort_FiFo(); //Wings it
	void sort_SJF(); // Looks at Runtime, sorts by shortest first
	void sort_SJtC(); //Looks at Runtime - Progress, sorts by by shortest first
	void sort_RR(); //Switches between each task equally

	

};

