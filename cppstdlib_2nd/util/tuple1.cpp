#include <tuple>
#include <iostream>
#include <complex>
#include <string>

using namespace std;

int main()
{
	// elements are initialized with defaut value
	tuple<string,int,int,complex<double>> t;

	tuple<int,float,string> t1(41, 6.3, "nico");
	cout << get<1>(t1) << endl;

	auto t2 = make_tuple(22, 44, "nico");
    get<1>(t1) = get<1>(t2);
    cout << get<1>(t1) << endl;

	// comparision and assignment
	// implicit type conversion
	if (t1 < t2) {
		t1 = t2;
	}
}
