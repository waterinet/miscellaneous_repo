#include <iostream>
#include <utility>

using namespace std;

int main() 
{
    pair<int, float> p(42, 3.14);
	int i = std::get<0>(p);
	float f = std::get<1>(p);
	int size = std::tuple_size< pair<int, float> >::value;
	std::tuple_element<0, pair<int, float> >::type j = 43;

	cout << "[" << i << "," << f << "]" << endl;
	cout << "size: " << size << endl;
	cout << "j: " << j << endl;
}
