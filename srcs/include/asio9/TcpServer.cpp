#ifndef ASIO9_TCPSERVER_CPP_
#define ASIO9_TCPSERVER_CPP_

#include "TcpServer.h"

asio9::TcpServer::TcpServer(basic_type::io_type* io, const basic_type::endpoint_type& endpoint)
	:m_acceptor(*io)
{
	this->m_acceptor.open(endpoint.protocol());
	this->m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	this->m_acceptor.bind(endpoint);
	this->m_acceptor.listen(boost::asio::socket_base::max_listen_connections);

	this->m_io = io;
}

asio9::TcpServer::~TcpServer() {

}

void asio9::TcpServer::accept_handler(boost::system::error_code ec, basic_type::socket_type socket)
{
	auto conn = std::make_shared<TcpConnection>(this->m_io, std::move(socket));

	//初始化读取缓存区
	conn->initBuffer(this->m_readBufferSize);

	//Connection开始接收数据
	conn->run();

	//调用回调
	if (this->m_NewConnectedCallBack_)
		this->m_NewConnectedCallBack_(ec, conn);

	//检查错误
	if (ec)
		return;
	else //继续接受连接
		this->do_accept();
}

#endif