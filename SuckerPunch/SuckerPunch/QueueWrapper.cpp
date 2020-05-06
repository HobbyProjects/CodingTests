#include "stdafx.h"
#include "QueueWrapper.h"
#include "Q.cpp"
#include "Queue.cpp"

QueueWrapper::~QueueWrapper()
{
	if (!m_currentInstances.empty())
	{
		for (Q* q : m_currentInstances)
		{
			destroy_queue(q);
		}
	}
}

Q* QueueWrapper::_create_queue()
{
	Q* q = create_queue();
	m_currentInstances.push_back(q);

	return q;
}

void QueueWrapper::_destroy_queue(Q* q)
{
	std::vector<Q*>::iterator it;
	it = std::find(m_currentInstances.begin(), m_currentInstances.end(), q);
	if (it != m_currentInstances.end())
	{
		m_currentInstances.erase(it);
	}
	destroy_queue(q);
}

unsigned char QueueWrapper::_dequeue_byte(Q* q) 
{
	return dequeue_byte(q);
}

void QueueWrapper::_enqueue_byte(Q* q, unsigned char b)
{
	enqueue_byte(q, b);
}