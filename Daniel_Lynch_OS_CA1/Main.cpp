#include <iostream>
#include <fstream>
#include <string>
#include "Task.h"
#include <deque>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

// Author: Daniel Lynch
// Known Issues: -> Make sure there is no blank line at end of jobs.txt
/*	
	C++ app to schedule hypothetical Tasks/Processes/Jobs by:
		> First in first out
		> Shortest job first
		> Shortest job to completion first
		> Round Robin
					

	1. Scheduler reads in Tasks from a file with ID, Runtime, Arrivaltime
	2. Every iteration the scheduler checks a job has arrived
	3. When a job arrives,
		-> Gets added to each scheduler's buffer
		-> Console outputs ARRIVED: Task::ID
	4. Each scheduler then sorts itself the order it will complete the Tasks.
		ie. It will put highest priority in lowest index.
	5. Each scheduler then progresses the task in its "0" index
	6. Each scheduler will check if job in "0" index is completed 
	7. When a job is completed it gets added to CompletedJobs
		-> CompletedJobs sets "Completed_Time" to the current iteration
		-> Console outputs COMPLETED: Task:ID
	8. Scheduler stops when no more jobs left to add AND no jobs left to process.
		-> Output SIMULATION COMPLETED
 */

//	Function declarations 
std::deque<Task> ReadInJobs(std::string path);
std::deque<std::string> split(std::string &data, std::string delimiter);
bool checkArrivedComplete(std::deque<Task> &inputList, int iteration);
bool checkAllComplete(std::deque<Task> &FIFO, std::deque<Task> &SJF, std::deque<Task> &SJtC, std::deque<Task> &RR, int max);
void addNewJobs(std::deque<Task> &scheduler, std::deque<Task> ALLJOBS, int iteration);
void swapTask(Task &x, Task &y);
Task GetMatchingTask(std::deque<Task> list, Task task);
float GetTurnaround(Task task);


void sort_SJF(std::deque<Task> &input); // Looks at Runtime, sorts by shortest first
void sort_SJtC(std::deque<Task> input); //Looks at Runtime - Progress, sorts by by shortest first
void sort_RR(std::deque<Task> input); //Switches between each task equally

int main() {


	Task test = Task("0x12345678",2,3);

	std::cout << test.getID() << std::endl;

	//read in all of our jobs that have to happen into Task jobs;

	//create job buffer initially empty
	std::deque<Task> ALL_JOBS;
	ALL_JOBS = ReadInJobs("Jobs.txt");
	std::cout << ALL_JOBS[0].getID() << std::endl;


	std::deque<Task> FiFo, Completed_FiFo;
	std::deque<Task> SJF, Completed_SJF;
	std::deque<Task> STtC, Completed_STtC;
	std::deque<Task> RR, Completed_RR;

	//MAIN LOOP - loops until AllCompleted jobs equal all jobs size
	for (int i = 1; !checkAllComplete(Completed_FiFo, Completed_SJF, 
										Completed_STtC, Completed_RR, 
										ALL_JOBS.size()); i++)
	{
		//for all of our schedulers, add any new jobs - not effiecent but does the job
		addNewJobs(FiFo, ALL_JOBS, i);
		addNewJobs(SJF, ALL_JOBS, i);
		addNewJobs(STtC, ALL_JOBS, i);
		addNewJobs(RR , ALL_JOBS, i);

		//Sort all of our schedulers
		sort_SJF(SJF);


		//All schedulers progress each task
		FiFo[0].tick();
		SJF[0].tick();

		//All scheduler check completeion
		if (FiFo[0].checkComplete())
		{
			FiFo[0].setCompleted_Time(i);
			Completed_FiFo.push_back(FiFo[0]);
			std::cout << "COMPLETED: " << FiFo[0].getID() << std::endl;
			FiFo.pop_front();
		}

		if (SJF[0].checkComplete())
		{
			SJF[0].setCompleted_Time(i);
			Completed_FiFo.push_back(SJF[0]);
			std::cout << "COMPLETED: " << SJF[0].getID() << std::endl;
			SJF.pop_front();
		}

	}//end main loop

	//per job statistics
	//Turn AroundTime
	std::cout << "Turnaround Time:" << std::endl;
	std::cout << "\\t JOB \\t FIFO \\t SJF \\t STTCF \\t RR" << std::endl;
	for (int i = 0; i < ALL_JOBS.size(); i++) {

		float t_FIFO, t_SJF, t_STTCF, t_RR;
		Task FIFO, SJF, STTCF, RR;

		// For each job, pull the completed job from compelted list
		FIFO = GetMatchingTask(Completed_FiFo, ALL_JOBS[i]);
		SJF = GetMatchingTask(Completed_SJF, ALL_JOBS[i]);
		STTCF = GetMatchingTask(Completed_STtC, ALL_JOBS[i]);
		RR = GetMatchingTask(Completed_RR, ALL_JOBS[i]);

		//for each job, get its response time
		t_FIFO = GetTurnaround(FIFO);
		t_SJF = GetTurnaround(SJF);
		t_STTCF = GetTurnaround(STTCF);
		t_RR = GetTurnaround(RR);

		//output information
		std::cout << "\\t" << (i + 1) <<
			"\\t" << t_FIFO <<
			"\\t" << t_SJF <<
			"\\t" << t_STTCF <<
			"\\t" << t_RR << std::endl;

	}



}

//reads in the jobs from the text file
std::deque<Task> ReadInJobs(std::string path) {

	std::deque<Task> outList; // our return list off all tasks
	std::string inLine; // string we read the line into
	std::deque<std::string> s_inLine; // our line split into a Que
	Task tempTask; //holds our task before adding it to outlist
	int tempInput; // temp holder for parseInt

	std::ifstream myReadFile; // file
	myReadFile.open(path);

	//loop until no lines are left
	while (!myReadFile.eof())
	{
		getline(myReadFile, inLine);//read line from file into string
		s_inLine = split(inLine, ";"); // split string into que

		//set temp task ID
		tempTask.setID(s_inLine[0]);
		
		//set temp task ArrivalTime
		tempInput = atoi(s_inLine[1].c_str());
		tempTask.setArrival_Time(tempInput);

		//set temp task Runtime
		tempInput = atoi(s_inLine[2].c_str());
		tempTask.setRuntime(tempInput);

		//Add temp task to out outlist
		outList.push_back(tempTask);
	}

	return outList;
	
}


//splits a string using a delimiter and returns a vector e.g. "torch/light/walk" and "/" returns torch, light, walk
std::deque<std::string> split(std::string &data, std::string delimiter)
{
	//if the last character is not whitespace, we add whitespace
	if (data.back() != ' ')
		data = data + " ";

	std::deque<std::string> outVector;
	std::string strElement;
	size_t oldPos = -1;
	size_t pos = data.find(delimiter, oldPos + 1);
	while (pos != std::string::npos)
	{
		strElement = data.substr(oldPos + 1, pos - oldPos - 1);
		outVector.push_back(strElement);
		oldPos = pos;
		pos = data.find(delimiter, oldPos + 1);
	}

	return outVector;
}

//pass in list and iteration, returns true if it finds a task that hasn't arrived yet
bool checkArrivedComplete(std::deque<Task> &inputList, int iteration)
{
	for (int j = 0; j < inputList.size(); j++)
	{
		if (inputList[j].Arrival_Time > iteration)
			return false;
	}

	return true;

}

//checks if all schedulers have completed all their tasks
bool checkAllComplete(std::deque<Task> &FIFO, std::deque<Task> &SJF, std::deque<Task> &SJtC, std::deque<Task> &RR, int max) {

	if (FIFO.size() < max)
		return false;

	if (SJF.size() < max)
		return false;

	if (SJtC.size() < max)
		return false;

	if (RR.size() < max)
		return false;

	//we we got this far we've nothing left to do
	return true;
}

//pass in scheduler, joblist and iteration. Add's all jobs of the current iteration to the scheduler
void addNewJobs(std::deque<Task> &scheduler, std::deque<Task> ALLJOBS, int iteration)
{
	//for everything in alljobs
	for (int k = 0; k < ALLJOBS.size(); k++)
	{
		// if the job arrives at this time, then add it to the list
		if (ALLJOBS[k].Arrival_Time == iteration) 
		{
			scheduler.push_back(ALLJOBS[k]);
			std::cout << "ARRIVED:" << ALLJOBS[k].getID() << std::endl;
		}

	}

}

//Sorts by sortest runtime in the list -- modified bubble sort
void sort_SJF(std::deque<Task> &input)
{
	bool swapped;

	if (input.size() < 1)
		return;

	//for every element except the last
	for (int i = 0; i < input.size() - 1; i++)
	{
		swapped = false;
		for (int j = 0; j < input.size() - 1; j++)
		{
			if (input[j].getRuntime() > input[j + 1].getRuntime())
			{
				swapTask(input[j], input[j + 1]);
				swapped = true;
			}
		}

		if (!swapped)
			break;
	}
}

//input tasklist and task, matches by ID
Task GetMatchingTask(std::deque<Task> list, Task task) {

	for (int i = 0; i < list.size(); i++) {

		if (task.getID() == list[i].getID())
			return list[i];
	}
	//if it doesn't find a match
}

float GetTurnaround(Task task) {

	return task.getCompleted_Time() - task.getArrival_Time();
}

//---------------- sorting stuff. REF: https://www.geeksforgeeks.org/bubble-sort/ ----------------

void swapTask(Task &x, Task &y)
{
	Task temp = x;
	x = y;
	y = temp;

}

/* An optimized version of Bubble Sort
void bubbleSort(std::deque<Task> inputTask, int n)
{
	int i, j;
	bool swapped;
	for (i = 0; i < n - 1; i++)
	{
		swapped = false;
		for (j = 0; j < n - i - 1; j++)
		{
			if (inputTask[j]. > arr[j + 1])
			{
				swap(&arr[j], &arr[j + 1]);
				swapped = true;
			}
		}

		// IF no two elements were swapped by inner loop, then break
		if (swapped == false)
			break;
	}
}
void swap(int *xp, int *yp)
{
int temp = *xp;
*xp = *yp;
*yp = temp;
}

*/