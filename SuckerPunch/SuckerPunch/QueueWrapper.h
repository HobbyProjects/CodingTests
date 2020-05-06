#pragma once

#include "Queue.h"
#include "Q.h"

#include <vector>
/* 
* Wrapper test class that ensures that every new test gets a new context.
* Hence, every unit test can run independently of each other.
*/

class QueueWrapper
{
private:
	std::vector<Q*> m_currentInstances;
public:
	QueueWrapper();
	~QueueWrapper();
	Q* _create_queue();			//Creates a FIFO byte queue, returning a handle to it.
	void _destroy_queue(Q* q);	//Destroy an earlier created byte queue.
	void _enqueue_byte(Q* q, unsigned char b);	//Adds a new byte to a queue.
	unsigned char _dequeue_byte(Q* q);			//Pops the next byte off the FIFO queue.
};


