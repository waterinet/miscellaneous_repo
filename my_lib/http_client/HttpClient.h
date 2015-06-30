#pragma once
#include "boost/asio.hpp"
#include "boost/shared_array.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/signals2.hpp"

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

struct ServerInfo
{
	tcp::endpoint endpoint;
	int conn_timeout;
	int transfer_timeout;
};

struct HttpBuffer
{
	boost::shared_array<char> buffer;
	size_t size;
	HttpBuffer(size_t s) : size(s)
	{
		buffer = boost::shared_array<char>(new char[size]);
	}
	char *get() const 
	{ 
		return buffer.get(); 
	}
};

//继承enable_shared_from_this，向外部的io_service传递shared_ptr
class HttpClient : public boost::enable_shared_from_this<HttpClient>
{
	typedef std::vector<ServerInfo>::iterator endpoint_iter;
	typedef boost::signals2::signal<void (boost::asio::streambuf&, std::string, unsigned short)> request_signal;
	typedef boost::signals2::signal<void (std::string, tcp::endpoint, std::vector<HttpBuffer>, size_t, std::string)> result_signal;
public:
	HttpClient(boost::asio::io_service&, std::vector<ServerInfo>, std::string msg_id);
	~HttpClient(void);
	void start();
	void stop();
	void add_request_builder(request_signal::slot_type slot);
	void add_result_handler(result_signal::slot_type slot);
private:
	void start_connect(endpoint_iter iter);
	void handle_connect(endpoint_iter iter, const boost::system::error_code &ec);
	void handle_write_request(const boost::system::error_code&);
	void handle_read_status_line(const boost::system::error_code&);
	void handle_read_headers(const boost::system::error_code&);
	void handle_read_content(size_t, const boost::system::error_code&);
	void handle_read_chunk_size(const boost::system::error_code&);
	void handle_read_chunk_data(size_t, const boost::system::error_code&);
	void check_deadline(const boost::system::error_code&);
	void http_response_status(std::string &http_version, unsigned int &status_code);
	void http_headers(std::string &transfer_encoding, size_t &length);
	size_t http_chunk_size();
	size_t http_chunk_data(HttpBuffer&);
	size_t http_content(HttpBuffer&);
	void signal_result();
	void signal_result(std::string);
private:
	tcp::socket socket_;
	boost::asio::streambuf request_;
	boost::asio::streambuf response_;
	deadline_timer deadline_;
	bool stopped_;
	std::vector<ServerInfo> endpoints_;
	std::string message_id_; 
	tcp::endpoint remote_endpoint_;
	std::vector<HttpBuffer> content_;  //存储接收到的数据 
	size_t content_length_;            //数据总字节数
	std::string err_msg_;              //错误信息
	request_signal request_sig;
	result_signal result_sig;
};
