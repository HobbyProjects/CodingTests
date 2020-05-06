#include "pch.h"
#include "CppUnitTest.h"
//#include "../SuckerPunch/Queue.cpp"
//#include "../SuckerPunch/Q.cpp"
//#include "../SuckerPunch/QueueWrapper.cpp"


#define MAX_NUMBER_OF_QUEUES 64
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(UnitTests)
	{
	public:
		
		TEST_METHOD(CanCreateAQueue)
		{
			/*QueueWrapper* qw = new QueueWrapper();
			Q* q1 = qw->_create_queue();
			Q* q0 = create_queue();

			Assert::IsNotNull(q0);
			Assert::IsNotNull(q1);*/
		}

		TEST_METHOD(CanCreateMultipleQueues)
		{
			/*for (uShort counter = 0; counter < uShort(MAX_NUMBER_OF_QUEUES); counter ++)
			{
				//Q* q0 = create_queue();
				//Assert::IsNotNull(q0);
			}*/
		}
	};
}
