#ifndef _obj_cont_h_
#define _obj_cont_h_

typedef int ID;
const unsigned INITIAL_CONTAINER_SIZE = 1024;

#include "pcb.h"

//*Threadsafe*

class ObjectContainer
{
public:

	static ID    store   (void* object);
	static void* retrieve(ID id);
	static void* remove  (ID id);
	
	static void initialize();
	static void finalize();

	static void** objects;

	static ID nextID;
	static ID* idStack;
	static int idStackCounter;

	static unsigned size;
};

inline void* ObjectContainer::retrieve(ID id)
{
	return objects[id];
}

#endif