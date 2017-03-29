#include "obj_cont.h"

#include "pcb.h"

void** ObjectContainer::objects;
ID ObjectContainer::nextID = 0;
ID* ObjectContainer::idStack;
int ObjectContainer::idStackCounter = -1;
unsigned ObjectContainer::size = INITIAL_CONTAINER_SIZE;

ID ObjectContainer::store(void* object)
{
	PCB::lock();
	
	ID returnID;
	
	//If there are available recycled IDs, do
	if (idStackCounter >= 0)
	{
		returnID = idStack[idStackCounter--];
		objects[returnID] = object;
	}
	else
	{
		//If there are no recycled IDs, then
		if (nextID == size)
		{
			int i;
			size *= 2;

			void** tempObjects = objects;
			objects = new void*[size];
			for (i = 0; i < nextID; i++) objects[i] = tempObjects[i];
			delete[] tempObjects;

			ID* tempIDStack = idStack;
			idStack = new ID[size];
			for (i = 0; i <= idStackCounter; i++) idStack[i] = tempIDStack[i];
			delete[] tempIDStack;
		}
		
		returnID = nextID++;
		objects[returnID] = object;
	}
	
	PCB::unlock();
	
	return returnID;
}

void* ObjectContainer::remove(ID id)
{
	PCB::lock();
	
	idStack[++idStackCounter] = id;
	void *returnObject = objects[id];
	
	PCB::unlock();
	
	return returnObject;
}

void ObjectContainer::initialize()
{
	objects = new void*[INITIAL_CONTAINER_SIZE];
	idStack = new ID[INITIAL_CONTAINER_SIZE];
}

void ObjectContainer::finalize()
{
	delete objects;
	delete idStack;
}