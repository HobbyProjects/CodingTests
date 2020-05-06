//
//  Queue.cpp
//  SuckerPunch
//
//  Created by Christian Helmich on 22.04.12.
//  Copyright (c) 2012 Christian Helmich. All rights reserved.
//

#include "stdafx.h"
#include "Queue.h"


typedef unsigned char	byteType;
typedef unsigned short	uShort;

uShort queued_byte::current_count = 0;

uShort Q::current_count = 0;
const uShort Q::max_count = 64;

uShort Q::current_max_length = 80;
const uShort Q::max_bytesize = Q::max_count * sizeof(Q);

static void initializeData()
{
	static bool once = false;
	if(once)
		return;
	
	memset(data, BAD_VALUE, data_size);
	once = true;
}

// "API" implementation

Q* create_queue()
{
	initializeData();
	Q* q = Q::create();
	//assert_IllegalOp(q != NULL);	
	//assert_IllegalOp(q->in_valid_range());
	
	++Q::current_count;
	
	q->start_offset = Q::get_start_offset(q);
	q->length = 0;
	
	return q;
}


void destroy_queue(Q* q)
{
	//	printf("destroying Q [0x%p] with id: %i \n", q, *q);
	Q::destroy(q);
	
	--Q::current_count;
}


void enqueue_byte(Q* q, unsigned char b)
{
	//assert_IllegalOp(q != NULL);
	//assert_IllegalOp(q->is_valid());
	
	q->enqueue_byte(b);
}


unsigned char dequeue_byte(Q* q)
{
	//assert_IllegalOp(q != NULL);
	//assert_IllegalOp(q->is_valid());

	return q->dequeue_byte();
}