#include "ManualResetEvent.h"

ManualResetEvent::ManualResetEvent(bool signaled) :
	signaled_(signaled)
{
}


ManualResetEvent::~ManualResetEvent(void)
{
}

void ManualResetEvent::set()
{
	{
		boost::lock_guard<boost::mutex> lock(mtx_);
		signaled_ = true;
	}
	cond_.notify_one();
}

void ManualResetEvent::reset()
{
	boost::lock_guard<boost::mutex> lock(mtx_);
	signaled_ = false;
}

//wait will atomically add the thread to the set of threads waiting on 
//the condition variable, and unlock the mutex. When the thread is woken, 
//the mutex will be locked again before the call to wait returns.
void ManualResetEvent::wait()
{
	boost::unique_lock<boost::mutex> lock(mtx_);
	while (!signaled_)
	{
		cond_.wait(lock);
	}
}

//use absolute time to avoid "spurious wake" of a condition variable
bool ManualResetEvent::wait(int timeout)
{
	boost::unique_lock<boost::mutex> lock(mtx_);
	boost::system_time abs_time = 
		boost::get_system_time() + boost::posix_time::milliseconds(timeout);
	if (cond_.timed_wait(lock, abs_time, boost::bind(&ManualResetEvent::is_signaled, this)))
		return true;
	return false;
}

bool ManualResetEvent::is_signaled()
{
	return signaled_;
}