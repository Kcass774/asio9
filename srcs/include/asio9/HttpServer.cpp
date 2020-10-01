#ifndef ASIO9_HTTPSERVER_CPP_
#define ASIO9_HTTPSERVER_CPP_

#include "HttpServer.h"

void asio9::HttpConnection::read_handler(basic_type::ec_type ec, size_t bytes_transferred, basic_type::req_ptr req)
{
	if (this->m_MessageCallBack)
		this->m_MessageCallBack(ec, req, this->shared_from_this());

	//处理下一个请求
	if (!ec)
		do_read();
}

void asio9::HttpConnection::write_handler(basic_type::ec_type ec, size_t bytes_transferred, WriteCallBack callback)
{
	if (callback)
	{
		callback(ec, bytes_transferred);
	}
}

asio9::HttpServer::HttpServer(basic_type::io_type* io, const basic_type::endpoint_type& endpoint)
	:m_acceptor(boost::asio::make_strand(*io))
{
	this->m_acceptor.open(endpoint.protocol());
	this->m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	this->m_acceptor.bind(endpoint);
	this->m_acceptor.listen();

	this->m_io = io;
}

void asio9::HttpServer::on_accept(boost::beast::error_code ec, basic_type::socket_type socket)
{
	if (ec) {
		if (this->m_NewConnectedCallBack)
			this->m_NewConnectedCallBack(ec, nullptr);
	}
	else
	{
		auto conn = std::make_shared<HttpConnection>(std::move(socket), this->m_io);
		if (this->m_NewConnectedCallBack)
			this->m_NewConnectedCallBack(ec, conn);
		conn->run();
	}
	this->do_accept();
}

#endif // !ASIO9_HTTPSERVER_H_