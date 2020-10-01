#ifndef ASIO9_TCPCLIENT_CPP_
#define ASIO9_TCPCLIENT_CPP_

#include "TcpClient.h"

#include <iostream>

asio9::TcpClient::TcpClient(basic_type::io_type* io)
    :m_socket(*io)
{
    this->m_io = io;
}

asio9::TcpClient::~TcpClient()
{
	if (this->m_pool)//使用内存池
		this->m_pool->free(this->m_buf);
	if (this->m_mallocbuf)//由类分配内存
		free(this->m_buf);
}

void asio9::TcpClient::conn_handler(const boost::system::error_code& ec)
{
	if (this->m_ConnectedCallBack_)
		this->m_ConnectedCallBack_(this->shared_from_this(), ec);
	this->do_read();
}

void asio9::TcpClient::write_handler(const boost::system::error_code& ec, size_t length, WriteCallBack callback_) {
	if (callback_)
		callback_(this->shared_from_this(), ec, length);
}
void asio9::TcpClient::read_handler(const boost::system::error_code& ec, size_t length) {
	if (ec.value() == boost::asio::error::eof)//服务器主动断开连接
		if (this->m_CloseCallBack_)
			this->m_CloseCallBack_(this->shared_from_this());
	else if (this->m_MessageCallBack_)//调用回调
		m_MessageCallBack_(this->shared_from_this(), (char*)this->m_buf, length);

	//无错误则继续接收数据
	if (!ec)
		this->do_read();
};

#endif