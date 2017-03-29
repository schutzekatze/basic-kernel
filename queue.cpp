#include "queue.h"

#include "pcb.h"

void Queue::insert(void* data)
{
	PCB::lock();
	
	rear = (rear ? (rear->next = new Node(data)) : (front = new Node(data)));
	
	PCB::unlock();
}

void* Queue::remove()
{
	PCB::lock();
	
	void *data;
	if (front)
	{
		Node *temp = front;
		data = temp->data;
		front = front->next;
		if (front == 0) rear = 0;
		delete temp;
	}
	else
		data = 0;
	
	PCB::unlock();
	
	return data;
}

Queue::~Queue()
{
	PCB::lock();
	
	Node* temp;
	while (front)
	{
		temp = front;
		front = front->next;
		delete temp;
	}
	
	PCB::unlock();
}