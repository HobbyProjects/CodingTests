#include "stdafx.h"
#include "Q.h"
#include <cmath>
#include <cstdio>

//array alias for queues
static Q* const queues = (Q*)&data[0];	//we use the memory area [0..63], first 64 bytes

//array alias for queued bytes
static const unsigned int remaining_space = data_size - Q::max_bytesize;
static const unsigned int max_queued_byte_count = remaining_space / sizeof(queued_byte);
static queued_byte* const queued_bytes = (queued_byte*)&data[Q::max_bytesize];	//we use the remaining data for storing the data

//queued_byte::functions implementation
queued_byte* const queued_byte::begin()
{
	return &queued_bytes[0];
}

queued_byte* const queued_byte::end()
{
	return &queued_bytes[max_queued_byte_count];
}

queued_byte* const queued_byte::at(unsigned int idx)
{
	//assert_IllegalOp(queued_byte::in_valid_range(idx));
	if (queued_byte::in_valid_range(idx))
		return &queued_bytes[idx];
	return NULL;
}

bool queued_byte::in_valid_range(queued_byte* qb)
{
	queued_byte* qbstart = queued_byte::begin();
	return int(qb - qbstart) < max_queued_byte_count;
}

bool queued_byte::in_valid_range(unsigned int idx)
{
	return idx < max_queued_byte_count;
}

bool queued_byte::memory_available()
{
	return current_count < max_queued_byte_count;
}

void queued_byte::invalidate()
{
	memset(this, BAD_VALUE, sizeof(queued_byte));
}


// Q::functions implementation
Q* Q::create()
{
	for (Q* q_1st_free = Q::begin(); q_1st_free != Q::end(); ++q_1st_free)
	{
		if (q_1st_free->start_offset == BAD_QUEUE)
			return q_1st_free;
	}
	//assert_OutOfMemory(false);	//out of queues
	return NULL;
}

void Q::destroy(Q* q)
{
	//assert_IllegalOp(q != NULL);
	//assert_IllegalOp(q->in_valid_range());
	//assert_IllegalOp(q->is_valid());

	q->destroy_queued_bytes();

	q->start_offset = BAD_QUEUE;
	q->length = BAD_QUEUE;
}

Q* const Q::begin()
{
	return &queues[0];
}

Q* const Q::end()
{
	return &queues[Q::max_count];
}


queued_byte* const Q::queued_bytes_begin()
{
	return &queued_bytes[start_offset];
}

queued_byte* const Q::queued_bytes_end()
{
	return &queued_bytes[start_offset + length];
}


bool Q::in_valid_range()
{
	Q* qstart = Q::begin();
	return int(this - qstart) < Q::max_bytesize;
}

bool Q::at_least_one_exists()
{
	for (Q* q = Q::begin(); q != Q::end(); ++q)
	{
		if (q->start_offset != BAD_VALUE)
			return true;
	}
	return false;
}

uShort Q::get_start_offset(Q* q)
{
	unsigned int qIdx = (q - Q::begin());
	//qIdx /= sizeof(Q);
	return qIdx * Q::current_max_length;
}

void Q::destroy_queued_bytes()
{
	//assert_IllegalOp(length_is_valid());

	for (queued_byte* qb = queued_bytes_begin();
		qb != queued_bytes_end();
		++qb)
	{
		qb->invalidate();
		--queued_byte::current_count;
	}
}

void Q::enqueue_byte(byteType b)
{
	//assert_OutOfMemory(queued_byte::memory_available());

	bound_check_and_memory_rearrange();

	queued_byte* qb = queued_bytes_begin() + length;
	qb->value = b;
	++length;
	++queued_byte::current_count;
}

byteType Q::dequeue_byte()
{
	queued_byte* qb = queued_bytes_begin();
	byteType b = qb->value;

	shift_left_queued_bytes();
	--length;
	--queued_byte::current_count;
	return b;
}

bool Q::is_valid()
{
	return length_is_valid() && offset_is_valid();
}

bool Q::length_is_valid()
{
	return length != BAD_QUEUE;
}

bool Q::offset_is_valid()
{
	return start_offset != BAD_QUEUE;
}

uShort Q::get_queued_bytes_data_size()
{
	return uShort(length * sizeof(queued_byte));
}


void Q::move_queued_bytes(queued_byte* const targetBuffer)
{
#define tempBufferSize 20u
	queued_byte buffer[tempBufferSize];

	for (int i = 0; i < length; i += tempBufferSize)
	{
		unsigned int copySize = MIN(tempBufferSize, (unsigned int)(length - i));
		queued_byte* origin = queued_bytes_begin() + i;

		memcpy(buffer, origin, copySize);
		memcpy(targetBuffer + i, buffer, copySize);
		memset(origin, BAD_VALUE, copySize);
	}
}


void Q::shift_left_queued_bytes()
{
	queued_byte* qb_target = queued_bytes_begin();
	for (queued_byte* qb = qb_target + 1;
		qb != queued_bytes_end();
		++qb)
	{
		memcpy(qb_target, qb, sizeof(queued_byte));

		qb->invalidate();
		++qb_target;
	}
}

// magic happens in here

static uShort memory_used_or_reserved()
{
	uShort sum = 0;
	for (Q* q = Q::begin(); q != Q::end(); ++q)
	{
		if (q->is_valid())
			sum += MAX(q->length, Q::current_max_length);
	}
	return sum;
}

static uShort memory_used()
{
	uShort sum = 0;
	for (Q* q = Q::begin(); q != Q::end(); ++q)
	{
		if (q->is_valid())
			sum += q->length;
	}
	return sum;
}

void Q::bound_check_and_memory_rearrange()
{
	uShort used_queued_byte_count = memory_used();
	//assert_OutOfMemory(queued_byte::current_count + 1 <= max_queued_byte_count);
	//assert_IllegalOp(queued_byte::current_count == used_queued_byte_count);	//something went VERY wrong if this one triggers

	uShort used_or_reserved_queued_byte_count = memory_used_or_reserved();
	uShort new_used_queued_byte_count = used_queued_byte_count + 1;
	uShort possible_queue_count = max_queued_byte_count / Q::current_max_length;

	static uShort last_Q_count = Q::current_count;
	bool Q_count_differs = last_Q_count != Q::current_count;
	last_Q_count = Q::current_count;

	if ((length + 1 <= Q::current_max_length)
		&& (new_used_queued_byte_count <= max_queued_byte_count)
		&& !(Q_count_differs
			&& Q::current_count > possible_queue_count)
		)
	{
		return;	//nothing to do
	}

	//move Qs to new arrangement
	// be it right to make place for longer Q
	// or left for shorter/more optimal Qs
	{
		uShort current_reserved_queued_byte_count = Q::current_count * Q::current_max_length;
		short resize_opt = short(floor((float(max_queued_byte_count) - float(used_or_reserved_queued_byte_count + 1)) / float(Q::current_count)));
		uShort optimal_queue_max_length = Q::current_max_length + resize_opt;

		//assert_IllegalOp(optimal_queue_max_length * Q::current_count <= max_queued_byte_count);

		//changing Q arrangement by changing the max length (by it bigger or smaller)
		Q::current_max_length = optimal_queue_max_length;

		//we copy the contents of the Qs into a temp buffer with the new alignment to avoid overwriting data.
		//allocating more data than needed to avoid being out of memory (should not happen though)
		byteType tempCopy[4096];
		memset(tempCopy, BAD_VALUE, 4096);

		//copy data to new arrangement (in tempBuffer to avoid overwriting)
		uShort cumulOffsets = 0;
		uShort newOffsets[64] = { BAD_QUEUE };
		uShort* current_Q_new_offset = &newOffsets[0];
		for (Q* q = Q::begin(); q != Q::end(); ++q)
		{
			if (q->is_valid())
			{
				*current_Q_new_offset = cumulOffsets;
				//assert_IllegalOp(*current_Q_new_offset < max_queued_byte_count);
				memcpy(&tempCopy[*current_Q_new_offset], q->queued_bytes_begin(), q->get_queued_bytes_data_size());
				memset(q->queued_bytes_begin(), BAD_VALUE, q->get_queued_bytes_data_size());

				cumulOffsets += MAX(Q::current_max_length, q->get_queued_bytes_data_size());
				//assert_IllegalOp(cumulOffsets <= max_queued_byte_count);
			}
			++current_Q_new_offset;
		}

		//copy newly arranged data back
		current_Q_new_offset = &newOffsets[0];
		for (Q* q = Q::begin(); q != Q::end(); ++q)
		{
			if (q->is_valid())
			{
				//assert_IllegalOp(*current_Q_new_offset != BAD_QUEUE);	//should not happen
				q->start_offset = *current_Q_new_offset;
				memcpy(q->queued_bytes_begin(), &tempCopy[*current_Q_new_offset], q->get_queued_bytes_data_size());
			}
			++current_Q_new_offset;
		}
	}
}

