#ifndef ASIO9_TCPSERVER_H_
#define ASIO9_TCPSERVER_H_

#include <functional>
#include <string>
#include <set>

#include <boost/pool/pool.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "TcpConnection.h"

namespace asio9 {

	class TcpServer 
		: public std::enable_shared_from_this<TcpServer>
	{
	public:
		typedef std::function<void(const boost::system::error_code& ec, TcpConnection::connection_ptr conn)> NewConnectedCallBack;

		TcpServer(basic_type::io_type* io, const basic_type::endpoint_type& endpoint);
		~TcpServer();

		//开始监听
		inline void run() {
			this->m_io->dispatch(std::bind(&TcpServer::do_accept, this->shared_from_this()));
		};

		//停止监听
		inline void Close(basic_type::ec_type* ec) {
			this->m_io->post([this]() {
				this->m_acceptor.close();
				});
		};

		//是否监听
		inline bool isListen() { return this->m_acceptor.is_open(); };

		//设置每个Connection的读取缓冲区
		inline void setReadBufferSize(const size_t& size) { this->m_readBufferSize = size; };

		//新连接回调
		inline void setNewConnectedCallBack(NewConnectedCallBack callback) { this->m_NewConnectedCallBack_ = callback; };

	private:
		inline void do_accept() {
			m_acceptor.async_accept(std::bind(&asio9::TcpServer::accept_handler, this, std::placeholders::_1, std::placeholders::_2));
		};
		void accept_handler(boost::system::error_code ec, basic_type::socket_type socket);

		NewConnectedCallBack m_NewConnectedCallBack_ = nullptr;

		basic_type::io_type* m_io = nullptr;
		basic_type::acceptor_type m_acceptor;

		size_t m_readBufferSize = 4096;
	};
}

#endif