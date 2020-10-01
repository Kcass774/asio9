#ifndef ASIO9_TCPCONNECTION_H_
#define ASIO9_TCPCONNECTION_H_

#include <boost/pool/pool.hpp>

#include "common.h"

namespace asio9 {
	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	public:
		typedef TcpConnection connection_type;
		typedef std::shared_ptr<connection_type> connection_ptr;

		typedef std::function<void(connection_ptr client)> CloseCallBack;
		typedef std::function<void(connection_ptr client, const basic_type::ec_type& ec, const size_t& size)> WriteCallBack;
		typedef std::function<void(connection_ptr client, const char* data, const size_t& size)> MessageCallBack;

		TcpConnection(basic_type::io_type* io, basic_type::socket_type socket)
			:m_socket(std::move(socket))
		{
			this->m_io = io;
		};
		~TcpConnection();

		inline void run() {
			this->m_io->dispatch(std::bind(&TcpConnection::do_read, this->shared_from_this()));
		};

		inline void Write(const char* data, const size_t size, basic_type::ec_type* ec, WriteCallBack callback_) {
			this->m_socket.async_write_some(boost::asio::buffer(data, size),
				std::bind(&asio9::TcpConnection::write_handler, this,
					std::placeholders::_1, std::placeholders::_2, callback_));
		}

		//使用malloc分配内存作为缓存区
		inline void initBuffer(const size_t& size) {
			this->m_bufsize = size;
			this->m_buf = (char*)malloc(size);
			this->m_mallocbuf = true;
		};
		//自定义缓冲区
		inline void initBuffer(char* buf, const size_t& size) {
			this->m_bufsize = size;
			this->m_buf = buf;
		};
		//使用Boost::Pool分配内存作为缓存区
		inline void initBuffer(boost::pool<>* pool) {
			this->m_pool = pool;
			this->m_buf = (char*)pool->malloc();
			this->m_bufsize = pool->get_requested_size();
		}

		inline void setMessageCallBack(MessageCallBack callback) { this->m_MessageCallBack_ = callback; };
		inline void setCloseCallBack(CloseCallBack callback) { this->m_CloseCallBack_ = callback; };

		boost::pool<>* m_pool = nullptr;
		bool m_mallocbuf = false;
		char* m_buf = nullptr;
		size_t m_bufsize = 0;
	private:
		inline void do_read() {
			this->m_socket.async_read_some(boost::asio::buffer(m_buf, this->m_bufsize),
				std::bind(&asio9::TcpConnection::read_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		};

		void read_handler(const boost::system::error_code& ec, size_t length);
		void write_handler(const boost::system::error_code& ec, size_t length, WriteCallBack callback_);

		basic_type::io_type* m_io;
		basic_type::socket_type m_socket;

		CloseCallBack m_CloseCallBack_ = nullptr;
		MessageCallBack m_MessageCallBack_ = nullptr;
	};
}


#endif