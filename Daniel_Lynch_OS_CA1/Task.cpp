#include "Task.h"
#include <string>



Task::Task()
{
	setID("0x123456478");
	setRuntime(0);
	setArrival_Time(0);
	Task::Progress = 0;
}

Task::Task(std::string ID, int Runtime, int Arrival_Time)
{
	setID(ID);
	setRuntime(Runtime);
	setArrival_Time(Arrival_Time);
	Task::Progress = 0;
}

void Task::tick()
{
	Task::Progress++;
}

bool Task::checkComplete()
{
	if (Task::Progress >= Task::Runtime)
		return true;
	else
		return false;
		
}

void Task::setID(std::string input)
{
	Task::ID = input;
}

void Task::setRuntime(int input)
{
	Task::Runtime = input;
}

void Task::setArrival_Time(int input)
{
	Task::Arrival_Time = input;
}

void Task::setCompleted_Time(int input)
{
	Task::Completed_Time = input;
}

int Task::getRuntime()
{
	return Task::Runtime;
}

int Task::getArrival_Time()
{
	return Task::Arrival_Time;
}

std::string Task::getID()
{
	return Task::ID;
}

int Task::getProgress()
{
	return Task::Progress;
}

int Task::getCompleted_Time()
{
	return Task::Completed_Time;
}

