#include "HttpClient.h"
#include "boost/format.hpp"

using namespace std;
using namespace boost;

HttpClient::HttpClient(boost::asio::io_service &ios, std::vector<ServerInfo> endpoint, string msg_id) :
	socket_(ios),
	deadline_(ios),
	endpoints_(endpoint),
	stopped_(false),
	content_length_(0),
	message_id_(msg_id)
{
}


HttpClient::~HttpClient(void)
{
}


void HttpClient::start()
{
	stopped_ = false;
	endpoint_iter iter = endpoints_.begin();
	start_connect(iter);
	deadline_.async_wait(boost::bind(&HttpClient::check_deadline, shared_from_this(), _1));
}

void HttpClient::stop()
{
	stopped_ = true;
	boost::system::error_code ignored_ec;
	socket_.close(ignored_ec);
	deadline_.cancel(ignored_ec);
}

void HttpClient::start_connect(endpoint_iter iter)
{
	if (iter != endpoints_.end())
	{
		deadline_.expires_from_now(boost::posix_time::seconds(iter->conn_timeout));
		socket_.async_connect(iter->endpoint, 
			boost::bind(&HttpClient::handle_connect, shared_from_this(), iter, _1));
	}
	else
	{
		stop();
		signal_result("fail to connect to server");
	}
}

void HttpClient::handle_connect(endpoint_iter iter, const boost::system::error_code &ec)
{
	if (stopped_)
		return;
	//连接超时
	if (!socket_.is_open())
	{
		start_connect(++iter);
	}
	//超时之前出错
	else if (ec)
	{
		boost::system::error_code ignored_ec;
		socket_.close(ignored_ec);
		start_connect(++iter);
	}
	//连接成功
	else
	{
		remote_endpoint_ = iter->endpoint;
		request_sig(request_, remote_endpoint_.address().to_string(), remote_endpoint_.port());
		deadline_.expires_from_now(boost::posix_time::seconds(iter->transfer_timeout));
		boost::asio::async_write(socket_, request_, 
			boost::bind(&HttpClient::handle_write_request, shared_from_this(), _1));
	}
}

void HttpClient::handle_write_request(const boost::system::error_code &ec)
{
	if (stopped_)
		return;
	if (ec)
	{
		stop();
		signal_result("handle_write_request: " + ec.message());
	}
	else
	{
		boost::asio::async_read_until(socket_, response_, "\r\n",
			boost::bind(&HttpClient::handle_read_status_line, shared_from_this(), _1));
	}
}

void HttpClient::handle_read_status_line(const boost::system::error_code &ec)
{
	if (stopped_)
		return;
	if (ec)
	{
		stop();
		signal_result("handle_read_status_line: " + ec.message());
	}
	else
	{
		string http_version;
		unsigned int status_code;
		http_response_status(http_version, status_code);
		if (http_version.substr(0, 5) != "HTTP/")
		{
			stop();
			signal_result("Invalide response");
			return;
		}
		if (status_code != 200)
		{
			stop();
			signal_result(boost::str(boost::format("Response with status code %1%") % status_code));
			return;
		}
		boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
			boost::bind(&HttpClient::handle_read_headers, shared_from_this(), _1));
	}
}

void HttpClient::handle_read_headers(const boost::system::error_code &ec)
{
	if (stopped_)
		return;
	if (ec)
	{
		stop();
		signal_result("handle_read_headers:" + ec.message());
	}
	else
	{
		string transfer_encoding;
		size_t content_length = 0;
		http_headers(transfer_encoding, content_length);
		//优先根据transfer_encoding来获取内容长度
		if (transfer_encoding == "chunked")
		{
			boost::asio::async_read_until(socket_, response_, "\r\n",
				boost::bind(&HttpClient::handle_read_chunk_size, shared_from_this(), _1));
		}
		else if (content_length > 0)
		{
			if (response_.size() > 0)
			{
				HttpBuffer buffer(response_.size());
				size_t count = http_content(buffer);
				content_.push_back(buffer);
				content_length_ += count;
			}
			boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(1), 
				boost::bind(&HttpClient::handle_read_content, shared_from_this(), content_length, _1));
		}
		else
		{
			stop();
			signal_result("handle_read_headers: empty content");
		}
	}
}

void HttpClient::handle_read_content(size_t content_length, const boost::system::error_code &ec)
{
	if (stopped_)
		return;
	if (ec)
	{
		if (ec == boost::asio::error::eof)
		{
			stop();
			signal_result(); //接收完成
		}
		else
		{
			stop();
			signal_result("handle_read_content: " + ec.message());
		}
	}
	else
	{
		HttpBuffer buffer(response_.size());
		size_t count = http_content(buffer);
		content_.push_back(buffer);
		content_length_ += count;
		boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(1), 
			boost::bind(&HttpClient::handle_read_content, shared_from_this(), content_length, _1));
	}
}

void HttpClient::handle_read_chunk_size(const boost::system::error_code &ec)
{
	if (stopped_)
		return;
	if (ec)
	{
		stop();
		signal_result("handle_read_chunk_size: " + ec.message());
	}
	else
	{
		size_t chunk_size = http_chunk_size();
		if (chunk_size > 0)
		{
			//chunk data之后包含\r\n
			int diff = chunk_size + 2 - response_.size();
			size_t remaining = diff > 0 ? diff : 0;
			boost::asio::async_read(socket_, response_, boost::asio::transfer_exactly(remaining),
				boost::bind(&HttpClient::handle_read_chunk_data, shared_from_this(), chunk_size, _1));
		}
		else
		{
			stop();
			signal_result(); //接收完成
		}
	}
}

void HttpClient::handle_read_chunk_data(size_t chunk_size, const boost::system::error_code &ec)
{
	if (stopped_)
		return;
	if (ec)
	{
		stop();
		signal_result("handle_read_chunk_data: " + ec.message());
	}
	else
	{
		HttpBuffer buffer(chunk_size);
		size_t count = http_chunk_data(buffer);
		if (count != chunk_size)
		{
			stop();
			signal_result("fail to read chunk data");
			return;
		}
		content_.push_back(buffer);
		content_length_ += count;
		boost::asio::async_read_until(socket_, response_, "\r\n",
			boost::bind(&HttpClient::handle_read_chunk_size, shared_from_this(), _1));
	}
}

void HttpClient::http_response_status(std::string &http_version, unsigned int &status_code)
{
	std::istream response_stream(&response_);
	response_stream >> http_version;
	response_stream >> status_code;
	string status_message;
	//从stream中提取第一行剩下的信息，包含回车和换行
	getline(response_stream, status_message);
}

void HttpClient::http_headers(string &transfer_encoding, size_t &length)
{
	istream response_stream(&response_);
	string line;
	while (getline(response_stream, line) && line != "\r")
	{
		string::size_type pos = line.find("Transfer-Encoding");
		string dump;
		if (pos != string::npos)
		{
			istringstream header_stream(line);
			header_stream >> dump >> transfer_encoding;
			continue;
		}
		pos = line.find("Content-Length");
		if (pos != string::npos)
		{
			istringstream header_stream(line);
			header_stream >> dump >> length;
		}
	}
}

size_t HttpClient::http_chunk_size()
{
	istream response_stream(&response_);
	string line;
	getline(response_stream, line);
	size_t chunk_size = 0;
	istringstream str_stream(line);
	str_stream >> hex >> chunk_size;
	return chunk_size;
}

size_t HttpClient::http_chunk_data(HttpBuffer &buffer)
{
	istream response_stream(&response_);
	response_stream.read(buffer.get(), buffer.size);
	//实际读取的字节数
	size_t count = (size_t)response_stream.gcount();
	string dump;
	//从response中去除换行
	getline(response_stream, dump);
	return count;
}

size_t HttpClient::http_content(HttpBuffer &buffer)
{
	istream response_stream(&response_);
	response_stream.read(buffer.get(), buffer.size);
	size_t count = (size_t)response_stream.gcount();
	return count;
}

void HttpClient::check_deadline(const boost::system::error_code& ec)
{
	if (stopped_)
		return;
    bool timedout = false;
	//检查是否超时
	if (deadline_.expires_at() <= deadline_timer::traits_type::now())
	{
		timedout = true;
		//取消定时器直到更新超时时间
		deadline_.expires_at(boost::posix_time::pos_infin);
	}
	deadline_.async_wait(boost::bind(&HttpClient::check_deadline, shared_from_this(), _1));
	if (timedout)
	{
		//取消所有未完成的异步操作
		//确保deadline_timer的所有更新操作在后续的cancel操作之前完成
		boost::system::error_code ignored_ec;
		socket_.close(ignored_ec);
	}
}

void HttpClient::add_request_builder(request_signal::slot_type slot)
{
	request_sig.connect(slot);
}

void HttpClient::add_result_handler(result_signal::slot_type slot)
{
	result_sig.connect(slot);
}

void HttpClient::signal_result()
{
	result_sig(message_id_, remote_endpoint_, content_, content_length_, "");
}

void HttpClient::signal_result(std::string err_msg)
{
	err_msg_ = err_msg;
	result_sig(message_id_, remote_endpoint_, content_, content_length_, err_msg_);
}


