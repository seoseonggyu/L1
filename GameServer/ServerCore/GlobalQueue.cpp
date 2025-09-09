#include "pch.h"
#include "GlobalQueue.h"

/*----------------
	GlobalQueue
-----------------*/

GlobalQueue::GlobalQueue()
{

}

GlobalQueue::~GlobalQueue()
{

}

void GlobalQueue::Push(JobQueueRef jobQueue)
{
	_jobQueues.Push(jobQueue);
}

JobQueueRef GlobalQueue::Pop()
{
	JobQueueRef Out{ nullptr };
	_jobQueues.Pop(Out);
	return Out;
}
