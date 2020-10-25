#ifndef ASIO9_TCPSESSION_HPP_
#define ASIO9_TCPSESSION_HPP_

#include <memory>

#include <boost/pool/pool.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace asio9 {
	class TcpSession;

	namespace basic_type {
		typedef boost::asio::io_context io_type;
		typedef boost::asio::ip::tcp::socket socket_type;
		typedef std::shared_ptr<socket_type> sock_ptr;
		typedef boost::asio::ip::tcp::endpoint endpoint_type;
		typedef boost::system::error_code ec_type;
		typedef boost::asio::ip::tcp::acceptor acceptor_type;
		typedef boost::asio::ip::address address_type;
	}

	class TcpSession : public std::enable_shared_from_this<TcpSession>
	{
	public:
		TcpSession(basic_type::io_type* io, basic_type::socket_type socket)
			:m_socket(std::move(socket))
		{
			this->m_io = io;
		}

		~TcpSession()
		{
			if (this->m_pool)//使用内存池
				this->m_pool->free(this->m_buf);
			if (this->m_mallocbuf)//由类分配内存
				free(this->m_buf);
		}

		inline void run() 
		{
			this->init();
			this->m_io->dispatch(std::bind(&TcpSession::do_read, this->shared_from_this()));
		}

		//获取一些东西
		inline basic_type::socket_type* getSocketPtr() { return &this->m_socket; }
		inline basic_type::io_type* getIo_context() { return this->m_io; };

		//关闭连接
		inline void close() {
			this->m_io->post([this]() {
				this->m_socket.close();
				});
		}

		//发送数据
		inline void write(const char* data, const size_t size) {
			this->m_socket.async_write_some(boost::asio::buffer(data, size),
				std::bind(&asio9::TcpSession::after_write, this->shared_from_this(),
					this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
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

		//回调函数
		virtual void init() {}
		virtual void on_message(std::shared_ptr<TcpSession> this_ptr, const char* data, const size_t& size) {}
		virtual void on_close(std::shared_ptr<TcpSession> this_ptr) {}
		virtual void after_write(std::shared_ptr<TcpSession> this_ptr, const basic_type::ec_type& ec, const size_t& size) {}
	private:
		void do_read() {
			this->m_socket.async_read_some(boost::asio::buffer(m_buf, this->m_bufsize),
				std::bind(&asio9::TcpSession::read_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		};

		void read_handler(const boost::system::error_code& ec, size_t length) {
			if (ec.value() == boost::asio::error::eof)//服务器主动断开连接
			{
				this->on_close(this->shared_from_this());
			}

			if (!ec)//检查错误
			{
				this->on_message(this->shared_from_this(), (char*)this->m_buf, length);
				this->do_read();
			}
		};

		basic_type::io_type* m_io;
		basic_type::socket_type m_socket;

		boost::pool<>* m_pool = nullptr;
		bool m_mallocbuf = false;
		char* m_buf = nullptr;
		size_t m_bufsize = 0;
	};
}

#endif

