#include <iostream>
#include <fstream>
#include <string>
#include "Task.h"
#include <deque>
#include <vector>

int main() {


	Task test = Task("0x12345678",2,3);

	std::cout << test.getID() << std::endl;

	system("pause");


	//read in all of our jobs that have to happen into Task jobs;

	//create job buffer initially empty
	std::deque<Task> ALL_JOBS;
	ALL_JOBS = ReadInJobs();

	std::deque<Task> FiFo;
	std::deque<Task> SJF;
	std::deque<Task> SJtC;
	std::deque<Task> RR;

	

	//check if new job arrived
		//if yes, add it to job buffer
	
	
	//Have a job list for FIFO, SJF, STtC, RR

	//Have a completed jobs list that stores 




}

std::deque<Task> ReadInJobs() {



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

