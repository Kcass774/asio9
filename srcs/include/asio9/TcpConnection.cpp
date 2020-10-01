#ifndef ASIO9_TCPCONNECTION_CPP_
#define ASIO9_TCPCONNECTION_CPP_

#include "TcpConnection.h"

asio9::TcpConnection::~TcpConnection()
{
}

void asio9::TcpConnection::read_handler(const boost::system::error_code& ec, size_t length)
{
	if (ec.value() == boost::asio::error::eof)//�����������Ͽ�����
		if (this->m_CloseCallBack_)
			this->m_CloseCallBack_(this->shared_from_this());
		else if (this->m_MessageCallBack_)//���ûص�
			m_MessageCallBack_(this->shared_from_this(), (char*)this->m_buf, length);

	//�޴����������������
	if (!ec)
		this->do_read();
}

void asio9::TcpConnection::write_handler(const boost::system::error_code& ec, size_t length, WriteCallBack callback_)
{
	if (callback_)
		callback_(this->shared_from_this(), ec, length);
}

#endif