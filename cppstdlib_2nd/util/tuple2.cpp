#include <tuple>
#include <iostream>
#include <functional>
#include <string>

using namespace std;

int main() 
{
    tuple<int,float,string> t(77, 1.1, "more light");
	int i;
	float f;
	string s;
    make_tuple(ref(i), ref(f), ref(s)) = t;
	cout << i << " " << f << " " << s << endl;

	std::get<0>(t) = 88;
	std::get<2>(t) = "more more light";
	std::tie(i, std::ignore, s) = t;
	cout << i << " " << f << " " << s << endl;

}
