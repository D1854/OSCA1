#include <iostream>
#include <fstream>
#include <string>
#include "Task.h"
#include <deque>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

// Author: Daniel Lynch
// Known Issues: -> Make sure there is no blank line at end of jobs.
//				-> Code is super inefficent, could do with using a list of list of jobs, then 1 loop can do everything
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
bool checkAllComplete(std::deque<Task> &FIFO, std::deque<Task> &SJF, std::deque<Task> &SJtC, std::deque<Task> &RR, std::deque<Task> &RR2, int max);
void addNewJobs(std::deque<Task> &scheduler, std::deque<Task> ALLJOBS, int iteration);
void swapTask(Task &x, Task &y);
Task GetMatchingTask(std::deque<Task> list, Task task);
float GetTurnaround(Task task);
int main(int argc, char ** argv);
void GetAggregatStatistics(std::deque<Task> inputList, float &avg_turn, float &avg_res);
void lineBreak();
void OutputArrivedJobs(std::deque<Task> inputList, int iteration);


void sort_SJF(std::deque<Task> &input); // Looks at Runtime, sorts by shortest first
void sort_SJtC(std::deque<Task> &input); //Looks at Runtime - Progress, sorts by by shortest first
void sort_RR(std::deque<Task> &input); //Switches between each task equally


// pass name of jobs file as agruement when calling .exe
int main(int argc, char **argv) {

	std::string path;

	if (argc < 2)
		path = "Jobs.txt";
	else
		path = argv[1];

	//create job buffer initially empty
	std::deque<Task> ALL_JOBS;
	ALL_JOBS = ReadInJobs(path);

	//Timeslice variables
	int timeSlice_One = 1; //used on most schedulers 
	int timeSlice_Two = 2; //only used on RR2

	//define Tasklist and completed lists
	std::deque<Task> FiFo, Completed_FiFo;
	std::deque<Task> SJF, Completed_SJF;
	std::deque<Task> STtC, Completed_STtC;
	std::deque<Task> RR, Completed_RR;
	std::deque<Task> RR2, Completed_RR2;

	//MAIN LOOP - loops until AllCompleted jobs equal all jobs size
	for (int i = 1; !checkAllComplete(Completed_FiFo, Completed_SJF, 
										Completed_STtC, Completed_RR, Completed_RR2,
										ALL_JOBS.size()); i++)
	{
		bool newJob = false;
		std::cout << "---------------- ITERATION " << i << "----------------" << std::endl;

		//ouput arrived jobs
		OutputArrivedJobs(ALL_JOBS, i);

		//for all of our schedulers, add any new jobs - not effiecent but does the job
		addNewJobs(FiFo, ALL_JOBS, i);
		addNewJobs(SJF, ALL_JOBS, i);
		addNewJobs(STtC, ALL_JOBS, i);
		addNewJobs(RR , ALL_JOBS, i);
		addNewJobs(RR2, ALL_JOBS, i);

		//Sort all of our schedulers
		if (i % timeSlice_One == 0) {
			sort_SJF(SJF);
			sort_SJtC(STtC);
			sort_RR(RR);
		}
		if (i % timeSlice_Two == 0) { sort_RR(RR2); }

		//All schedulers progress each task
		if (!FiFo.empty()) { FiFo[0].tick(i); }
		if (!SJF.empty()) { SJF[0].tick(i); }
		if (!STtC.empty()) { STtC[0].tick(i); }
		if (!RR.empty()) { RR[0].tick(i); }
		if (!RR2.empty()) { RR2[0].tick(i); }

		//All scheduler check completeion
		if (!FiFo.empty() && FiFo[0].checkComplete())
		{
			FiFo[0].setCompleted_Time(i); //set completed time
			Completed_FiFo.push_back(FiFo[0]); //add to completed list
			std::cout << "FIFO - COMPLETED: " << FiFo[0].getID() << std::endl; //console output
			FiFo.pop_front(); //pop off front
		}

		if (!SJF.empty() && SJF[0].checkComplete())
		{
			SJF[0].setCompleted_Time(i);
			Completed_SJF.push_back(SJF[0]);
			std::cout << "SJF - COMPLETED: " << SJF[0].getID() << std::endl;
			SJF.pop_front();
		}

		if (!STtC.empty() && STtC[0].checkComplete())
		{
			STtC[0].setCompleted_Time(i);
			Completed_STtC.push_back(STtC[0]);
			std::cout << "STTCF - COMPLETED: " << STtC[0].getID() << std::endl;
			STtC.pop_front();
		}

		if (!RR.empty() && RR[0].checkComplete())
		{
			RR[0].setCompleted_Time(i);
			Completed_RR.push_back(RR[0]);
			std::cout << "RR - COMPLETED: " << RR[0].getID() << std::endl;
			RR.pop_front();
		}

		if (!RR2.empty() && RR2[0].checkComplete())
		{
			RR2[0].setCompleted_Time(i);
			Completed_RR2.push_back(RR2[0]);
			std::cout << "RR2 - COMPLETED: " << RR2[0].getID() << std::endl;
			RR2.pop_front();
		}

	}//end main loop
	
	//per job statistics
	//Turn AroundTime
	lineBreak();
	std::cout << "Turnaround Time:" << std::endl;
	std::cout << "\t JOB \t FIFO \t SJF \t STTCF \t RR \t RR2" << std::endl;
	for (int i = 0; i < ALL_JOBS.size(); i++) {

		float t_FIFO, t_SJF, t_STTCF, t_RR, t_RR2;
		Task FIFO, SJF, STTCF, RR, RR2;

		// For each job, pull the completed job from compelted list
		FIFO = GetMatchingTask(Completed_FiFo, ALL_JOBS[i]);
		SJF = GetMatchingTask(Completed_SJF, ALL_JOBS[i]);
		STTCF = GetMatchingTask(Completed_STtC, ALL_JOBS[i]);
		RR = GetMatchingTask(Completed_RR, ALL_JOBS[i]);
		RR2 = GetMatchingTask(Completed_RR2, ALL_JOBS[i]);

		//for each job, get its response time
		t_FIFO = GetTurnaround(FIFO);
		t_SJF = GetTurnaround(SJF);
		t_STTCF = GetTurnaround(STTCF);
		t_RR = GetTurnaround(RR);
		t_RR2 = GetTurnaround(RR2);

		//output information
		std::cout << "\t" << ALL_JOBS[i].getID() <<
			"\t" << t_FIFO <<
			"\t" << t_SJF <<
			"\t" << t_STTCF <<
			"\t" << t_RR << 
			"\t" << t_RR2 <<
			std::endl;
	}
	lineBreak();
	std::cout << "AVERAGE RESPONSE AND TURNAROUND TIME PER SCHEDULER" << std::endl; //line break

	//Aggregate Statistics
	std::cout << "\t SCHEDULER \t TURNAROUND \t RESPONSE" << std::endl;
	float avg_res = 0;
	float avg_turn = 0;

	//FIFO
	GetAggregatStatistics(Completed_FiFo, avg_turn, avg_res);
	std::cout << "\t FIFO \t\t" << avg_turn << "\t\t" << avg_res << std::endl;

	//SJF
	avg_res = 0;
	avg_turn = 0;
	GetAggregatStatistics(Completed_SJF, avg_turn, avg_res);
	std::cout << "\t SJF \t\t" << avg_turn << "\t\t" << avg_res << std::endl;

	//STTCF
	avg_res = 0;
	avg_turn = 0;
	GetAggregatStatistics(Completed_STtC, avg_turn, avg_res);
	std::cout << "\t STTCF \t\t" << avg_turn << "\t\t" << avg_res << std::endl;

	//RR
	avg_res = 0;
	avg_turn = 0;
	GetAggregatStatistics(Completed_RR, avg_turn, avg_res);
	std::cout << "\t RR \t\t" << avg_turn << "\t\t" << avg_res << std::endl;

	//RR2
	avg_res = 0;
	avg_turn = 0;
	GetAggregatStatistics(Completed_RR2, avg_turn, avg_res);
	std::cout << "\t RR2 \t\t" << avg_turn << "\t\t" << avg_res << std::endl;

	system("pause");


}

//pass in job list and current iteration, outputs to console what job just arrived.
void OutputArrivedJobs(std::deque<Task> inputList, int iteration) {

	for (int k = 0; k < inputList.size(); k++) {

		if (iteration == inputList[k].getArrival_Time())
			std::cout << "ARRIVED: " << inputList[k].getID() << std::endl;
	}

}

//Pass in a list of tasks, will set average turnaround and response times
void GetAggregatStatistics(std::deque<Task> inputList, float &avg_turn, float &avg_res) {

	for (int i = 0; i < inputList.size(); i++)
	{
		avg_turn = avg_turn + GetTurnaround(inputList[i]);
		avg_res = avg_res + inputList[i].getResponseTime();
	}

	avg_turn = avg_turn / inputList.size();
	avg_res = avg_res / inputList.size();

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
bool checkAllComplete(std::deque<Task> &FIFO, std::deque<Task> &SJF, std::deque<Task> &SJtC, std::deque<Task> &RR, std::deque<Task> &RR2, int max) {

	if (FIFO.size() < max)
		return false;

	if (SJF.size() < max)
		return false;

	if (SJtC.size() < max)
		return false;

	if (RR.size() < max)
		return false;

	if (RR2.size() < max)
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
		
		}

	}

}

//Sorts by shortest runtime in the list -- modified bubble sort
void sort_SJF(std::deque<Task> &input)
{
	bool swapped;

	//leave if empty
	if (input.size() < 2)
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

//sorts by shortest (runtime - progress) in the list - bubble sort
void sort_SJtC(std::deque<Task> &input)
{
	bool swapped;

	if (input.size() < 2)
		return;

	//for every element except the last
	for (int i = 0; i < input.size() - 1; i++)
	{
		swapped = false;
		for (int j = 0; j < input.size() - 1; j++)
		{
			
			if ((input[j].getRuntime() - input[j].getProgress()) > (input[j + 1].getRuntime() - input[j + 1].getProgress() ))
			{
				swapTask(input[j], input[j + 1]);
				swapped = true;
			}
		}

		if (!swapped)
			break;
	}

}

//Puts "0" on back and shifts forwards
void sort_RR(std::deque<Task>& input)
{
	if (input.size() < 2)
		return;

	Task temp;
	temp = input[0];

	input.pop_front();
	input.push_back(temp);
}

//input tasklist and task, matches by ID
Task GetMatchingTask(std::deque<Task> list, Task task) {

	for (int i = 0; i < list.size(); i++) {

		if (task.getID() == list[i].getID())
			return list[i];
	}
	//if it doesn't find a match
}

//pass in a task, returns you its turnaround time
float GetTurnaround(Task task) {

	return task.getCompleted_Time() - task.getArrival_Time();
}

//does a line break
void lineBreak() {
	std::cout << std::endl;
}
