#ifndef Q_h
#define Q_h

#include <cstring>

typedef unsigned char	byteType;
typedef unsigned short	uShort;

//min/max. not using std for it
template<typename T>
T MIN(T a, T b)
{
	return a < b ? a : b;
}

template<typename T>
T MAX(T a, T b)
{
	return a > b ? a : b;
}

/* error handling
void on_out_of_memory()
{
	printf("out of memory\n");
	while (true)
	{
		;
	}
}

void on_illegal_operation()
{
	printf("illegal operation\n");
	while (true)
	{
		;
	}

}
//assert functions
static void assert_IllegalOp(bool cond)
{
	if (!cond)
		on_illegal_operation();
}

static void assert_OutOfMemory(bool cond)
{
	if (!cond)
		on_out_of_memory();
}*/

//overall available data
static const unsigned int data_size = 2048;
static byteType data[data_size];	//queue heap
									//data needs to be initialized once to BAD_DATA
//tag for uninitialized queues and data
static const byteType	BAD_VALUE = 0xFF;
static const uShort		BAD_QUEUE = 0xFFFF;

struct queued_byte
{
	byteType value;
	static uShort current_count;

	static queued_byte* const begin();
	static queued_byte* const end();
	static queued_byte* const at(unsigned int idx);

	static bool in_valid_range(queued_byte* qb);
	static bool in_valid_range(unsigned int idx);
	static bool memory_available();

	void invalidate();
};

class Q
{
public:
	uShort start_offset;	//can exceed 255
	uShort length;			//can exceed 255

	static uShort current_count;
	static const uShort max_count;

	static uShort current_max_length;
	static const uShort max_bytesize;
public:
	//API replication
	static Q* create();
	static void destroy(Q* q);

	void enqueue_byte(byteType b);
	byteType dequeue_byte();

	//queued bytes iterating
	queued_byte* const queued_bytes_begin();
	queued_byte* const queued_bytes_end();
	queued_byte* const queued_byte_at(unsigned int idx);

	//Q iterating
	static Q* const begin();
	static Q* const end();
	static uShort get_start_offset(Q* q);

	//conditions to check for data validity
	static bool at_least_one_exists();
	bool in_valid_range();
	bool is_valid();
	bool length_is_valid();
	bool offset_is_valid();

	uShort get_queued_bytes_data_size();

protected:
	void destroy_queued_bytes();
	void move_queued_bytes(queued_byte* const targetBuffer);
	void shift_left_queued_bytes();

	void bound_check_and_memory_rearrange();
};

#endif