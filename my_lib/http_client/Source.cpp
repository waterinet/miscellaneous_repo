#include <iostream>
#include <vector>
#include "HttpClient.h"
#include "boost/bind.hpp"
#include "boost/shared_ptr.hpp"

using namespace std;
using boost::asio::ip::address;

void build_request(boost::asio::streambuf& request, string ip, unsigned short port,
	string path, string content)
{
	std::ostream request_stream(&request);
	request_stream << "POST " << path << " HTTP/1.1\r\n";
	request_stream << "Host: " << ip << "\r\n";
	request_stream << "Connection: close\r\n";
	request_stream << "Content-Length: " << content.length() << "\r\n";
	request_stream << "\r\n";
	request_stream.write(content.data(), content.length());
}

void handle_result(string task_id, tcp::endpoint endpoint, vector<HttpBuffer> buffer,
	size_t buffer_size, std::string err_msg)
{
	if (!err_msg.empty()) {
		cout << err_msg << "\n";
	}
}

void main()
{
	boost::asio::io_service ios;
	//boost::asio::io_service::work work(ios);
	tcp::endpoint ep(address::from_string("192.168.1.129"), 8080);
	ServerInfo ser{ ep, 3, 10 };
	vector<ServerInfo> vec{ ser };
	boost::shared_ptr<HttpClient> conn_ptr(new HttpClient(ios, vec, "001"));
	conn_ptr->add_request_builder(
		boost::bind(build_request, _1, _2, _3, "", ""));
	conn_ptr->add_result_handler(
		boost::bind(handle_result, _1, _2, _3, _4, _5));
	conn_ptr->start();
	ios.run();
}