/* @Author
Student Name: Furkan Pala
Student ID : 150180109
Date: 08/11/2019 */

/*
PLEASE, DO NOT CHANGE void display(bool verbose, bool testing), int getUsableDay() and int getUsableTime() FUNCTIONS.
YOU HAVE TO WRITE THE REQUIRED  FUNCTIONS THAT IS MENTIONED ABOVE. YOU CAN ADD NEW FUNCTIONS IF YOU NEED.
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <iomanip>

#include "task_management_tool.h"

using namespace std; 


void WorkPlan::display(bool verbose,bool testing)
{
	string inone="***";
	if (head!=NULL)
	{
		Task *pivot =new Task;
		Task *compeer =new Task;
			
		pivot=head;
		do
		{
			if(testing)
				inone+=" ";
			else
				cout << pivot->day <<". DAY"<< endl;
			compeer=pivot;
			while(compeer!=NULL)
			{
				string PREV= compeer->previous!=NULL?compeer->previous->name:"NULL";
				string NEXT=compeer->next!=NULL?compeer->next->name:"NULL";
				string CONT=compeer->counterpart!=NULL?compeer->counterpart->name:"NULL";
				if (testing)
					inone+=compeer->name;
				else
					if(verbose)
						cout<<"\t"<<setw(2)<< compeer->time <<":00\t"<< PREV<<"\t<- "<< compeer->name <<"("<< compeer->priority <<")->\t"<<NEXT <<"\t |_"<<CONT<<endl;
					else
						cout<<"\t"<<setw(2)<< compeer->time <<":00\t"<< compeer->name <<"("<< compeer->priority <<")"<<endl;				
				compeer=compeer->counterpart;
			}
			pivot=pivot->next;
		}while(pivot!=head);
		if(testing)
		{
			cout<<inone<<endl;
			cout<<"(checking cycled list:";
			if (checkCycledList())
				cout<<" PASS)"<<endl;
			else
				cout<<" FAIL)"<<endl;
		}
	}
	else
		cout<<"There is no task yet!"<<endl;
}

int WorkPlan::getUsableDay()
{
	return usable_day;
}

int WorkPlan::getUsableTime()
{
	return usable_time;
}

/**
 * Copies the name of one task to the another task's name
 * @param sourceTask Source Task
 * @param destTask Destination Task
 */
void nameCopier(Task *sourceTask, Task *destTask) {
	int size = strlen(sourceTask->name);
	char *name = new char[size + 1];
	strcpy(name, sourceTask->name);
	*(name + size) = '\0';
	destTask->name = name;
}

void WorkPlan::create()
{
	head = NULL;
}

void WorkPlan::close()
{
	Task *traverseForDays, *traverseForHours, *tempForDays;
	if (head != NULL)
	{
		traverseForDays = head->next;
		while (traverseForDays != NULL && traverseForDays != head)
		{
			tempForDays = traverseForDays;
			traverseForHours = tempForDays->counterpart;
			while (traverseForHours)
			{
				tempForDays->counterpart = tempForDays->counterpart->counterpart;
				traverseForHours -> name = NULL;
				delete traverseForHours;
				traverseForHours = tempForDays->counterpart;
			}
			traverseForDays = traverseForDays->next;
			tempForDays -> name = NULL;
			delete tempForDays;
		}
		head -> name = NULL;
		delete head;
		head = NULL;
	}
}

void WorkPlan::add(Task *task)
{
	Task *traverseForDays, *traverseForHours, *tailForHours, *newTask;
	newTask = new Task;
	*newTask = *task;
	newTask -> next = NULL;
	newTask -> previous = NULL;
	newTask -> counterpart = NULL;

	nameCopier(task,newTask);

	// Check if list is empty
	if (head == NULL)
	{
		newTask->next = newTask;
		newTask->previous = newTask;
		newTask->counterpart = NULL;
		head = newTask;
	}
	else
	{
		traverseForDays = head;
		bool isDayExisting = false;
		// Check if the given task's day exits already
		do
		{
			if (traverseForDays->day == newTask->day)
			{
				isDayExisting = true;
				break;
			}
			traverseForDays = traverseForDays->next;
		} while (traverseForDays != head);

		if (isDayExisting)
		{
			// Check if the given task will be first task of the day
			if (newTask->time < traverseForDays->time)
			{
				// In case of first day, assign head to the given task
				if (head == traverseForDays)
				{
					head = newTask;
				}
				newTask->counterpart = traverseForDays;
				newTask->next = traverseForDays->next;
				newTask->previous = traverseForDays->previous;
				traverseForDays->previous = NULL;
				traverseForDays->next = NULL;
				newTask->next->previous = newTask;
				newTask->previous->next = newTask;
			}
			else
			{
				// Check if there is a contradiction
				Task *traverse;
				traverse = getTask(newTask->day, newTask->time);
				if (traverse)
				{
					// Delay the task with low priority
					checkAvailableNextTimesFor(traverse);
					if (newTask->priority <= traverse->priority)
					{
						newTask->day = getUsableDay();
						newTask->time = getUsableTime();
						add(newTask);
					}
					else
					{
						// If the task in the list(traverse) already has lower priority than of the new coming task(newTask)
						// then store the content of traverse in the tempTask and re-add it thus traverse will be delayed.
						// Since ,except for the name and priority, all the content of traverse and newTask are the same; 
						// transferring newTask's name and priority to the traverse is enough to add newTask into the list. 
						Task *tempTask = new Task;
						tempTask -> next = NULL;
						tempTask -> previous = NULL;
						tempTask -> counterpart = NULL;
						tempTask->day = getUsableDay();
						tempTask->time = getUsableTime();
						tempTask->priority = traverse->priority;

						nameCopier(traverse,tempTask);

						add(tempTask);

						traverse->priority = newTask->priority;

						nameCopier(newTask,traverse);
					}
				}
				// If there is no contradiciton then add the newTask to the counterpart
				else
				{
					// If counterpart is empty
					if (traverseForDays->counterpart == NULL)
					{
						newTask->counterpart = NULL;
						traverseForDays->counterpart = newTask;
					}
					else
					{
						// If newTask's time smaller than of the first counterpat
						if (newTask->time < traverseForDays->counterpart->time)
						{
							newTask->counterpart = traverseForDays->counterpart;
							traverseForDays->counterpart = newTask;
						}
						// If that is not the case then find the appropriate place
						else
						{
							traverseForHours = traverseForDays->counterpart;
							while (traverseForHours && newTask->time > traverseForHours->time)
							{
								tailForHours = traverseForHours;
								traverseForHours = traverseForHours->counterpart;
							}
							if (traverseForHours)
							{
								newTask->counterpart = traverseForHours;
								tailForHours->counterpart = newTask;
							}
							else
							{
								tailForHours->counterpart = newTask;
							}
						}
					}
				}
			}
		}
		// If newTask's day does not exist then create a new day
		else
		{
			// Add to the begining of the list
			if (newTask->day < head->day)
			{
				newTask->next = head;
				newTask->previous = head->previous;
				head->previous = newTask;
				newTask->previous->next = newTask;
				head = newTask;
			}
			// Add to the end of the list
			else if (newTask->day > head->previous->day)
			{
				head->previous->next = newTask;
				newTask->previous = head->previous;
				newTask->next = head;
				head->previous = newTask;
			}
			// Add somewhere in the middle of the list
			else
			{
				traverseForDays = head;
				do
				{
					if (traverseForDays->day > newTask->day)
					{
						break;
					}
					traverseForDays = traverseForDays->next;
				} while (traverseForDays != head);

				newTask->next = traverseForDays;
				newTask->previous = traverseForDays->previous;
				traverseForDays->previous->next = newTask;
				traverseForDays->previous = newTask;
			}
		}
	}
}

Task *WorkPlan::getTask(int day, int time)
{
	Task *traverseForDays = head;
	Task *traverseForHours;

	// Iterate through the all tasks
	do
	{
		if (traverseForDays->day == day)
		{
			if (traverseForDays->time == time)
			{
				return traverseForDays;
			}

			traverseForHours = traverseForDays->counterpart;
			while (traverseForHours)
			{
				if (traverseForHours->time == time)
				{
					return traverseForHours;
				}
				traverseForHours = traverseForHours->counterpart;
			}
		}
		traverseForDays = traverseForDays->next;
	} while (traverseForDays != head);

	return NULL;
}

void WorkPlan::checkAvailableNextTimesFor(Task *delayed)
{
	bool isThereAvaliableTime = false;

	Task *traverseForDays, *traverse, *tail;
	traverseForDays = head;

	// First, go to the first task of the delayed task's day
	while (traverseForDays->day != delayed->day)
	{
		traverseForDays = traverseForDays->next;
	}

	// Iterate through the tasks that are after the task will be delayed.
	// Keep the track of the time difference between consecutive tasks that are in the same day.
	// If the time difference is more than one hour then there is an avaliable time.
	do
	{
		if (traverseForDays->day == delayed->day)
		{
			tail = delayed;
			traverse = tail->counterpart;
			while (traverse)
			{
				int timeDifference = traverse->time - tail->time;
				if (timeDifference > 1)
				{
					isThereAvaliableTime = true;
					usable_day = tail->day;
					usable_time = tail->time + 1;
					break;
				}
				tail = traverse;
				traverse = traverse->counterpart;
			}
		}
		else
		{
			if (traverseForDays->time > 8)
			{
				isThereAvaliableTime = true;
				usable_day = traverseForDays->day;
				usable_time = 8;
				break;
			}
			else
			{
				tail = traverseForDays;
				traverse = traverseForDays->counterpart;
				while (traverse)
				{
					int timeDifference = traverse->time - tail->time;
					if (timeDifference > 1)
					{
						isThereAvaliableTime = true;
						usable_day = tail->day;
						usable_time = tail->time + 1;
						break;
					}
					tail = traverse;
					traverse = traverse->counterpart;
				}
			}
		}
		traverseForDays = traverseForDays->next;
	} while (traverseForDays != head && !isThereAvaliableTime);

	// This is the delaying part
	// First decide that whether is it needed to create a new day or not
	// Even if one of the days has last appointment with a time smaller than 16 then it is not required
	// to create a new day.
	if (!isThereAvaliableTime)
	{
		bool shouldCreateNewDay = true;
		traverseForDays = head;
		while (traverseForDays->day != delayed->day)
		{
			traverseForDays = traverseForDays->next;
		}
		do
		{
			traverse = traverseForDays;
			while (traverse->counterpart)
			{
				traverse = traverse->counterpart;
			}
			if (traverse->time < 16)
			{
				shouldCreateNewDay = false;
				usable_day = traverse->day;
				usable_time = traverse->time + 1;
				break;
			}
			traverseForDays = traverseForDays->next;
		} while (traverseForDays != head && shouldCreateNewDay);

		// If there is no choice but to create a new day 
		// then go to the last day with the help of the circular list.
		if (shouldCreateNewDay)
		{
			traverseForDays = head->previous;
			usable_day = traverseForDays->day + 1;
			usable_time = 8;
		}
	}
}

void WorkPlan::delayAllTasksOfDay(int day)
{
	Task *traverseForDays, *traverse, *tail, *storageTask;

	traverseForDays = head;

	// First, find the first task of the day that will be delayed.
	while (traverseForDays->day != day)
	{
		traverseForDays = traverseForDays->next;
	}

	traverse = traverseForDays;
	tail = traverse;

	// The fact that should be taken into account is that if there is no avaliable time in whole list and
	// last appointment of the day ,that will be delayed, is not at time of 16 then, checkAvailableNextTimesFor
	// function will select proper time in the CURRENT DAY. This is a problem because we want to delay that day completely.
	// In order to sort this out, I created a storageTask and copied the content (of tail) into this stroageTask except for the time value.
	// I changed the time value to 16 so checkAvailableNextTimesFor function will directly look for subsequent days.
	// Lastly, delete the task that will be delayed(tail) and add storageTask to the list.
	while (traverse)
	{
		tail = traverse;
		traverse = traverse->counterpart;

		storageTask = new Task;

		storageTask -> next = NULL;
		storageTask -> previous = NULL;
		storageTask -> counterpart = NULL;

		nameCopier(tail,storageTask);
		storageTask->day = tail->day;
		storageTask->time = 16;
		storageTask->priority = tail->priority;

		checkAvailableNextTimesFor(storageTask);
		storageTask->time = getUsableTime();
		storageTask->day = getUsableDay();

		remove(tail);
		add(storageTask);
	}
}

void WorkPlan::remove(Task *target)
{
	Task *traverseForDays, *traverseForHours, *tailForHours;
	// If the task will be removed is head then shift the head.
	if (head == target)
	{
		target->previous->next = target->next;
		target->next->previous = target->previous;
		head = target->next;
	}
	else
	{
		// This is checking if the task will be delayed is first task of the day
		if (target->previous || target->next)
		{
			// This is checking if the task will be delayed has countepart
			if (target->counterpart)
			{
				target->counterpart->previous = target->previous;
				target->counterpart->next = target->next;
				target->previous->next = target->counterpart;
				target->next->previous = target->counterpart;
			}
			else
			{
				target->previous->next = target->next;
				target->next->previous = target->previous;
			}
		}
		else
		{
			traverseForDays = head;
			// Go to the first task of the day that is of the will be removed task
			while (traverseForDays->day != target->day)
			{
				traverseForDays = traverseForDays->next;
			}
			traverseForHours = traverseForDays->counterpart;
			// Find the exact task by iterating through counterparts
			while (traverseForHours != target)
			{
				tailForHours = traverseForHours;
				traverseForHours = traverseForHours->counterpart;
			}
			tailForHours->counterpart = traverseForHours;
		}
	}
	target -> name = NULL;
	delete target;
}

bool WorkPlan::checkCycledList()
{				
	Task *pivot=new Task();
	pivot=head;
	int patient=100;
	bool r=false;
	while (pivot!=NULL&&patient>0)
	{		
		patient--;
		pivot=pivot->previous;
		if(pivot==head)
		{
			r=true;
			break;
		}
	}
	cout<<"("<<100-patient<<")";
	patient=100;
	bool l=false;
	while (pivot!=NULL&&patient>0)
	{		
		patient--;
		pivot=pivot->next;
		if(pivot==head)
		{
			l=true;
			break;
		}
	}
	return r&l;
}
