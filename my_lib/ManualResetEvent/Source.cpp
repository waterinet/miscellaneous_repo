#include <iostream>
#include "ManualResetEvent.h"
#include "boost/bind.hpp"
#include "boost/ref.hpp"

using namespace std;

void set_string(ManualResetEvent& mre, string& str)
{
	str = "set, hhhhhhh";
	boost::this_thread::sleep_for(boost::chrono::seconds(5));
	mre.set();
}

void main()
{
	string str;
	ManualResetEvent mre;
	boost::thread t(boost::bind(set_string, boost::ref(mre), boost::ref(str)));
	mre.wait();
	cout << str << "\n";
}