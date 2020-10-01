#ifndef ASIO9_TCPCLIENT_H_
#define ASIO9_TCPCLIENT_H_

#include <functional>
#include <string>

#include <boost/pool/pool.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "common.h"

namespace asio9 {
	class TcpClient 
		: public std::enable_shared_from_this<TcpClient>
	{
	public:
		typedef TcpClient client_type;
		typedef std::shared_ptr<client_type> client_ptr;

		typedef std::function<void(client_ptr client, const basic_type::ec_type& ec)> ConnectedCallBack;
		typedef std::function<void(client_ptr client)> CloseCallBack;
		typedef std::function<void(client_ptr client, const basic_type::ec_type& ec, const size_t& size)> WriteCallBack;
		typedef std::function<void(client_ptr client, const char* data, const size_t& size)> MessageCallBack;

		TcpClient(basic_type::io_type* io);
		~TcpClient();

		inline void Connect(const boost::asio::ip::tcp::endpoint& endpoint) {
			this->m_io->dispatch(std::bind(&TcpClient::do_connect, this->shared_from_this(), endpoint));
		};
		inline void Close() {
			this->m_io->post([this]() {
				this->m_socket.close();
				});
		}

		inline void Write(const char* data, const size_t size, basic_type::ec_type* ec, WriteCallBack callback_) {
			this->m_socket.async_write_some(boost::asio::buffer(data, size),
				std::bind(&asio9::TcpClient::write_handler, this,
					std::placeholders::_1, std::placeholders::_2, callback_));
		}

		inline basic_type::socket_type* getSocketPtr() { return &this->m_socket; }
		inline basic_type::io_type* getIo_context() { return this->m_io; };

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
		inline void setConnectCallBack(ConnectedCallBack callback) { this->m_ConnectedCallBack_ = callback; };
	private:
		inline void do_connect(const boost::asio::ip::tcp::endpoint& endpoint) {
			this->m_socket.async_connect(endpoint,
				std::bind(&TcpClient::conn_handler, this->shared_from_this(), std::placeholders::_1));
		};
		inline void do_read() {
			this->m_socket.async_read_some(boost::asio::buffer(m_buf, this->m_bufsize),
				std::bind(&asio9::TcpClient::read_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		};
		
		void conn_handler(const boost::system::error_code& ec);
		void write_handler(const boost::system::error_code& ec, size_t length, WriteCallBack callback_);
		void read_handler(const boost::system::error_code& ec, size_t length);

		basic_type::io_type* m_io;
		basic_type::socket_type m_socket;

		ConnectedCallBack m_ConnectedCallBack_ = nullptr;
		CloseCallBack m_CloseCallBack_ = nullptr;
		MessageCallBack m_MessageCallBack_ = nullptr;

		boost::pool<>* m_pool = nullptr;
		bool m_mallocbuf = false;
		char* m_buf = nullptr;
		size_t m_bufsize = 0;
	};
}

#endif