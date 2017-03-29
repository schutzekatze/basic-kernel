#include "list.h"

#include "pcb.h"

void List::insert(void* data)
{
	PCB::lock();
	
	List::Node* node = new Node(data, 0, first);
	if (first) first->prev = node;
	first = node;
	
	PCB::unlock();
}

void List::insertBefore(void* data, Node* node)
{
	PCB::lock();
	
	Node* newNode = new Node(data, node->prev, node);
	
	if (node->prev) node->prev->next = newNode;
	node->prev = newNode;
	
	if (node == first) first = newNode;
	
	PCB::unlock();
}

void List::insertAfter (void* data, Node* node)
{
	PCB::lock();
	
	Node* newNode = new Node(data, node, node->next);
	
	if (node->next) node->next->prev = newNode;
	node->next = newNode;
	
	PCB::unlock();
}

void List::remove(Node* node)
{
	PCB::lock();
	
	if (first == node) first = first->next;
	
	if (node->prev) node->prev->next = node->next;
	if (node->next) node->next->prev = node->prev;
	
	delete node;
	
	PCB::unlock();
}

List::~List()
{
	PCB::lock();
	
	Node* temp;
	while (first)
	{
		temp = first;
		first = first->next;
		delete temp;
	}
	
	PCB::unlock();
}