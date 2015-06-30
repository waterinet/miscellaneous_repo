#pragma once
#include "boost/thread.hpp"

class ManualResetEvent : public boost::noncopyable
{
public:
	ManualResetEvent(bool=false);
	~ManualResetEvent(void);
	void set();
	void reset();
	void wait();
	bool wait(int);
private:
	bool is_signaled();
private:
	bool signaled_;
	boost::mutex mtx_;
	boost::condition_variable cond_;
};

