#ifndef _list_h_
#define _list_h_

//*Threadsafe*

class List
{
public:

	struct Node
	{
		void* data;
		Node *prev, *next;
		Node(void* data_, Node *prev_ = 0, Node *next_ = 0): data(data_), prev(prev_), next(next_) {}
	};

	List(): first(0) {}
	~List();

	void insert(void* data);
	void insertBefore(void* data, Node* node);
	void insertAfter (void* data, Node* node);
	void remove(Node* node);
	
	Node *first;
};

#endif