#pragma once
#include "ConcuurentQueue.h"

/*----------------
	GlobalQueue
-----------------*/

class GlobalQueue
{
public:
	GlobalQueue();
	~GlobalQueue();

	void					Push(JobQueueRef jobQueue);
	JobQueueRef				Pop();

private:
	ConcuurentQueue<JobQueueRef> _jobQueues;
};

