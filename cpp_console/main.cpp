#include <iostream>
#include "time_accumulator.h"
#include "stime.h"
#include "sstring.h"
#include "stree.h"

using namespace std;

int main()
{
    typedef pair<int, string> node_type;
    
	vector<node_type> vec;
	vec.push_back(node_type(0, "pgrlink"));
	vec.push_back(node_type(1, "link init"));
    vec.push_back(node_type(2, "link analyze"));
	vec.push_back(node_type(3, "load aa files"));
    vec.push_back(node_type(4, "link model"));
	vec.push_back(node_type(5, "link check"));
	vec.push_back(node_type(6, "load ao files"));
	vec.push_back(node_type(7, "sn check"));
	vec.push_back(node_type(8, "pm chech"));
	vec.push_back(node_type(9, "Extraction_TypeNameLink"));
	vec.push_back(node_type(10, "Extraction_TagNameLink"));
	vec.push_back(node_type(11, "Extraction_ObjectLink"));
	vec.push_back(node_type(12, "GetAppearNodeInfotomap"));
	vec.push_back(node_type(13, "CheckNouseObj"));
	vec.push_back(node_type(14, "CheckObjectUsedinParallelFunction"));
    vec.push_back(node_type(15, "link display"));

    pgr::stree_helper<int, string> helper;
    helper.add_nodes(vec);
    helper.add_pos_info(0, pgr::array_as_vector<int>(4, 1, 2, 5, 15));
    helper.add_pos_info(2, pgr::array_as_vector<int>(2, 3, 4));
    helper.add_pos_info(5, pgr::array_as_vector<int>(3, 6, 7, 8));
    helper.add_pos_info(8, pgr::array_as_vector<int>(3, 9, 10, 11));
    helper.add_pos_info(11, pgr::array_as_vector<int>(3, 12, 13, 14));
    pgr::stree<int, string> tree;
    tree.build(helper);
    tree.print();

    return 0;
}
