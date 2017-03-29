#ifndef _queue_h_
#define _queue_h_

//*Threadsafe*

class Queue
{
public:

	struct Node
	{
		void* data;
		Node *next;
		Node(void* data_): data(data_), next(0) {}
	};

	Queue(): front(0), rear(0) {}
	~Queue();

	void insert(void* data);
	void* remove();
	char empty() const;
	
	Node *front, *rear;
};

inline char Queue::empty() const
{
	return front == 0;
}

#endif